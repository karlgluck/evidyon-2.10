#include "server_gamefile.h"
#include <dc/debug>


namespace Evidyon {

void ServerEquipmentSlotMask::item_initialize() {
  mask = 0x0000;
}

/**
 * This mask is going to be used for equipment, so it should reflect the available slots
 * on a character.  Thus, all slots where no items are equipped are '1'.  This method
 * makes all slots available.
 */
void ServerEquipmentSlotMask::equipment_initialize()
{
  mask = 0xFFFF;
}

/**
 * Returns 'false' if the item can't be equipped because other equipment blocks it
 */
bool ServerEquipmentSlotMask::equipment_equipMultipleSlotItem(const ServerEquipmentSlotMask* itemFilledSlotsMask)
{
  if ((itemFilledSlotsMask->mask & mask) != itemFilledSlotsMask->mask) return false;
  mask &= ~itemFilledSlotsMask->mask;
  return true;
}

/**
 *
 */
void ServerEquipmentSlotMask::equipment_unequipMultipleSlotItem(const ServerEquipmentSlotMask* itemFilledSlotsMask)
{
  // Make sure this item was equipped
  APP_ERROR(!(((itemFilledSlotsMask->mask ^ mask) & itemFilledSlotsMask->mask) == itemFilledSlotsMask->mask))("Unequipping item that was not equipped");

  // Free up the item's occupied slots
  mask |= itemFilledSlotsMask->mask;
}

/**
 * Gets the index of a slot into which an item that is able to fill single, different slots can be placed
 * and makes it unavailable.  If this method returns 'false', the item can't be equipped.
 */
bool ServerEquipmentSlotMask::equipment_equipSingleSlotItem(const ServerEquipmentSlotMask* itemCompatibilityMask)
{
  // Get the common bits.  For example, a ring is 000 111111 0000000.  If the current
  // mask is 110 111100 1101000, then common is 000 111100 0000000.  The first available
  // equipment index is 9.
  unsigned long common = itemCompatibilityMask->mask & mask;

  int availableIndex = getLowestBitSet(common);
  if (availableIndex < 0) return false;

  // Make this item's bit unavailable
  mask &= ~(1 << availableIndex);
  return true;
}

int ServerEquipmentSlotMask::getLowestBitSet(unsigned long ul)
{
  if (!ul) return -1;

  // count the number of zeroes to the right of the matched mask
  // from: http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightModLookup
  static const int Mod37BitPosition[] = // map a bit value mod 37 to its position
  {
    32, 0, 1, 26, 2, 23, 27, 0, 3, 16, 24, 30, 28, 11, 0, 13, 4,
    7, 17, 0, 25, 22, 31, 15, 29, 10, 12, 6, 0, 21, 14, 9, 5,
    20, 8, 19, 18
  };

  // Get the number of bits
  return Mod37BitPosition[(-ul & ul) % 37];
}

void ServerEquipmentSlotMask::equipment_unequipSingleSlotItem(const ServerEquipmentSlotMask* itemCompatibilityMask)
{
  // Get the bits from this equipment mask that are used which are potentially
  // filled by this item
  unsigned long unavailableMask = itemCompatibilityMask->mask & (~mask);

  // Find the first bit that is unavailable
  int bit = getLowestBitSet(unavailableMask);
  if (APP_ERROR(bit < 0)("Unequipping item that wasn't equippped!")) return;
  mask |= 1 << bit;
}

}