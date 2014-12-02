#include "globaldatabasemanager.h"
#include <dcx/win32>
#include <time.h>     // For getting formatted time
#include <math.h>
#include <direct.h>
#include "../common/sqlite/sqlite3.h"
#include <dc/debug>
#include "globalgamefilemanager.h"
#include "../shared/server-editor/server_gamefile.h"
#include "../shared/evidyon_avatarinventory.h"
#include "avatarattributes.h"
#include "globalitemmanager.h"
#include "item.h"
#include "../common/safemacros.h"
#include "randint32.h"
#include "../shared/evidyon_accounts.h"
#include "databaseinventoryitem.h"
#include "avatarinventoryitem.h"

#include <direct.h>
#include <map>

// TODO: convert old-style (callback) SQLite queries into new style queries


namespace Evidyon {





DWORD WINAPI DiskDBThread(void* pointer)
{
  GlobalDatabaseManager* db = (GlobalDatabaseManager*)pointer;
  db->executeDiskDBThread();
  return S_OK;
}

GlobalDatabaseManager* GlobalDatabaseManager::singleton_ = NULL;

GlobalDatabaseManager* GlobalDatabaseManager::singleton() {
  return singleton_;
}


//------------------------------------------------------------------------------------------------
// Name: GlobalDatabaseManager
// Desc: 
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::GlobalDatabaseManager()
{
  // Reset the database connection pointer
  myDB = 0;

  // Setting this to 0 when the server starts makes the server back up the database when it
  // starts, regardless of anything else.  This is usually a good idea.
  myLastBackupTime = 0;



  myNextCharacterID = 0;

  myDiskDB = 0;
  myDiskDBThreadHandle = 0;
  myDiskDBThreadWaitEvent = 0;
}



//------------------------------------------------------------------------------------------------
// Name: GlobalDatabaseManager
// Desc: 
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::~GlobalDatabaseManager()
{
  // Make sure the database connection closed
  if (APP_WARNING(myDB != 0)("Management database was still open when the server closed"))
  {
    destroy();
  }
}



//------------------------------------------------------------------------------------------------
// Name: open
// Desc: 
//------------------------------------------------------------------------------------------------
bool GlobalDatabaseManager::create(const char* rootDBName, const char* configurationFilename)
{
  myRootDBName = rootDBName;
  std::string sqldbFileNameString  = std::string(rootDBName) + ".sqldb",
       oldSQLDBFileNameString  = "";
  const char* sqldbFileName = sqldbFileNameString.c_str();
  bool blankDB = true;

  // Move the existing database file to a backup file name
  {
    // Generate the file name to which to move the database
    char file[256], mask[256];
    _mkdir("./backups/");
    sprintf_s(mask, sizeof(mask), "./backups/%s - %%s.sqldb", rootDBName);

    // Find the current time
    tm * loc;
    time_t ltime;
    time(&ltime);
  	  loc = localtime(&ltime);

    // Print the date and time
    char dateTime[32];
	    asctime_s(dateTime, sizeof(dateTime), loc);
    dateTime[strlen(dateTime)-1] = '\0';

    // Create the file
    sprintf_s(file, sizeof(file), mask, dateTime);

    // Remove invalid characters
    for (int i = 0; file[i] != '\0'; ++i)
      if (file[i] == ':')
        file[i] = '\'';

    // Copy to this file
    if (MoveFile(sqldbFileNameString.c_str(), file))
    {
      oldSQLDBFileNameString = file;
      blankDB = false;
    }
  }

  // Read the configuration file and generate the data formats table
  {
    // Open the database for this execution of the server (it shouldn't exit right now)
    //sqlite3_open(sqldbFileNameString.c_str(), &myDB);
    sqlite3_open(sqldbFileNameString.c_str(), &myDiskDB);
    sqlite3_open(":memory:", &myDB);

    // Make sure that the databases were opened correctly
    CONFIRM(myDiskDB && myDB) else { destroy(); return false; }

    // Result codes
    int rc;
    char* error_message;

    // Start writing to the database
    CONFIRM(SQLITE_OK == (rc = sqlite3_executeQuery("BEGIN", &error_message)));

    // Create a table that holds the structure of this database
    //rc = sqlite3_exec(myDB, "CREATE TABLE meta_structure (table_name VARCHAR, field_name VARCHAR, field_type VARCHAR, import_default VARCHAR)", 0, 0, &error_message);
    rc = sqlite3_executeQuery("CREATE TABLE meta_structure (table_name VARCHAR, field_name VARCHAR, field_type VARCHAR, import_default VARCHAR)", &error_message);

    // Read the database structure definition file and use it to (1) fill the meta_structure
    // table with entries and (2) create tables in this database
    FILE* pf = 0;
    CONFIRM(0 == fopen_s(&pf, configurationFilename, "rt")) else { destroy(); return false; }
    bool fileEnded = 0 != feof(pf);
    bool readFirstTableName = false;
    bool readFirstField = false;
    char tableSQL[2048];
    char completeTableSQL[2048];
    char metaStructureInsertSQLMask[1024];
    while(!fileEnded)
    {
      // Character arrays used to read data from the database configuration file
      char line[512];     line[0] = '\0';
      char label[128];    label[0] = '\0';
      char type[128];     type[0] = '\0';
      char importValue[256];  importValue[0] = '\0';
      bool completedTableSQL = false;

      // Read each line of the config file
      fscanf_s(pf, " %[^\n\r]", line, sizeof(line));
      sscanf_s(line, "%s %[^;];%[^\n\r]", label, sizeof(label), type, sizeof(type), importValue, sizeof(importValue));

      // If the table's SQL is complete (i.e. we've started reading a different table), copy the statement over
      if (readFirstTableName && (type[0] == '\0'))
      {
        strcpy_s(completeTableSQL, sizeof(completeTableSQL), tableSQL);
        completedTableSQL = true;
      }

      // If this is a table header, create table information
      if (type[0] == '\0')
      {
        // This string builds the table-creation SQL
        sqlite3_snprintf(sizeof(tableSQL), tableSQL, "CREATE TABLE %Q (", label);

        // Create the insert mask for meta_structure entries related to this table
        sqlite3_snprintf(sizeof(metaStructureInsertSQLMask), metaStructureInsertSQLMask, "INSERT INTO meta_structure VALUES (%Q,%%Q,%%Q,%%Q)", label);

        // Reset the flag that tells the app whether or not it's read the first field entry
        // (this is used to control commas)
        readFirstField = false;
      }
      else
      {
        char sql[1024];

        // Add this field name to the table SQL
        sprintf_s(sql, sizeof(sql), "%s%s %s", readFirstField ? ", " : "", label, type);
        strcat_s(tableSQL, sizeof(tableSQL), sql);
        readFirstField = true;

        // Employ the insert mask to generate an SQL statement to insert the field format into the meta table.
        sqlite3_snprintf(sizeof(sql), sql, metaStructureInsertSQLMask, label, type, importValue);

        // Insert into our table_structure table
        rc = sqlite3_executeQuery(sql, &error_message);
        if (APP_ERROR(rc != SQLITE_OK)("Problem inserting table entry into meta_structure table:  %s (sql=%s)", error_message, sql))
        {
          fclose(pf);
          destroy();
          //sqlite3_close(myDB);
          //sqlite3_close(myDiskDB);
          return false;
        }
      }

      // If the file ends, we're done reading
      if (fileEnded = (0 != feof(pf)))
      {
        strcpy_s(completeTableSQL, sizeof(completeTableSQL), tableSQL);
        completedTableSQL = true;
      }

      // If the file is finished or this type is not a database, add the current
      // table entry to the 
      if (completedTableSQL)
      {
        // Terminate the table-creation SQL
        strcat_s(completeTableSQL, sizeof(completeTableSQL), ")");

        // Execute this SQL
        //rc = sqlite3_exec(myDB, completeTableSQL, 0, 0, &error_message);
        rc = sqlite3_executeQuery(completeTableSQL, &error_message);
        if (APP_ERROR(rc != SQLITE_OK)("Problem creating table:  %s (sql=%s)", error_message, completeTableSQL))
        {
          fclose(pf);
          destroy();
          //sqlite3_close(myDB);
          //sqlite3_close(myDiskDB);
          return false;
        }
      }

      // We've now read the name of the first table
      readFirstTableName = true;
    }

    // Close the configuration file
    fclose(pf);
  }

  // Perform import if a previous database exists
  if (!blankDB)
  {
    // Open the previous database
    sqlite3* previousDB;
    sqlite3_open(oldSQLDBFileNameString.c_str(), &previousDB);

    char sql[1024];

    // Query for each of the tables in the new database
    sqlite3_snprintf(sizeof(sql), sql, "SELECT DISTINCT table_name FROM meta_structure");
    sqlite3_stmt* newTables;
    sqlite3_prepare_v2(myDB, sql, sizeof(sql), &newTables, 0);

    // Iterate through all of the results
    while(SQLITE_ROW == sqlite3_step(newTables)) {
      // Query for fields of this table in the new and old databases
      sqlite3_stmt* oldTableFields;
      sqlite3_stmt* newTableFields;
      const char* tableName = (const char*)sqlite3_column_text(newTables, 0);
      sqlite3_snprintf(sizeof(sql), sql, "SELECT field_name, field_type, import_default FROM meta_structure WHERE table_name=%Q", tableName);
      sqlite3_prepare_v2(previousDB, sql, sizeof(sql), &oldTableFields, 0);
      sqlite3_prepare_v2(myDB, sql, sizeof(sql), &newTableFields, 0);

      struct Field
      {
        std::string field_name;
        std::string field_type;
        std::string import_default;
      };

      // Add all of the fields from the databases into a list
      typedef std::map<std::string,Field> FieldTable;
      FieldTable newFields;
      while(SQLITE_ROW == sqlite3_step(newTableFields))
      {
        // Add this to the new fields
        Field field;
        field.field_name   = (const char*)sqlite3_column_text(newTableFields, 0);
        field.field_type   = (const char*)sqlite3_column_text(newTableFields, 1);
        field.import_default = (const char*)sqlite3_column_text(newTableFields, 2);
        newFields.insert(FieldTable::value_type(field.field_name, field));
      }

      // As long as this entry exists in the new database, add it
      FieldTable oldFields;
      while(SQLITE_ROW == sqlite3_step(oldTableFields))
      {
        // Add this to the new fields
        Field field;
        field.field_name   = (const char*)sqlite3_column_text(oldTableFields, 0);
        field.field_type   = (const char*)sqlite3_column_text(oldTableFields, 1);
        field.import_default = (const char*)sqlite3_column_text(oldTableFields, 2);
        oldFields.insert(FieldTable::value_type(field.field_name, field));
      }

      // Finalize the queries
      sqlite3_finalize(newTableFields);
      sqlite3_finalize(oldTableFields);

      // Remove any fields that exist only in the old database
      for (FieldTable::iterator i = oldFields.begin(); i != oldFields.end();)
      {
        if (newFields.find(i->first) == newFields.end())
        {
          FieldTable::iterator next = i; next++;
          oldFields.erase(i);
          i = next;
        }
        else
          ++i;
      }

      // Removes any fields that exist in the old database (leaving only newly created ones)
      for (FieldTable::iterator i = newFields.begin(); i != newFields.end();)
      {
        if (oldFields.find(i->first) != oldFields.end())
        {
          FieldTable::iterator next = i; next++;
          newFields.erase(i);
          i = next;
        }
        else
          ++i;
      }

      // Query for old fields
      strcpy_s(sql, sizeof(sql), "SELECT ");
      for (FieldTable::iterator i = oldFields.begin(); i != oldFields.end(); ++i)
        strcat_s(sql, sizeof(sql), (std::string((i == oldFields.begin()) ? "" : ",") + i->first).c_str());
      strcat_s(sql, sizeof(sql), " FROM ");
      strcat_s(sql, sizeof(sql), tableName);
      strcat_s(sql, sizeof(sql), " WHERE 1");
      sqlite3_stmt* oldFieldValues;
      sqlite3_prepare_v2(previousDB, sql, sizeof(sql), &oldFieldValues, 0);

      // If we couldn't prepare the old fields values statement, warn about this.  It's
      // likely, however, that we are creating a new database
      if (APP_WARNING(!oldFieldValues)("Couldn't query for old fields from '%s'; this is OK if it's a new table", tableName))
        continue;

      // Build an insert statement to transform the results into 
      char insertStatementFields[1024];
      char insertStatementValues[1024];
      sqlite3_snprintf(sizeof(insertStatementFields), insertStatementFields, "INSERT INTO %s (", tableName);
      sqlite3_snprintf(sizeof(insertStatementValues), insertStatementValues, ") VALUES (");

      // Add the default values first, so we can assign them directly
      bool insertedFieldsFirst = false, insertedValuesFirst = false;
      for (FieldTable::iterator i = newFields.begin(); i != newFields.end(); ++i) {
        strcat_s(insertStatementFields,
               (std::string(insertedFieldsFirst ? ", " : "") + i->first).c_str());
        strcat_s(insertStatementValues,
               (std::string(insertedValuesFirst ? ", " : "") + i->second.import_default).c_str());
        insertedFieldsFirst = true;
        insertedValuesFirst = true;
      }

      // Add the field names that will be pulled from the old database
      for (FieldTable::iterator i = oldFields.begin(); i != oldFields.end(); ++i)
      {
        strcat_s(insertStatementFields, sizeof(insertStatementFields), (std::string(insertedFieldsFirst ? "," : "") + i->first).c_str());
        insertedFieldsFirst = true;
      }

      // Add the default values and such
      strcat_s(insertStatementFields, sizeof(insertStatementFields), insertStatementValues);

      // Query the old database table and insert into the new database for each element
      bool insertedValuesFirstInTemplate = insertedValuesFirst;
      while(SQLITE_ROW == sqlite3_step(oldFieldValues))
      {
        // Copy the old string
        strcpy_s(sql, sizeof(sql), insertStatementFields);
        insertedValuesFirst = insertedValuesFirstInTemplate;

        // Add each field values
        int index = 0;
        for (FieldTable::iterator i = oldFields.begin(); i != oldFields.end(); ++i, ++index)
        {
          if (insertedValuesFirst)
            strcat_s(sql, sizeof(sql), ", ");
          char value[256];
          sqlite3_snprintf(sizeof(value), value, "%Q", (const char*)sqlite3_column_text(oldFieldValues, index));
          strcat_s(sql, sizeof(sql), value);
          insertedValuesFirst = true;
        }

        // Complete the statement
        strcat_s(sql, sizeof(sql), ")");

        // Execute the insertion
        char* error_message;
        //int rc = sqlite3_exec(myDB, sql, 0, 0, &error_message);
        int rc = sqlite3_executeQuery(sql, &error_message);
        if (APP_ERROR(rc != SQLITE_OK)("Couldn't insert into DB:  %s  (sql=%s)", error_message, sql))
          return false; // todo: cleanup
      }

      // Close the query
      sqlite3_finalize(oldFieldValues);
    }
    sqlite3_finalize(newTables);

    // Close the old database
    sqlite3_close(previousDB);
  }

  { // Create a statement to pick the maximum character ID
  sqlite3_stmt* stmt;
  const char query[] = "SELECT MAX(character_id) FROM characters";
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  int rc = sqlite3_step(stmt);

  // Make sure this succeeded
  if (!APP_WARNING(rc != SQLITE_ROW)("Unable to get maximum character ID: unless this is the first time the server is being run, new characters may fail to be created"))
  {
    myNextCharacterID = sqlite3_column_int(stmt,0);
  }
  else
    myNextCharacterID = 0;

  DEBUG_INFO("%lu characters have been created on the server", myNextCharacterID);

  // Erase the statement structure memory
  sqlite3_finalize(stmt);
  }

  { // import all character names into the unique name function
    sqlite3_stmt* stmt;
    const char query[] = "SELECT name FROM characters WHERE account_id != 0";
    sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      std::string name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
      unique_avatar_name_checker_.addName(name);
    }
    sqlite3_finalize(stmt);

    // nobody can use these reserved names
    unique_avatar_name_checker_.addName("evidyon");
    unique_avatar_name_checker_.addName("geosid");
    unique_avatar_name_checker_.addName("genoite");
    unique_avatar_name_checker_.addName("karl");
    unique_avatar_name_checker_.addName("karlgluck");
    unique_avatar_name_checker_.addName("evidyon");
    unique_avatar_name_checker_.addName("volucris");
    unique_avatar_name_checker_.addName("projectvolucris");
    unique_avatar_name_checker_.addName("ej");
    unique_avatar_name_checker_.addName("thayer");
    unique_avatar_name_checker_.addName("ejthayer");
  }

