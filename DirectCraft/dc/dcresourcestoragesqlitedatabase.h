#ifndef __DCRESOURCESTORAGESQLITEDATABASE_H__
#define __DCRESOURCESTORAGESQLITEDATABASE_H__

#include "../sqlite/sqlite3.h"
#include "dcresourcestorage.h"

namespace dc {
  
class dcResourceStorageSQLiteDatabase : public dcResourceStorage {
public:

  // Initializes the class
  dcResourceStorageSQLiteDatabase();

  // Closes the database, just in case the user forgot
  ~dcResourceStorageSQLiteDatabase();

  // Access the database in the given filename
  bool open(const std::string& file_name,
            const std::string& table);

  // Closes the database
  void close();


public:

  // Returns all records that are children of the given path
  virtual bool read(const std::string& owner_path,
                    RecordList* members);

private:

  // Creates or saves over the given record.  A unique record is
  // defined as a (parent_path, member_name) pair.
  virtual bool write(const std::string& owner_path,
                     const Record* record);

  // Starts a transaction then deletes the resource at the given path
  // and all of its descendents.
  virtual bool begin(const std::string& owner_path,
                     const std::string& source_name);

  // Finishes a storage transaction and commits the changes if
  // the flag is set.  If the commit is 'false', all writes since
  // the last 'begin' call are undone.
  virtual void end(bool commit);


private:
  bool queryOpen(const std::string& query, sqlite3_stmt** stmt_out);
  void queryGetError(std::string* error);
  void queryClose(sqlite3_stmt** stmt);

private:
  std::string table_;
  sqlite3* database_;
};

}


#endif