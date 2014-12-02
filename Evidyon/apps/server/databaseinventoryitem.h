#ifndef __DATABASEINVENTORYITEM_H__
#define __DATABASEINVENTORYITEM_H__

#include "itemidtype.h"


namespace Evidyon {

struct DatabaseInventoryItem {
  ItemIDType id;
  unsigned int permanent_storage_hash;
  int quantity;
  bool equipped;
};


}


#endif