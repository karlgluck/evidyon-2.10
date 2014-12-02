#ifndef __EVIDYON_AVATARINVENTORY_H__
#define __EVIDYON_AVATARINVENTORY_H__

namespace Evidyon {

static const int AVATAR_INVENTORY_SIZE = 128;


#pragma pack(push, 1)

struct ClientAvatarInventoryItem
{
  unsigned equipped          : 1;
  unsigned inventory_index   : 7;  // must be large enough to hold AVATAR_INVENTORY_SIZE
  unsigned quantity          : 8;
  unsigned type              : 16; // set to all 1's if this entry should be cleared
};

#pragma pack(pop)


}

#endif