  // Commit changes to the database
  CONFIRM(SQLITE_OK == (sqlite3_executeQuery("COMMIT", 0)));

  // Close the disk database so that the thread can take over
  sqlite3_close(myDiskDB);
  myDiskDB = 0;

  // Create the disk database thread info
  CONFIRM(myDiskDBQueriesToExecute.create()) else return false;
  myDiskDBThreadWaitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  myDiskDBThreadHandle = CreateThread(NULL, 0, DiskDBThread, this, THREAD_PRIORITY_BELOW_NORMAL|CREATE_SUSPENDED, NULL);

  // Start watching for messages and create the disk DB.
  ResumeThread(myDiskDBThreadHandle);

  singleton_ = this;

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name: close
// Desc: 
//------------------------------------------------------------------------------------------------
void GlobalDatabaseManager::destroy()
{
  singleton_ = NULL;

  if (myDB != 0)
  {
    sqlite3_close(myDB);
    myDB = 0;
  }

  if (myDiskDBThreadWaitEvent)
  {
    SetEvent(myDiskDBThreadWaitEvent);
    WaitForSingleObject(myDiskDBThreadHandle, 25000);
    CloseHandle(myDiskDBThreadWaitEvent);
    CloseHandle(myDiskDBThreadHandle);
  }

  if (myDiskDB != 0)
  {
    sqlite3_close(myDiskDB);
    myDiskDB = 0;
  }

  myDiskDBQueriesToExecute.destroy();
}


/*
//------------------------------------------------------------------------------------------------
// Name: needsBackup
// Desc: 
//------------------------------------------------------------------------------------------------
bool GlobalDatabaseManager::needsBackup()
{
  // Back up the account database every hour
  return dcxClock::getSystemTime() - myLastBackupTime > (1000 * 60 * 60);
}



//------------------------------------------------------------------------------------------------
// Name: backup
// Desc: 
//------------------------------------------------------------------------------------------------
bool GlobalDatabaseManager::backup()
{
  return false;//unimplemented
}

*/

//------------------------------------------------------------------------------------------------
// Name: isValidAccountNumber
// Desc: Determines whether or not the provided account number is valid
//------------------------------------------------------------------------------------------------
bool GlobalDatabaseManager::isValidAccountNumber(unsigned int id)
{
  return !(id < 1000000 || id > 9999999);
}


//------------------------------------------------------------------------------------------------
// Name: createAccount
// Desc: Tries to create an account with the given settings
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::createAccount(const char* password, unsigned int* id)
{
  // These variables are used during SQL queries
  int rc;
  char* error_message;

  // This is the ID number of the created account
  unsigned int idNumber;

  // Calculate the expiration date for this new account
  __int64 expirationDate = 0;
  {
    time_t timeIn14Days;
    time(&timeIn14Days);
    timeIn14Days += 60 * 60 * 24 * 14;
    expirationDate = (__int64)timeIn14Days;
  }

  // Build the SQL query.  The %Q option inserts a string from the parameter list in which every
  // "'" character is doubled, and the entire phrase is surrounded by single-quotes
  do
  {
    // Create a random ID number with 7 digits
    char idNumberString[8]; idNumberString[0] = '\0';
    for (int i = 0; i < 7; ++i)
    {
      char number[2];
      sprintf_s(number, sizeof(number), "%i", rand()%10);
      strcat_s(idNumberString, sizeof(idNumberString), number);
    }

    // Get the account ID number
    idNumber = (unsigned int)atoi(idNumberString);

    // Make sure the number is in the right range
    if (!isValidAccountNumber(idNumber))
      continue;

    // Print the integer
    char expirationDateString[64];
    sprintf_s(expirationDateString, sizeof(expirationDateString), "%I64d", expirationDate);

    // Build the query
    char query[256];
    sqlite3_snprintf(sizeof(query), query, "INSERT INTO accounts (account_id,password,time_expiration_date) VALUES (%lu,%Q,%s)", idNumber, password, expirationDateString);

    // Execute the query
    //rc = sqlite3_exec(myDB, query, 0, 0, &error_message);
    rc = sqlite3_executeQuery(query, &error_message);

    // If we get a constraint error, that means we didn't generate a unique account number.  Just
    // run the generation routine again.
    if (rc == SQLITE_CONSTRAINT) continue; 

    // Any other errors in the database are fatal
    if (APP_ERROR(rc != SQLITE_OK)("Couldn't execute query (%s) during createAccount:  %s", query, error_message))
      return DBR_INTERNALERROR;

  } while(0 == sqlite3_total_changes(myDB));

  // Save the account's ID
  *id = idNumber;

  // Success!
  return DBR_SUCCESS;
}



//------------------------------------------------------------------------------------------------
// Name: GlobalDatabaseManager_accessAccount
// Desc: Callback for the SQL query in ProjectVolucirsManagementDB::accessAccount 
//------------------------------------------------------------------------------------------------
int GlobalDatabaseManager_accessAccount(void * passwordVoid, int argc, char** argv, char** cols)
{
  // Get the password parameter
  std::string* password = (std::string*)passwordVoid;

  // Obtain the value of the password column
  for (int i = 0; i < argc; ++i)
  {
    if (0 == _stricmp(cols[i], "password"))
    {
      password->assign(argv[i]);
      break;
    }
  }

  // Success
  return 0;
}



//------------------------------------------------------------------------------------------------
// Name: accessAccount
// Desc: Attempts to access the account with the provided ID number
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::accessAccount(unsigned int id, const char* password)
{
  // Fail automatically for bad account numbers
  if (!isValidAccountNumber(id)) return DBR_DOESNTEXIST;

  // Build the SQL query
  char query[128];
  sqlite3_stmt* stmt;
  sqlite3_snprintf(sizeof(query), query, "SELECT password FROM accounts WHERE account_id=%lu", id);
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);

