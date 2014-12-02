#ifndef __SERVERSTORAGE_H__
#define __SERVERSTORAGE_H__

#include "../shared/evidyon_storage.h"
#include "itemidtype.h"

namespace Evidyon {

// This is the structure held by Client and managed by the global database
struct ServerStorageItem {
  ItemIDType id;
  unsigned int type;
  int quantity;
};

}

#endif