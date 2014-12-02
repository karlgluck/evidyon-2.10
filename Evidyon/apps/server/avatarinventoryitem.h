#ifndef __AVATARINVENTORYITEM_H__
#define __AVATARINVENTORYITEM_H__

namespace Evidyon {

class Item;

struct AvatarInventoryItem {
  bool equipped;
  bool stackable;
  Item* item;
};

}


#endif