  // Execute the query
  switch(sqlite3_step(stmt))
  {
    case SQLITE_ROW:
    {
      // Compare the password
      DBResult retval = DBR_INCORRECTPASSWORD;
      const char* passwordFromDatabase = (const char*)sqlite3_column_text(stmt, 0);
      if ((passwordFromDatabase != 0) &&
        (0 == strcmp(passwordFromDatabase, password)))
        retval = DBR_SUCCESS;

      // Erase the query
      sqlite3_finalize(stmt);

      // Return the result
      return retval;
    }

    default:
      return DBR_DOESNTEXIST;
      // TODO: return DBR_INTERNALERROR
  }
}



//------------------------------------------------------------------------------------------------
// Name: getCharactersOnAccount
// Desc: Gets a list of all of the characters that are linked to the account with the given ID
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::getCharactersOnAccount(
  unsigned int accountID, unsigned int* characterIDArray, size_t* numberOfCharacters)
{
  // Fail for bad account numbers
  if (!isValidAccountNumber(accountID)) return DBR_DOESNTEXIST;

  // Build the SQL query
  char query[128];
  sqlite3_snprintf(sizeof(query), query, "SELECT character_id FROM characters WHERE account_id=%lu", accountID);

  // Create a statement
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);

  // Stores the number of characters in the account
  size_t characters = 0;

  // Reset the output parameter
  *numberOfCharacters = 0;

  // Iterate through all of the results
  int rc;
  while(SQLITE_DONE != (rc = sqlite3_step(stmt)))
  {
    switch(rc)
    {
      // The DB is returning a row of data
      case SQLITE_ROW:

        // Make sure that we haven't obtained too many results
        if (!APP_ERROR(characters >= MAX_AVATARS_PER_ACCOUNT)("Account %lu has too many characters!", accountID))
        {
          // Get the value of the character's ID from the query
          sscanf_s((char*)sqlite3_column_text(stmt, 0), "%lu", &characterIDArray[characters]);

          // Increment the number of characters
          characters++;
        }

        break;

      // If we get here, something went wrong with the query
      default:
        *numberOfCharacters = 0;
        sqlite3_finalize(stmt);
        return DBR_INTERNALERROR;
    }
  }

  // Finalize the statement
  *numberOfCharacters = characters;
  sqlite3_finalize(stmt);

  // Success
  return DBR_SUCCESS;
}




//------------------------------------------------------------------------------------------------
// Name: createCharacter
// Desc: Tries to create a character with the provided attributes
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::createCharacter(
  unsigned int accountID, const char* name, unsigned int* characterID, float worldX, float worldY,
  bool male, int chRace, int chClass, int level, int hp, int mp, int attributes[5])
{
  // Fail for bad account numbers
  if (!isValidAccountNumber(accountID)) return DBR_DOESNTEXIST;

  // Make sure the name doesn't already exist
  if (!unique_avatar_name_checker_.isNameAvailable(std::string(name))) return DBR_ALREADYEXISTS;

  // Get the next available character ID
  unsigned int newCharacterID = ++myNextCharacterID;

  // Build the SQL query
  char query[2048];
  strcpy_s(query, sizeof(query), "INSERT INTO characters (account_id, character_id, name, world_x, world_y, gender, race, class, "\
                   "level, hp_max, hp_current, mp_max, mp_current, exp_current, str, agi, con, "\
                   "int, wis, exp_current) VALUES (");

  {
    char sql[256];
    sqlite3_snprintf(sizeof(sql), sql, "%lu,%lu,%Q", accountID, newCharacterID, name);
    strcat_s(query, sizeof(query), sql);
    sqlite3_snprintf(sizeof(sql), sql, ",%.02f,%.02f,%i", worldX, worldY, (male ? 0 : 1));
    strcat_s(query, sizeof(query), sql);
    sqlite3_snprintf(sizeof(sql), sql, ",%i,%i,%i", chRace, chClass, level);
    strcat_s(query, sizeof(query), sql);
    sqlite3_snprintf(sizeof(sql), sql, ",%i,%i,%i,%i,0", hp, hp, mp, mp); // last one is EXP
    strcat_s(query, sizeof(query), sql);
    sqlite3_snprintf(sizeof(sql), sql, ",%i,%i,%i,%i,%i", attributes[0], attributes[1], attributes[2], attributes[3], attributes[4]);
    strcat_s(query, sizeof(query), sql);
    sqlite3_snprintf(sizeof(sql), sql, ",%i)", 0);
    strcat_s(query, sizeof(query), sql);
  }

  DEBUG_INFO("query:  %s", query);

  int rc = sqlite3_executeQuery(query, 0);
/*
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);

  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);*/

  switch(rc)
  {
    case SQLITE_OK:
    case SQLITE_DONE:

      // Set the character's ID
      *characterID = newCharacterID;

      return DBR_SUCCESS;

    // This character (or the ID!) already exists
    case SQLITE_CONSTRAINT:
      return DBR_ALREADYEXISTS;

    default:
      return DBR_INTERNALERROR;
  }
}


