#include "../dc/dcresourcestorage.h"
#include "../dc/dcresourcestoragesqlitedatabase.h"
#include <queue>
#include <algorithm>

namespace dc {


  
//----[  dcResourceStorage  ]------------------------------------------------
dcResourceStorage::dcResourceStorage() {
}
  
//----[  dcResourceStorage  ]------------------------------------------------
dcResourceStorage::~dcResourceStorage() {
}




//----[  open  ]---------------------------------------------------------------
dcResourceStorage* dcResourceStorage::open(const std::string& file_name,
                                           const std::string& segment) {
  dcResourceStorageSQLiteDatabase* db = new dcResourceStorageSQLiteDatabase;
  if (db->open(file_name, segment)) return db;
  delete db;
  return NULL;
}


//----[  close  ]--------------------------------------------------------------
void dcResourceStorage::close(dcResourceStorage* storage) {
  ((dcResourceStorageSQLiteDatabase*)storage)->close();
  delete storage;
}

  
//----[  load  ]-------------------------------------------------------------
bool dcResourceStorage::load(const std::string& owner_name,
                             dcGenericResource* resource) {
  typedef std::queue<dcGenericResource*> LoadingQueue;
  LoadingQueue loading_queue;
  loading_queue.push(resource);
  std::string root_path = owner_name + dc::dcGenericResource::PATH_SEPARATOR;
  do {
    dcGenericResource* resource = loading_queue.front();

    // Load members of the current resource
    RecordList members;
    if (!read(root_path + resource->getPathString(), &members)) return false;
    RecordList::iterator i;
    for (i = members.begin(); i != members.end(); ++i) {
      dcGenericResource* member =
        resource->acquireMember(
          i->member_name.c_str(),
          i->resource_type.c_str(),
          0);
      if (member) {
        member->interpret(i->value.c_str());
        loading_queue.push(member);
      }
    }

    // We're done loading this resource, so remove it from the queue
    loading_queue.pop();

  } while (!loading_queue.empty());
  return true;
}



//----[  save  ]---------------------------------------------------------------
bool dcResourceStorage::save(const std::string& owner_path,
                             dcGenericResource* resource) {
  typedef std::queue<dcGenericResource*> SavingQueue;
  SavingQueue saving_queue;
  saving_queue.push(resource);
  std::string root_path = owner_path + dc::dcGenericResource::PATH_SEPARATOR;
  if (!begin(owner_path, resource->getName())) return false;
  do {
    dcGenericResource* resource = saving_queue.front();
    dcGenericResource* owner = resource->getOwner();

    // Build and save this resource's record
    bool written;
    Record record;
    record.resource_type = resource->getTypeName();
    record.member_name.assign(resource->getName());
    resource->serialize(&record.value);
    if (owner) {
      //record.owner_path.assign(root_path + owner->getPathString());
      written = write(root_path + owner->getPathString(), &record);
    } else {
      //record.owner_path.assign(owner_path);
      written = write(owner_path, &record);
    }

    // If we couldn't save this resource, undo the transaction and exit
    if (!written) { end(false); return false; }

    // Add all resource children to the saving queue
    dcGenericResource::Array members = resource->getMembers();
    dcGenericResource::Array::iterator i;
    for (i = members.begin(); i != members.end(); ++i) {
      saving_queue.push(*i);
    }

    saving_queue.pop();
  } while (!saving_queue.empty());
  end(true);
  return true;
}




//----[  clear  ]--------------------------------------------------------------
void dcResourceStorage::clear() {
  begin("","");
  end(true);
}




}