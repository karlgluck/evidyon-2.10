#ifndef __DCRESOURCESTORAGE_H__
#define __DCRESOURCESTORAGE_H__


#include "dcresource.h"


namespace dc {


//----[  dcResourceStorage  ]------------------------------------------------
// This class is used to read and write a hierarchial permanent storage of
// resource objects.  It's intended to be used with a database file, but
// the implementation is left virtualized.
class dcResourceStorage {

protected:

  // This element is stored in the resource
  struct Record {
    std::string resource_type;
    std::string member_name;
    std::string value;
  };

  typedef std::list<Record> RecordList;

public:
  dcResourceStorage();
  virtual ~dcResourceStorage();

  // Opens up the given file for editing.  Segments with different names won't
  // be able to interfere with each other.
  static dcResourceStorage* open(const std::string& file_name,
                                 const std::string& segment);

  // Closes the storage and cleans up memory
  static void close(dcResourceStorage* storage);

  // Reads the resource from the given path into the provided parameter. The
  // storage structure supports any number of root resources as long as
  // none of them conflict.
  bool load(const std::string& owner_path, dcGenericResource* resource);

  // Updates a given resource and all of its children in the file, leaving
  // other resources intact.
  bool save(const std::string& owner_path, dcGenericResource* resource);

  // Erases all resource records from the storage
  void clear();

public:

  // Returns all records of resource that are members of the given owner
  virtual bool read(const std::string& owner_path,
                    RecordList* members) = 0;

private:

  // Saves over the record and all of its descendents
  virtual bool write(const std::string& owner_path,
                     const Record* record) = 0;

  // Starts a transaction then deletes the resource at the given path
  // and all of its descendents.
  virtual bool begin(const std::string& owner_path,
                     const std::string& source_name) = 0;

  // Finishes a storage transaction and commits the changes if
  // the flag is set.  If the commit is 'false', all writes since
  // the last 'begin' call are undone.
  virtual void end(bool commit) = 0;


private:
  dcResourceStorage(dcResourceStorage&);
  void operator =(dcResourceStorage&);
};


}




#endif