//------------------------------------------------------------------------------------------------
// Name: removeCharacterFromAccount
// Desc: Unlinks a character from the account
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::removeCharacterFromAccount(unsigned int accountID, unsigned int characterID)
{
  // Fail for bad account numbers
  if (!isValidAccountNumber(accountID)) return DBR_DOESNTEXIST;

  // These variables are used during SQL queries
  int rc;
  char* error_message;

  char name_buffer[512];
  if (DBR_SUCCESS != getCharacterName(accountID, characterID, name_buffer, sizeof(name_buffer))) {
    return DBR_INTERNALERROR;
  }

  std::string deleted_name = name_buffer;
  deleted_name.append(" [deleted from ");

  { // append the source account
    char account_number[16];
    sprintf_s(account_number, sizeof(account_number), "%lu", accountID);
    deleted_name.append(account_number);
    deleted_name.append(" ");
  }

  { // Append the current date and time to the name to make it unique
    tm * loc;
    time_t ltime;
    time(&ltime);
	  loc = localtime(&ltime);
    char dateTime[32];
	  asctime_s(dateTime, sizeof(dateTime), loc);
    dateTime[strlen(dateTime)] = '\0';
    deleted_name.append(dateTime);
    deleted_name.append("]");
  }

  // Build the SQL query
  char query[1024];
  sqlite3_snprintf(sizeof(query),
           query,
           "UPDATE characters SET name='%s', account_id=0 WHERE account_id=%lu AND character_id=%lu",
           deleted_name.c_str(),
           accountID,
           characterID);

  // Execute the query
  rc = sqlite3_executeQuery(query, &error_message);

  // Check to make sure the operation completed successfully
  if (APP_ERROR(rc != SQLITE_OK)("Query (%s) during removeCharacterFromAccount failed:  %s", query, error_message))
    return DBR_INTERNALERROR;

  // Success
  return DBR_SUCCESS;
}



//------------------------------------------------------------------------------------------------
// Name: GlobalDatabaseManager_getAccountTimeLeft_callback
// Desc: 
//------------------------------------------------------------------------------------------------
int GlobalDatabaseManager_getAccountTimeLeft_callback(void* timeExpirationDateVoid, int argc, char** argv, char** cols)
{
  __int64 timeExpirationDate;
  sscanf_s(argv[0], "%I64d", &timeExpirationDate);

  // Calculate the difference in times
  *((time_t*)timeExpirationDateVoid) = (time_t)timeExpirationDate;

  // Exit
  return 0;
}



//------------------------------------------------------------------------------------------------
// Name: getAccountTimeLeft
// Desc: Gets the amount of play time that an account has free
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::getAccountTimeLeft(unsigned int accountID, float* daysLeft)
{
  // Check parameter
  if (APP_ERROR(!daysLeft)("Invalid daysLeft parameter to getAccountTimeLeft")) return DBR_INTERNALERROR;

  // Fail quickly for bad account numbers
  if (!isValidAccountNumber(accountID)) return DBR_DOESNTEXIST;

  // These variables are used during SQL queries
  int rc;
  char* error_message;

  // Build the SQL query
  char query[128];
  sqlite3_snprintf(sizeof(query), query,
    "SELECT time_expiration_date FROM accounts WHERE account_id=%lu", accountID);

  // The time value stored in the database
  time_t timeExpirationDate = 0;

  // Execute the query
  rc = sqlite3_exec(myDB, query, GlobalDatabaseManager_getAccountTimeLeft_callback, &timeExpirationDate, &error_message);

  // Check to make sure the operation completed successfully
  if (APP_ERROR(rc != SQLITE_OK)("Query (%s) during getAccountTimeLeft failed:  %s", query, error_message))
    return DBR_INTERNALERROR;

  // Check to make sure the time exists
  if (timeExpirationDate == 0)
    return DBR_DOESNTEXIST;

  // Calculate how many days are left
  double doubleDaysLeft = difftime(timeExpirationDate, time(0)) / (60.0 * 60.0 * 24.0);

  // Assign to the lower-capacity output parameter
  if (doubleDaysLeft < -9999.0)    *daysLeft = -9999.0f;
  else if (doubleDaysLeft > 99999.0) *daysLeft = 99999.0f;
  else                *daysLeft = (float)doubleDaysLeft;

  *daysLeft = 14.0f;

  // Success
  return DBR_SUCCESS;

}



//------------------------------------------------------------------------------------------------
// Name: getCharacterName
// Desc: Gets the name of the character with the given ID
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::getCharacterName(unsigned int accountID, unsigned int characterID, char* nameBuffer, size_t nameBufferLength)
{
  if (APP_ERROR(!nameBuffer)("Invalid parameter to getCharacterName")) return DBR_INTERNALERROR;

  // Build the SQL query
  char query[128];
  sqlite3_snprintf(sizeof(query), query,
    "SELECT name FROM characters WHERE account_id=%lu AND character_id=%lu", accountID, characterID);

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);

  if (sqlite3_step(stmt) == SQLITE_ROW)
  {
    // Get the value of the character's name from the query
    strncpy_s(nameBuffer,
              nameBufferLength,
              (char*)sqlite3_column_text(stmt, 0),
              nameBufferLength - 1);
    sqlite3_finalize(stmt);
    return DBR_SUCCESS;
  }
  else
  {
    // If we get here, something went wrong with the query
    nameBuffer[0] = 0;
    sqlite3_finalize(stmt);
    return DBR_DOESNTEXIST;
  }
}


//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::getCharacterDesc(
    unsigned int account_id,
    unsigned int character_id,
    char* name_buffer,
    size_t name_buffer_length,
    ActorTypeIndex* actor_type,
    size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT],
    int* map_index,
    float* world_x,
    float* world_y) {

  CONFIRM(name_buffer && name_buffer_length && actor_type && visible_equipment && world_x && world_y)
    else return DBR_DOESNTEXIST;

  // Get world info
  char query[1024];
  sqlite3_snprintf(sizeof(query), query,
  "SELECT world_x, world_y, race, gender, name, map_index FROM characters WHERE account_id=%lu AND character_id=%lu",
  account_id, character_id);
  sqlite3_stmt* statement;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &statement, 0);
  if (sqlite3_step(statement) != SQLITE_ROW) {
    sqlite3_finalize(statement);
    return DBR_DOESNTEXIST;
  }

  *world_x = (float)sqlite3_column_double(statement, 0);
  *world_y = (float)sqlite3_column_double(statement, 1);
  *actor_type = GlobalGamefileManager::singleton()->getCharacterActorTypeIndex(
      sqlite3_column_int(statement, 2),  // race index
      sqlite3_column_int(statement, 3)); // gender index
  strncpy_s(name_buffer,
            MAX_AVATAR_NAME_LENGTH,
            (const char*)sqlite3_column_text(statement, 4),
            MAX_AVATAR_NAME_LENGTH - 1);
  *map_index = sqlite3_column_int(statement, 4);

  sqlite3_finalize(statement);

  // Erase all members
  memset(visible_equipment, 0xFFFFFFFF, sizeof(size_t) * ACTORATTACHMENTPOINT_COUNT);

  //// Get equipment info
  //// Query the item database for this character's inventory
  //sqlite3_snprintf(sizeof(query), query,
  //  "SELECT type FROM items WHERE owner_character_id=%lu AND location=%lu", character_id, (int)(ITEMLOCATION_EQUIPPED));
  //sqlite3_prepare_v2(myDB, query, sizeof(query), &statement, 0);
  //while(sqlite3_step(statement) == SQLITE_ROW) {

  //unsigned int type = sqlite3_column_int(statement, 0);
  //const ServerItemDescription* itemDesc = GlobalGamefileManager::singleton()->getItemDescription(type);
  //CONFIRM(itemDesc) else continue;

  //// Is this a visible item?
  //if (itemDesc->equipped_attachment_point >= 0 && itemDesc->equipped_attachment_point < ACTORATTACHMENTPOINT_COUNT)
  //  visible_equipment[itemDesc->equipped_attachment_point] = itemDesc->equipped_scenery_index;
  //}

  //sqlite3_finalize(statement);

  return DBR_SUCCESS;
}




