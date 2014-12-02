#include "../dc/dcresourcestoragesqlitedatabase.h"
#include "../dc/dcresource.h"
#include "../dc/dcdebug.h"

namespace dc {
  

//----[  dcResourceStorageSQLiteDatabase  ]----------------------------------
dcResourceStorageSQLiteDatabase::dcResourceStorageSQLiteDatabase() {
  database_ = NULL;
}


//----[  dcResourceStorageSQLiteDatabase  ]----------------------------------
dcResourceStorageSQLiteDatabase::~dcResourceStorageSQLiteDatabase() {
  close();
}
  

//----[  open  ]-------------------------------------------------------------
bool dcResourceStorageSQLiteDatabase::open(const std::string& file_name,
                                           const std::string& table) {
  sqlite3_open(file_name.c_str(), &database_);
  if (!database_) return false;
  table_ = table;

  char sql[2048];
  sqlite3_stmt* stmt = NULL;

  // Does the table already exist?
  sqlite3_snprintf(2048,
                   sql,
                   "SELECT COUNT(name) "\
                   "FROM sqlite_master "\
                   "WHERE type='table' AND name='%q'",
                   table.c_str());
  if (!queryOpen(sql, &stmt) ||
      SQLITE_ROW != sqlite3_step(stmt)) {
    queryClose(&stmt);
    close();
    return true;
  }
  bool already_exists = 1 == sqlite3_column_int(stmt, 0);
  queryClose(&stmt);
  if (already_exists) return true;


  // create the data structure
  sqlite3_snprintf(2048,
                   sql,
                   "CREATE TABLE %q ("\
                   "owner_path VARCHAR,"\
                   "resource_type VARCHAR,"\
                   "member_name VARCHAR,"\
                   "value VARCHAR"\
                   ")",
                   table.c_str());
  if (!queryOpen(sql, &stmt) ||
      SQLITE_DONE != sqlite3_step(stmt)) {
    close();
    return false;
  }
  queryClose(&stmt);

  return true;
}




//----[  close  ]--------------------------------------------------------------
void dcResourceStorageSQLiteDatabase::close() {
  if (database_) {
    sqlite3_close(database_);
    database_ = NULL;
  }
}




//----[  read  ]---------------------------------------------------------------
bool dcResourceStorageSQLiteDatabase::read(const std::string& owner_path,
                                           RecordList* members) {
  char sql[2048];
  sqlite3_snprintf(2048,
                   sql,
                   "SELECT resource_type, member_name, value "\
                   "FROM '%q' "\
                   "WHERE owner_path='%q'",
                   table_.c_str(),
                   owner_path.c_str());
  sqlite3_stmt* stmt = NULL;
  if (!queryOpen(sql, &stmt)) return false;
  while (SQLITE_ROW == sqlite3_step(stmt)) {
    Record record;
    record.resource_type.assign((const char*)sqlite3_column_text(stmt, 0));
    record.member_name.assign((const char*)sqlite3_column_text(stmt, 1));
    record.value.assign((const char*)sqlite3_column_text(stmt, 2));
    members->push_back(record);
  }
  queryClose(&stmt);
  return true;
}




//----[  write  ]--------------------------------------------------------------
bool dcResourceStorageSQLiteDatabase::write(const std::string& owner_path,
                                            const Record* record) {
  char* sql = sqlite3_mprintf("REPLACE INTO '%q' "\
                              "(owner_path, resource_type, member_name, value) "\
                              "VALUES "\
                              "(%Q, %Q, %Q, %Q)",
                              table_.c_str(),
                              owner_path.c_str(),
                              record->resource_type.c_str(),
                              record->member_name.c_str(),
                              record->value.c_str());
  sqlite3_stmt* stmt = NULL;
  bool return_value = SQLITE_OK == sqlite3_exec(database_, sql, NULL, 0, NULL);
  sqlite3_free(sql);
  return return_value;
}




//----[  begin  ]--------------------------------------------------------------
bool dcResourceStorageSQLiteDatabase::begin(const std::string& owner_path,
                                            const std::string& source_name) {
  sqlite3_stmt* stmt = NULL;
  if (!queryOpen("BEGIN", &stmt)) return false;
  sqlite3_step(stmt);
  queryClose(&stmt);

  // Erase all resources whose parent string contains the name of this resource.
  char sql[2048];
  std::string path = owner_path +
                     dc::dcGenericResource::PATH_SEPARATOR +
                     source_name;
  sqlite3_snprintf(2048,
                   sql,
                   "DELETE FROM %s "\
                   "WHERE (owner_path LIKE '%s%%') OR (member_name=%Q AND owner_path=%Q)",
                   table_.c_str(),
                   path.c_str(),
                   source_name.c_str(),
                   owner_path.c_str());
  if (!queryOpen(sql, &stmt)) {
    end(false);
    return false;
  }
  queryClose(&stmt);
  return true;
}





//----[  end  ]----------------------------------------------------------------
void dcResourceStorageSQLiteDatabase::end(bool commit) {
  sqlite3_stmt* stmt = NULL;
  if (queryOpen(commit ? "COMMIT" : "ROLLBACK", &stmt)) {
    sqlite3_step(stmt);
    queryClose(&stmt);
  }
}






//----[  queryOpen  ]----------------------------------------------------------
bool dcResourceStorageSQLiteDatabase::queryOpen(const std::string& query,
                                                sqlite3_stmt** stmt_out) {
  sqlite3_stmt* stmt = NULL;
  if (SQLITE_OK == sqlite3_prepare_v2(database_,
                                      query.c_str(),
                                      query.length() + 1,
                                      &stmt, NULL)) {
    *stmt_out = stmt;
    return true;
  } else {
    DEBUG_INFO("queryOpen:  %s", sqlite3_errmsg(database_));
    return false;
  }
}








//----[  queryGetError  ]------------------------------------------------------
void dcResourceStorageSQLiteDatabase::queryGetError(std::string* error) {
  error->assign(sqlite3_errmsg(database_));
}









//----[  queryClose  ]---------------------------------------------------------
void dcResourceStorageSQLiteDatabase::queryClose(sqlite3_stmt** stmt) {
  if (!stmt) return;
  sqlite3_stmt* internal_stmt = *stmt;
  if (!internal_stmt) return;
  sqlite3_finalize(internal_stmt);
  *stmt = NULL;
}


}