//------------------------------------------------------------------------------------------------
// Name: getCharacterData
// Desc: 
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::getCharacterData(unsigned int accountID,
                                                                        unsigned int characterID,
                                                                        float* x,
                                                                        float* y,
                                                                        AvatarAttributes* character,
                                                                        AvatarInventoryItem* inventory,
                                                                        int* map_index) {
  // Build the SQL query
  char query[2048];
  sqlite3_snprintf(sizeof(query), query,
    "SELECT world_x, world_y, gender, race, class, level, hp_max, hp_current, "\
    "mp_max, mp_current, exp_current, str, agi, con, int, wis, name, "\
    "ability_pts, money, map_index, geonite FROM characters "\
    "WHERE account_id=%lu AND character_id=%lu", accountID, characterID);

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);

  if (sqlite3_step(stmt) == SQLITE_ROW)
  {
    GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();

    // Fill out information using data from the database
    *x = (float)sqlite3_column_double(stmt, 0);
    *y = (float)sqlite3_column_double(stmt, 1);
    character->genderIndex = sqlite3_column_int(stmt, 2);
    character->raceIndex = sqlite3_column_int(stmt, 3);
    character->classIndex = sqlite3_column_int(stmt, 4);
    character->base.level = sqlite3_column_int(stmt, 5);
    character->hp = sqlite3_column_int(stmt, 7);
    character->mp = sqlite3_column_int(stmt, 9);
    character->currentEXP = sqlite3_column_int(stmt, 10);
    character->stats[0] = sqlite3_column_int(stmt, 11);
    character->stats[1] = sqlite3_column_int(stmt, 12);
    character->stats[2] = sqlite3_column_int(stmt, 13);
    character->stats[3] = sqlite3_column_int(stmt, 14);
    character->stats[4] = sqlite3_column_int(stmt, 15);
    strncpy_s(character->name,
              MAX_AVATAR_NAME_LENGTH,
              (const char*)sqlite3_column_text(stmt, 16),
              MAX_AVATAR_NAME_LENGTH-1);
    character->availableStatPoints = sqlite3_column_int(stmt, 17);
    character->money = sqlite3_column_int(stmt, 18);
    *map_index = sqlite3_column_int(stmt, 19);
    character->geonite = sqlite3_column_int(stmt, 20);

    //// get the character's stats
    //gamefile_manager->calculateAvatarAttributes(character->classIndex,
    //                                            character->base.level,
    //                                            character->stats,
    //                                           &character->base.max_hp,
    //                                           &character->base.max_mp,
    //                                           &character->base.hp_regen_rate,
    //                                           &character->base.mp_regen_rate,
    //                                           &character->base.speed,
    //                                           &character->base.attack_delay_time,
    //                                           &character->base.min_attack_damage,
    //                                           &character->base.random_attack_damage);
    gamefile_manager->calculateCoreAvatarAttributes(character);

    character->base.actor_type = gamefile_manager->getCharacterActorTypeIndex(
        character->raceIndex,
        character->genderIndex);


    character->next_level_EXP =
      gamefile_manager->getLevelUpExperience(character->base.level);
    character->base.physical_defense = 0;
    character->base.magical_defense = 0;

    // Copy the base structure into the "current" one
    memcpy(&character->current,
           &character->base,
           sizeof(character->current));

    // Erase the query structure
    sqlite3_finalize(stmt);
  }
  else
  {
    // If we get here, it wasn't found or something is wrong with the database
    // TODO: checkhe return code for SQLITE_DONE vs. error code
    sqlite3_finalize(stmt);
    return DBR_DOESNTEXIST;
  }


  // Query the item database for this character's inventory
  sqlite3_snprintf(sizeof(query), query,
    "SELECT item_id, type_hash, quantity, equipped "\
    "FROM items "\
    "WHERE owner_character_id=%lu "\
    "ORDER BY equipped", characterID);

  // Read all of the items out of the database
  GlobalItemManager* item_manager = GlobalItemManager::singleton();
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  int inventoryIndex = 0;
  while(sqlite3_step(stmt) == SQLITE_ROW) {
    DatabaseInventoryItem database_item;
    database_item.id = (ItemIDType)sqlite3_column_int64(stmt, 0); // unique id #
    database_item.permanent_storage_hash = (unsigned)sqlite3_column_int(stmt, 1);
    database_item.quantity = sqlite3_column_int(stmt, 2);
    database_item.equipped = 0 > sqlite3_column_int(stmt, 3) ? true : false;

    // Translate this item
    if (item_manager->acquireAvatarItemForDatabaseItem(&database_item,
                                                       &inventory[inventoryIndex])) {
        ++inventoryIndex;
    }
  }

  // Invalidate the rest of the entries in the inventory
  while (inventoryIndex < AVATAR_INVENTORY_SIZE) {
    inventory[inventoryIndex].equipped  = false;
    inventory[inventoryIndex].stackable = false;
    inventory[inventoryIndex].item      = NULL;
    ++inventoryIndex;
  }

  // Finalize the statement
  sqlite3_finalize(stmt);


  //// Query the item database for this character's inventory
  //sqlite3_snprintf(sizeof(query), query,
  //  "SELECT item_id, location, type, quantity FROM items WHERE owner_character_id=%lu", characterID);



  //// Read all of the items out of the database
  //sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  //int inventoryIndex = 0;
  //while(sqlite3_step(stmt) == SQLITE_ROW)
  //{
  //  // Skip this item if it isn't carried by the character
  //  ItemLocation location = (ItemLocation)sqlite3_column_int(stmt, 1);
  //  if (location != ITEMLOCATION_EQUIPPED && location != ITEMLOCATION_INVENTORY) continue;

  //  // Save the item's data in the inventory
  //  AvatarInventoryItem* item = &inventory[inventoryIndex];
  //  unsigned short hashIndex;
  //  Item* managedItem = GlobalItemManager::singleton()->acquireStructForExistingItem(
  //    (ItemIDType)sqlite3_column_int64(stmt, 0),  // unique id
  //    (unsigned)sqlite3_column_int(stmt, 3),    // quantity
  //    (unsigned)sqlite3_column_int(stmt, 2),    // type
  //    &hashIndex);
  //  ASSERT(managedItem) else { item->invalid = 1; continue; }
  //  item->itemHashIndex = hashIndex;
  //  managedItem->fill(item);
  //  item->equipped = location == ITEMLOCATION_EQUIPPED;
  //  item->invalid = 0;
  //  item->inventoryIndex = inventoryIndex;
  //  ++inventoryIndex;
  //}

  //// Invalidate the rest of the entries
  //while(inventoryIndex < AVATAR_INVENTORY_SIZE) {
  //  inventory[inventoryIndex].invalid = 1;
  //  ++inventoryIndex;
  //}

  //// Finalize the statement
  //sqlite3_finalize(stmt);

  // Success
  return DBR_SUCCESS;

}





//------------------------------------------------------------------------------------------------
// Name: commitCharacterData
// Desc: Saves information about a character into the database
//------------------------------------------------------------------------------------------------
GlobalDatabaseManager::DBResult GlobalDatabaseManager::commitCharacterData(
  unsigned int accountID, unsigned int characterID, float x, float y, AvatarAttributes* character,
  AvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE], int map_index)
{
  // Fail for bad account numbers
  if (!isValidAccountNumber(accountID)) return DBR_DOESNTEXIST;

  // Build the SQL query
  char query[1024];
  sqlite3_snprintf(sizeof(query), query,
    "UPDATE characters SET world_x=%.02f, world_y=%.02f, hp_current=%i, mp_current=%i, "\
    "hp_max=%i, mp_max=%i, level=%i, exp_current=%i, ability_pts=%i, "\
    "str=%i, agi=%i, con=%i, int=%i, wis=%i, money=%i, map_index=%i, geonite=%i WHERE account_id=%lu AND character_id=%lu",
    x, y, character->hp, character->mp, character->base.max_hp, character->base.max_mp,
    character->base.level, character->currentEXP, character->availableStatPoints,
    character->stats[0], character->stats[1], character->stats[2], 
    character->stats[3], character->stats[4], character->money, map_index,
    character->geonite,
    accountID, characterID);

  // Begin a transaction
  char* error_message;
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery("BEGIN", &error_message))("Couldn't commit (BEGIN):  %s", error_message);

  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't commit character (%s):  %s", query, error_message);

  sqlite3_snprintf(sizeof(query), query,
    "DELETE FROM items WHERE owner_character_id=%lu",
    characterID);
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't commit character:  %s", error_message);

  { // write the inventory into the database
    GlobalItemManager* item_manager = GlobalItemManager::singleton();
    for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i) {
      DatabaseInventoryItem database_item;
      if (false == item_manager->translateAvatarItemToDatabaseItem(
            &inventory[i],
            &database_item)) {
        continue;
      }

      sqlite3_snprintf(sizeof(query), query,
        "REPLACE INTO items (item_id, type_hash, quantity, equipped, owner_character_id) "\
        "VALUES (%lld, %lu, %lu, %i, %lu)",
        database_item.id,
        database_item.permanent_storage_hash,
        database_item.quantity,
        database_item.equipped ? (i-AVATAR_INVENTORY_SIZE) : i,
        characterID);
      CONFIRM(SQLITE_OK == sqlite3_executeQuery(query, &error_message)) else {
        DEBUG_INFO("%s: %s", query, error_message);
      }
    }
  }

  //for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i)
  //{
  //  if (inventory[i].invalid) continue;
  //  Item* item = GlobalItemManager::singleton()->getItem(inventory[i].itemHashIndex,inventory[i].itemHashValue);
  //  CONFIRM(item) else continue;

  //  sqlite3_snprintf(sizeof(query), query,
  //    "UPDATE items SET location=%lu WHERE item_id=%lld AND owner_character_id=%lu",
  //    (int)(inventory[i].equipped ? ITEMLOCATION_EQUIPPED : ITEMLOCATION_INVENTORY),
  //    item->getUniqueID(),
  //    characterID);
  //  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't commit item:  %s", error_message);
  //}

  // Done comitting
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery("COMMIT", &error_message))("Couldn't finish commit:  %s", error_message);

  // Success
  return DBR_SUCCESS;
}

GlobalDatabaseManager::DBResult
GlobalDatabaseManager::renameCharacter(const char* old_name, const char* new_name) {
  char query[1024];
  sqlite3_snprintf(sizeof(query), query,
    "UPDATE characters SET name='%s' WHERE name='%s'", new_name, old_name);
  char* error_message;
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't change char name:  %s", error_message);
  return DBR_SUCCESS;
}



GlobalDatabaseManager::DBResult
GlobalDatabaseManager::findAccountFromName(const char* name, unsigned int* number) {
  sqlite3_stmt* stmt;
  char query[2048];
  sqlite3_snprintf(sizeof(query), query,
    "SELECT account_id FROM characters WHERE name='%s'", name);
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  DBResult dbr = DBR_SUCCESS;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    *number = sqlite3_column_int(stmt, 0);
  } else {
    dbr = DBR_DOESNTEXIST;
  }
  sqlite3_finalize(stmt);
  return dbr;
}

GlobalDatabaseManager::DBResult
GlobalDatabaseManager::storageGet(unsigned int account_id,
                                  ServerStorageItem* storage_items,
                                  int* number_of_items) {
  sqlite3_stmt* stmt;
  char query[2048];

  // Query the item database for this character's inventory
  sqlite3_snprintf(sizeof(query), query,
    "SELECT item_id, type_hash, quantity "\
    "FROM storage "\
    "WHERE account_id=%lu", account_id);

  // Read all of the items out of the database
  GlobalItemManager* item_manager = GlobalItemManager::singleton();
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  int storage_index = 0;
  while(sqlite3_step(stmt) == SQLITE_ROW && storage_index < MAX_ITEMS_IN_STORAGE) {
    unsigned int item_type;
    if (false == item_manager->translatePermanentStorageHashToItemType(sqlite3_column_int(stmt,1),
                                                                      &item_type)) {
      continue; // couldn't translate this item...
    }
    storage_items[storage_index].id = (ItemIDType)sqlite3_column_int64(stmt, 0); // unique id #
    storage_items[storage_index].type = item_type;
    storage_items[storage_index].quantity = sqlite3_column_int(stmt, 2);
    ++storage_index;
  }

  *number_of_items = storage_index;

  sqlite3_finalize(stmt);

  return DBR_SUCCESS;
}



GlobalDatabaseManager::DBResult
GlobalDatabaseManager::storageWithdraw(unsigned int account_id,
                                       ItemIDType item_id,
                                       AvatarInventoryItem* inventory_item) {
  // Query the storage database for this item
  char query[1024];
  sqlite3_snprintf(sizeof(query), query,
    "SELECT type_hash, quantity "\
    "FROM storage "\
    "WHERE account_id=%lu AND item_id=%lld", account_id, item_id);

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  DBResult dbr = DBR_DOESNTEXIST;
  DatabaseInventoryItem database_item;
  if (SQLITE_ROW == sqlite3_step(stmt)) {
    database_item.id = item_id;
    database_item.equipped = false;
    database_item.permanent_storage_hash = sqlite3_column_int(stmt,0);
    database_item.quantity = sqlite3_column_int(stmt, 1);
    if (GlobalItemManager::singleton()->acquireAvatarItemForDatabaseItem(&database_item, inventory_item)) {
      dbr = DBR_SUCCESS;
    }
  }
  sqlite3_finalize(stmt);

  // if we obtained the item, remove it from storage so that it can't possibly be duplicated
  if (dbr == DBR_SUCCESS) {
    sqlite3_snprintf(sizeof(query), query,
      "DELETE FROM storage WHERE item_id=%lld", item_id);
    char* error_message;
    CONFIRM(SQLITE_OK == sqlite3_executeQuery(query, &error_message)) else {
      GlobalItemManager::singleton()->releaseAvatarInventoryItem(inventory_item);
      return DBR_INTERNALERROR;
    }
  }

  return dbr;
}



GlobalDatabaseManager::DBResult
GlobalDatabaseManager::storageDeposit(unsigned int account_id,
                                      AvatarInventoryItem* inventory_item) {
  Item* item = inventory_item->item;
  CONFIRM(item) else return DBR_INTERNALERROR;

  DatabaseInventoryItem database_item;
  GlobalItemManager* global_item_manager = GlobalItemManager::singleton();
  global_item_manager->translateAvatarItemToDatabaseItem(inventory_item, &database_item);

  char query[1024];

  { // check to make sure the user doesn't already have a lot of stored items
    sqlite3_snprintf(sizeof(query), query, "SELECT COUNT(*) FROM storage WHERE account_id=%lu", account_id);
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
    int items_in_storage = 0;
    if (SQLITE_ROW == sqlite3_step(stmt)) {
      items_in_storage = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if (items_in_storage >= MAX_ITEMS_IN_STORAGE) return DBR_OVERLIMIT;
  }



  // Query the storage database for this item
  sqlite3_snprintf(sizeof(query), query,
    "INSERT INTO storage "\
    "(item_id, account_id, type_hash, quantity) "\
    "VALUES (%lld, %lu, %lu, %lu)",
    database_item.id,
    account_id,
    database_item.permanent_storage_hash,
    database_item.quantity);

  DBResult dbr = DBR_INTERNALERROR;
  char* error_message;
  if (SQLITE_OK == sqlite3_executeQuery(query, &error_message)) {
    global_item_manager->releaseAvatarInventoryItem(inventory_item);
    dbr = DBR_SUCCESS;
  }

  // If we obtained the item, erase it from the database storage of the
  // character's inventory to prevent duplication
  if (dbr == DBR_SUCCESS) {
    sqlite3_snprintf(sizeof(query), query,
      "DELETE FROM items WHERE item_id=%lld", database_item.id);
    char* error_message;
    sqlite3_executeQuery(query, &error_message);
  }

  return dbr;
}







GlobalDatabaseManager::DBResult GlobalDatabaseManager::commitCharacterMoney(unsigned int accountID, unsigned int characterID, unsigned int money) {
  char query[1024];
  sqlite3_snprintf(sizeof(query), query,
    "UPDATE characters SET money=%i WHERE account_id=%lu AND character_id=%lu",
    money, accountID, characterID);
  char* error_message;
  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't commit character:  %s", error_message);
  return DBR_SUCCESS;
}


void GlobalDatabaseManager::executeDiskDBThread()
{
  sqlite3* diskDB;

  // Open up the database for writing to the hard drive
  CONFIRM(SQLITE_OK == sqlite3_open("evidyon.sqldb", &diskDB));

  // Repeat until the memory database is closed
  while(myDB)
  {
    WaitForSingleObject(myDiskDBThreadWaitEvent, INFINITE);

    const char* query;
    while(myDiskDBQueriesToExecute.readByAssignment(&query))
    {
      char* error;
      if (SQLITE_OK != sqlite3_exec(diskDB, query, 0, 0, &error))
        DEBUG_WARNING("disk query failed:  %s - %s", query, error);

      // Deallocate this query
      SAFE_DELETE_ARRAY(query);
    }
  }

  // Close our database
  sqlite3_close(diskDB);

  // Remove all of the entries
  const char* query;
  while(myDiskDBQueriesToExecute.readByAssignment(&query))
    SAFE_DELETE_ARRAY(query);

  sqlite3_thread_cleanup();
}



int GlobalDatabaseManager::sqlite3_executeDiskQuery(const char* query)
{
  CONFIRM(myDiskDBThreadHandle && myDiskDBQueriesToExecute.isValid()) else return -1;

  size_t len = strlen(query) + 1;
  char* sql = new char[len];
  memcpy(sql, query, sizeof(char)*len);

  int attempts = 0;
  while(APP_ERROR(!myDiskDBQueriesToExecute.writeByAssignment(sql))("Failed to add query"))
  {
    if (APP_ERROR(attempts > 3)("Completely failed to write %s", sql))
    {
      SAFE_DELETE(sql);
      return -1;
    }
    ++attempts;
    Sleep(100);
  }
  SetEvent(myDiskDBThreadWaitEvent);
  return 0;
}


int GlobalDatabaseManager::sqlite3_executeQuery(const char* query, char** error_message)
{
  int rc = sqlite3_exec(myDB, query, 0, 0, error_message);

  if (rc == SQLITE_OK || rc == SQLITE_DONE)
  {
    // Write to disk asynchronously if possible
    if (myDiskDBThreadHandle)
      sqlite3_executeDiskQuery(query);

    // Asynchronous is not activated; therefore, we are in startup mode.  Write
    // synchronously to the disk.
    else if (myDiskDB)
    {
      rc = sqlite3_exec(myDiskDB, query, 0, 0, error_message);
      CONFIRM(SQLITE_OK == rc || SQLITE_DONE == rc);
    }
  }

  return rc == SQLITE_DONE ? SQLITE_OK : rc; // merge done -> OK
}



//// Creates a new listing in the bazaar.  A price of 0 indicates that
//// this item cannot be bought for currency.  Unless DBR_SUCCESS is returned,
//// inventory_item is not affected.  If the item was listed in the bazaar,
//// the item is erased from the inventory structure.
//GlobalDatabaseManager::DBResult
//GlobalDatabaseManager::bazaarCreateListing(AvatarInventoryItem* inventory_item,
//                                           unsigned int price,                // todo: unsigned int -> money_t
//                                           unsigned int selling_account_id);  // todo: unsigned int -> accountid_t
//
//// Purchases the given listing if its price is less than *money and nonzero.
//// This will move the item to the purchaser's storage and change *money
//// to reflect the cost of the item.  The listing is move from the open-
//// listings table and an entry is created in purchased listings.
//// The selling account ID is listed so that, if it is logged in, the account
//// can be notified of the change.
//GlobalDatabaseManager::DBResult
//GlobalDatabaseManager::bazaarPurchaseListing(Bazaar::ListingID listing_id,
//                                             unsigned int* money,
//                                             unsigned int purchaser_account_id,
//                                             unsigned int* selling_account_id);
//
//// Takes a listing out of the bazaar.  If successful, the item will appear
//// in the storage for listing account.  The 'selling_account_id' is used
//// to confirm the authenticity of the origin of this transaction (use the
//// acting client's account ID).
//// If this method fails, it's likely the item was purchased.  The client's
//// seller listings should be updated.
//GlobalDatabaseManager::DBResult
//GlobalDatabaseManager::bazaarWithdrawListing(Bazaar::ListingID listing_id,
//                                             unsigned int selling_account_id);
//
//// Takes a listing that was purchased out of the bazaar.  If successful,
//// *income will be set to the price for which the item was sold and the
//// listing will be removed from the database.  If this method fails, be
//// sure the client's seller listing are synchronized since
//// this method only works once.
//GlobalDatabaseManager::DBResult
//GlobalDatabaseManager::bazaarRedeemListing(Bazaar::ListingID listing_id,
//                                           unsigned int selling_account_id,
//                                           unsigned int* income);
//
//
//// Gets a list of the items put up for sale by an account.  This
//// method returns both open and purchased listings, setting values
//// in the returned array of BazaarListing structures to denote
//// which group each item belongs to.
//GlobalDatabaseManager::DBResult
//GlobalDatabaseManager::bazaarGetSellerListings(Bazaar::Server::SellerListing* listings,
//                                               size_t buffer_size,
//                                               unsigned int selling_account_id,
//                                               size_t* listings_returned);
//
//// Opens up an unprocessed offer for the given listing that consists of
//// items from the avatar's inventory.  If the listing doesn't exist or
//// the offering account already has an unprocessed or rejected offer
//// for the given listing, this method will fail.  If bazaarCreateOffer
//// succeeds, the inventory indices will be erased.
//// todo: should we erase items with matching IDs from 'items'?
//GlobalDatabaseManager::DBResult
//GlobalDatabaseManager::bazaarCreateOffer(Bazaar::ListingID listing_id,        // desired item listing
//                                         unsigned int offering_character_id,  // account trying to make the purchase
//                                         AvatarInventoryItem* inventory,
//                                         unsigned int* inventory_indices,
//                                         size_t number_of_inventory_indices);
//
//// The seller has rejected a specific offer for his or her listing.  This
//// will move the offer from the unprocessed table to the rejected one.
//// Be sure to notify the offering account of the change of it is logged in!
//GlobalDatabaseManager::DBResult
//GlobalDatabaseManager::bazaarRejectOffer(Bazaar::ListingID listing_id,
//                                         unsigned int offering_account_id);
//
//// Called to automatically reject all offers in the 'unprocessed' category
//// for a given listing.  This method can be invoked at a client's request,
//// but is also used to clean up dangling offers when an item has been
//// traded or purchased.
//GlobalDatabaseManager::DBResult
//GlobalDatabaseManager::bazaarRejectAllOffers(Bazaar::ListingID listing_id);
//
//// Returns items bid on a given listing to the storage of the account
//// that offered them from *either* the unprocssed or the rejected
//// table.  If this method fails, synchronize the client's offers
//// since it's possible the offer was accepted.
//GlobalDatabaseManager::DBResult
//GlobalDatabaseManager::bazaarWithdrawOffer(Bazaar::ListingID listing_id,
//                                           unsigned int offering_character_id);
//
//// Gets the offers that this buyer has made on listings.
//GlobalDatabaseManager::DBResult
//GlobalDatabaseManager::bazaarGetBuyerOffers(Bazaar::Server::ListingForBuyerOffer* listings_desired,
//                                            size_t listings_buffer_size,
//                                            Bazaar::Server::BuyerOfferElement* offerings,
//                                            size_t offerings_buffer_size,
//                                            unsigned int offering_character_id,
//                                            size_t* listings_returned);
//
//// Moves an item into the offering account's storage, if it was accepted.
//DBResult bazaarClaimAcceptedOffer(Bazaar::ListingID listing_id,
//                                  unsigned int offering_character_id);








/*
bool GlobalDatabaseManager::fillBazaar(Economy::Bazaar* bazaar) {
  char query[1024];
  sqlite3_snprintf(
  sizeof(query),
  query,
  "SELECT item_id, seller_character_id, price FROM bazaar_listings WHERE 1");
  sqlite3_stmt* bazaar_stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &bazaar_stmt, 0);
  bool error = false;
  while (sqlite3_step(bazaar_stmt) == SQLITE_ROW) {
  error = true;
  Economy::BazaarItemListing listing;
  listing.item_id = sqlite3_column_int64(bazaar_stmt, 0);
  listing.listing_character = sqlite3_column_int(bazaar_stmt, 1);
  listing.trade_listing.price = (unsigned)sqlite3_column_int(bazaar_stmt, 2);
  sqlite3_snprintf(
    sizeof(query),
    query,
    "SELECT type, quantity, owner_character_id FROM items WHERE item_id=%lld AND location=%lu",
    listing.item_id,
    (unsigned int)(ITEMLOCATION_BAZAAR));
  sqlite3_stmt* item_stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &item_stmt, 0);
  CONFIRM(sqlite3_step(item_stmt) == SQLITE_ROW) else {
    DEBUG_ERROR("Invalid item listed in bazaar");
    break;
  }
  listing.trade_listing.type = sqlite3_column_int(item_stmt, 0);
  listing.trade_listing.quantity = sqlite3_column_int(item_stmt, 1);
  listing.trade_listing.purchased = 0;
  sqlite3_finalize(item_stmt);

  CONFIRM(bazaar->addItemsForSaleFromDB(&listing, 1)) else break; // TODO: add more than one at a time

  error = false;
  }
  sqlite3_finalize(bazaar_stmt);
  sqlite3_snprintf(
  sizeof(query),
  query,
  "SELECT seller_character_id, price, item_type, item_quantity FROM bazaar_sales WHERE 1");
  sqlite3_prepare_v2(myDB, query, sizeof(query), &bazaar_stmt, 0);
  while (sqlite3_step(bazaar_stmt) == SQLITE_ROW) {
  error = true;
  Economy::BazaarItemListing listing;
  listing.item_id = 0;
  listing.listing_character = sqlite3_column_int(bazaar_stmt, 0);
  listing.trade_listing.price = sqlite3_column_int(bazaar_stmt, 1);
  listing.trade_listing.type = sqlite3_column_int(bazaar_stmt, 2);
  listing.trade_listing.quantity = sqlite3_column_int(bazaar_stmt, 3);
  listing.trade_listing.purchased = 1;

  CONFIRM(bazaar->addItemsForSaleFromDB(&listing, 1)) else break; // TODO: add more than one at a time
  error = false;
  }
  sqlite3_finalize(bazaar_stmt);
  return !error;
}



int GlobalDatabaseManager::getNumberOfListedItems(unsigned int character_id) {
  char query[1024];
  sqlite3_snprintf(sizeof(query), query,
  "SELECT COUNT(*) FROM bazaar_listings WHERE seller_character_id=%lu",
  character_id);
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
  int retval = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
  retval = sqlite3_column_int(stmt, 0);
  }
  sqlite3_finalize(stmt);
  return retval;
}*/

//
//GlobalDatabaseManager::DBResult GlobalDatabaseManager::itemGenerated(
//  size_t type,
//  int quantity,
//  ItemIDType* unique_id) {
//  ItemIDType generated_unique_id;
//  ((unsigned long*)&generated_unique_id)[0] = rand_uint32();
//  ((unsigned long*)&generated_unique_id)[1] = rand_uint32();
//  char query[1024];
//  sqlite3_snprintf(sizeof(query), query,
//    "INSERT INTO items (item_id, location, type, quantity, owner_character_id) VALUES (%lld, %lu, %lu, %lu, %lu)",
//    generated_unique_id,
//    (int)(ITEMLOCATION_NONE),
//    type,
//    quantity,
//    0);
//  char* error_message;
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't commit item:  %s", error_message);
//  *unique_id = generated_unique_id;
//  return DBR_SUCCESS;
//}
//
//GlobalDatabaseManager::DBResult GlobalDatabaseManager::itemOnGroundErased(ItemIDType unique_id) {
//  char query[1024];
//  sqlite3_snprintf(sizeof(query), query,
//    "DELETE FROM items WHERE owner_character_id=0 AND location=%lu AND item_id=%lld",
//    (int)(ITEMLOCATION_WORLD), unique_id);
//  char* error_message;
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't erase item:  %s", error_message);
//  return DBR_SUCCESS;
//}
//
//GlobalDatabaseManager::DBResult GlobalDatabaseManager::itemPickedUp(unsigned int owner_character_id, ItemIDType unique_id) {
//  char query[1024];
//  sqlite3_snprintf(sizeof(query), query,
//    "UPDATE items SET owner_character_id=%lu, location=%lu WHERE location=%lu AND item_id=%lld",
//    owner_character_id, (int)(ITEMLOCATION_INVENTORY), (int)(ITEMLOCATION_WORLD), unique_id);
//  char* error_message;
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't pick up item:  %s", error_message);
//  return DBR_SUCCESS;
//}
//
//GlobalDatabaseManager::DBResult GlobalDatabaseManager::itemDroppedByOwner(ItemIDType unique_id) {
//  char query[1024];
//  sqlite3_snprintf(sizeof(query), query,
//    "UPDATE items SET owner_character_id=0, location=%lu WHERE item_id=%lld",
//    (int)(ITEMLOCATION_WORLD), unique_id);
//  char* error_message;
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't pick up item:  %s", error_message);
//  return DBR_SUCCESS;
//}
//
//
//GlobalDatabaseManager::DBResult GlobalDatabaseManager::itemListedInBazaar(
//  ItemIDType unique_id,
//  unsigned int seller_character_id,
//  unsigned int price) {
//  char query[1024];
//  sqlite3_snprintf(sizeof(query), query,
//    "UPDATE items SET location=%lu WHERE owner_character_id=%lu AND item_id=%lld",
//    (int)(ITEMLOCATION_BAZAAR), seller_character_id, unique_id);
//  char* error_message;
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't list bazaar item:  %s", error_message);
//
//  sqlite3_snprintf(sizeof(query), query,
//    "INSERT INTO bazaar_listings (item_id, seller_character_id, price) VALUES (%lld,%lu,%lu)",
//    unique_id, seller_character_id, price);
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't list bazaar item:  %s", error_message);
//  return DBR_SUCCESS;
//}
//
//
//GlobalDatabaseManager::DBResult GlobalDatabaseManager::itemPurchasedFromBazaar(
//  ItemIDType unique_id,
//  unsigned int new_owner_character_id) {
//  char query[1024];
//  char* error_message;
//  sqlite3_snprintf(
//    sizeof(query),
//    query,
//    "SELECT price, seller_character_id FROM bazaar_listings WHERE item_id=%lld",
//    unique_id);
//  sqlite3_stmt* stmt;
//  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
//  if (sqlite3_step(stmt) != SQLITE_ROW) return DBR_INTERNALERROR;
//  unsigned int item_price = sqlite3_column_int(stmt, 0);
//  unsigned int seller_character_id = sqlite3_column_int(stmt, 1);
//  sqlite3_finalize(stmt);
//  sqlite3_snprintf(sizeof(query), query, "SELECT type, quantity FROM items WHERE item_id=%lld", unique_id);
//  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
//  if (sqlite3_step(stmt) != SQLITE_ROW) return DBR_INTERNALERROR;
//  unsigned int item_type = sqlite3_column_int(stmt, 0);
//  unsigned int item_quantity = sqlite3_column_int(stmt, 1);
//  sqlite3_finalize(stmt);
//  sqlite3_snprintf(sizeof(query), query, "DELETE FROM bazaar_listings WHERE item_id=%lld", unique_id);
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't purchase bazaar item:  %s", error_message);
//  sqlite3_snprintf(
//    sizeof(query),
//    query,
//    "INSERT INTO bazaar_sales (seller_character_id, price, item_type, item_quantity) VALUES (%lu, %lu, %lu, %lu)",
//    seller_character_id,
//    item_price,
//    item_type,
//    item_quantity);
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't purchase bazaar item:  %s", error_message);
//  sqlite3_snprintf(sizeof(query), query,
//    "UPDATE items SET location=%lu, owner_character_id=%lu WHERE location=%lu AND item_id=%lld",
//    (int)(ITEMLOCATION_INVENTORY), new_owner_character_id, (int)(ITEMLOCATION_BAZAAR), unique_id);
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery(query, &error_message))("Couldn't purchase bazaar item:  %s", error_message);
//  return DBR_SUCCESS;
//}
//
//// if 'profit' returns a nonzero value, the item was purchased and the seller no longer has the item
//GlobalDatabaseManager::DBResult GlobalDatabaseManager::itemWithdrawnFromBazaar(
//  ItemIDType unique_id,
//  unsigned int seller_character_id) {
//  DBResult dbr = DBR_SUCCESS;
//  char* error_message;
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery("BEGIN", &error_message))("Couldn't start commit:  %s", error_message);
//  char query[2048];
//  { // get the current owner and make sure the seller is still the owner
//  sqlite3_snprintf(
//    sizeof(query),
//    query,
//    "SELECT owner_character_id FROM items WHERE item_id=%lld",
//    unique_id);
//  sqlite3_stmt* stmt;
//  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, 0);
//  if (sqlite3_step(stmt) != SQLITE_ROW) return DBR_INTERNALERROR;
//  unsigned int database_seller_character_id = sqlite3_column_int(stmt,0);
//  sqlite3_finalize(stmt);
//  if (APP_ERROR(database_seller_character_id != seller_character_id)(
//    "Character %lu trying to withdraw item sold by %lu", seller_character_id, database_seller_character_id)) {
//      dbr = DBR_INTERNALERROR; goto EXIT;
//  }
//  }
//  if (seller_character_id != 0) { // move the item to the seller's inventory
//  sqlite3_snprintf(sizeof(query), query,
//    "UPDATE items SET location=%lu WHERE location=%lu AND owner_character_id=%lu AND item_id=%lld",
//    (int)(ITEMLOCATION_INVENTORY), (int)(ITEMLOCATION_BAZAAR), seller_character_id, unique_id);
//  CONFIRM(SQLITE_OK == sqlite3_executeQuery(query, &error_message)) else { dbr = DBR_INTERNALERROR; goto EXIT; }
//  } else { // bot sale, erase the item
//  sqlite3_snprintf(sizeof(query), query,
//    "DELETE FROM items WHERE location=%lu AND owner_character_id=%lu AND item_id=%lld",
//    (int)(ITEMLOCATION_BAZAAR), seller_character_id, unique_id);
//  CONFIRM(SQLITE_OK == sqlite3_executeQuery(query, &error_message)) else { dbr = DBR_INTERNALERROR; goto EXIT; }
//  }
//  { // erase the item from the  bazaar
//  sqlite3_snprintf(
//    sizeof(query), query,
//    "DELETE FROM bazaar_listings WHERE item_id=%lld AND seller_character_id=%lu",
//    unique_id,
//    seller_character_id);
//  char* error_message;
//  CONFIRM(SQLITE_OK == sqlite3_executeQuery(query, &error_message)) else { dbr = DBR_INTERNALERROR; goto EXIT; }
//  }
//  
//  // Done comitting
//EXIT:
//  APP_ERROR(SQLITE_OK != sqlite3_executeQuery("COMMIT", &error_message))("Couldn't finish commit:  %s", error_message);
//  return dbr;
//}
//
//
//GlobalDatabaseManager::DBResult GlobalDatabaseManager::itemProfitsWithdrawn(
//  unsigned int seller_character_id,
//  unsigned int* profit) {
//  char query[2048];
//  sqlite3_snprintf(
//    sizeof(query),
//    query,
//    "SELECT price FROM bazaar_sales WHERE seller_character_id=%lu",
//    seller_character_id);
//  sqlite3_stmt* stmt;
//  sqlite3_prepare_v2(myDB, query, sizeof(query), &stmt, NULL);
//  unsigned int total_profit = 0;
//  while (SQLITE_ROW == sqlite3_step(stmt)) {
//  total_profit += sqlite3_column_int(stmt,0);
//  }
//  sqlite3_finalize(stmt);
//  sqlite3_snprintf(
//    sizeof(query),
//    query,
//    "DELETE FROM bazaar_sales WHERE seller_character_id=%lu",
//    seller_character_id);
//  char* error_message;
//  CONFIRM(SQLITE_OK == sqlite3_executeQuery(query, &error_message)) else return DBR_INTERNALERROR;
//  *profit = total_profit;
//  return DBR_SUCCESS;
//}


UniqueAvatarNameChecker* GlobalDatabaseManager::getUniqueAvatarNameChecker() {
  return &unique_avatar_name_checker_;
}


}