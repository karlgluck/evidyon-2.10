#include "globalitemmanager.h"
#include "worldregion.h"
#include "globaldatabasemanager.h"
#include <dc/debug>
#include "item.h"
#include "../shared/evidyon_avatarinventory.h"
#include "client.h"
#include "globalworldmanager.h"

#include "../shared/server-editor/compiledtreasure.h"
#include "randint32.h"
#include "../shared/server-editor/server_gamefile.h"

#include "../common/safemacros.h"
#include "avatarinventoryitem.h"
#include "databaseinventoryitem.h"


#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

namespace Evidyon {


struct TreasureClassDescription {
  CompiledTreasureClass info;
  CompiledTreasure* treasure;
};



GlobalItemManager* GlobalItemManager::singleton_ = NULL;
GlobalItemManager* GlobalItemManager::singleton() {
  return singleton_;
}




//------------------------------------------------------------------------------------------------
// Name: create
// Desc: Sets up this class for use
//------------------------------------------------------------------------------------------------
bool GlobalItemManager::create(FILE* file) {
  CONFIRM(item_memory_pool_.create()) else return false;

  size_t number_of_items = 0;
  CONFIRM(1 ==
          fread(&number_of_items,
                sizeof(number_of_items),
                1,
                file)) else return false;

  if (number_of_items) {
    number_of_item_descriptions_ = number_of_items;

    item_descriptions_ = new ServerItemDescription[number_of_item_descriptions_];
    CONFIRM(item_descriptions_ != NULL) else return false;

    CONFIRM(number_of_items ==
            fread(item_descriptions_,
                  sizeof(ServerItemDescription),
                  number_of_items,
                  file)) else return false;

    // Build the table of hash values
    permanent_hash_to_description_index_table_.clear();
    for (int i = 0; i < number_of_items; ++i) {
      PermanentStorageHashToDescriptionIndexTable::_Pairib result =
        permanent_hash_to_description_index_table_.insert(
            PermanentStorageHashToDescriptionIndexTable::value_type(
              item_descriptions_[i].permanent_storage_hash,
              i));

      // this shouldn't ever happen, since the hash is generated from
      // the editor's item name and this must always be unique.
      if (APP_FATAL(!result.second)("Item type hash collision!!!"))
        return false;
    }
  }
  else
    DEBUG_WARNING("No item descriptions!");


  {
    size_t number_of_treasure_classes = 0;
    CONFIRM(1 == fread(&number_of_treasure_classes,
                  sizeof(number_of_treasure_classes),
                  1,
                  file)) else return false;

    treasure_classes_ = new TreasureClassDescription[number_of_treasure_classes];
    number_of_treasure_classes_ = number_of_treasure_classes;
    for (size_t i = 0; i < number_of_treasure_classes; ++i) {
      CONFIRM(1 ==
              fread(&treasure_classes_[i].info,
                    sizeof(CompiledTreasureClass),
                    1,
                    file)) else return false;

      int treasure_item_count = treasure_classes_[i].info.treasure_item_count;
      treasure_classes_[i].treasure = new CompiledTreasure[treasure_item_count];
      CONFIRM(treasure_item_count ==
              fread(treasure_classes_[i].treasure,
                    sizeof(CompiledTreasure),
                    treasure_item_count,
                    file)) else return false;
    }

  }


  singleton_ = this;

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Erases the information contained in this class
//------------------------------------------------------------------------------------------------
void GlobalItemManager::destroy()
{
  singleton_ = NULL;

  if (treasure_classes_) {
    for (size_t i = 0; i < number_of_treasure_classes_; ++i) {
      SAFE_DELETE_ARRAY(treasure_classes_[i].treasure);
    }

    SAFE_DELETE_ARRAY(treasure_classes_);
  }

  SAFE_DELETE_ARRAY(item_descriptions_);

  //for (ActiveItems::iterator i = active_items_.begin();
  //     i != active_items_.end(); ++i) {
  //  i->first->removeReference();
  //}
  //active_items_.clear();

  item_memory_pool_.destroy();
}



int CalculateRarityIndex(unsigned int random_value) {
  random_value ^= random_value >> 16; // xor high/low words
  random_value &= 0xFFFF; // take the low-word only

  //// To initially generate the table algorithmically:
  //BitsSetTable256[0] = 0;
  //for (int i = 0; i < 256; i++)
  //  BitsSetTable256[i] = (i & 1) + BitsSetTable256[i / 2];
  static const unsigned char BitsSetTable256[] = 
  {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
  };

  // calculate the number of bits set, but
  int rarity_index = BitsSetTable256[ random_value     & 0xFF] +
                     BitsSetTable256[(random_value>>8) & 0xFF];

  // return abs(rarity_index - 8)
  rarity_index -= 8;
  return rarity_index < 0 ? -rarity_index : rarity_index;
}


void GlobalItemManager::dropMonsterTreasure(Map* map,
                                            int x,
                                            int y,
                                            int treasure_class_index,
                                            unsigned int treasure_randomizer_mask) {
  CONFIRM(map) else return;

  WorldRegion* region = map->getRegionFromPoint(x, y);
  CONFIRM(region) else return;

  const TreasureClassDescription* treasure_class =
      getTreasureClassDescription(treasure_class_index);
  CONFIRM(treasure_class) else return;

  int treasure_range = 0,
      number_of_items = 0;
  {
    unsigned int treasure_randomizer = rand_uint32();
    treasure_randomizer &= treasure_randomizer_mask;
    int treasure_level = CalculateRarityIndex(treasure_randomizer);
    CONFIRM(treasure_level >=0 && treasure_level < TREASURE_WEIGHTED_RANDOM_RANGE);
    treasure_range = treasure_class->info.treasure_index_range[treasure_level];

    int item_rarity_index = CalculateRarityIndex(rand_uint32());
    CONFIRM(item_rarity_index >=0 && item_rarity_index < TREASURE_WEIGHTED_RANDOM_RANGE);
    number_of_items = treasure_class->info.number_of_items_to_drop[item_rarity_index];
  }

  for (int i = 0; i < number_of_items; ++i) {
    int treasure_item_index = rand()%treasure_range;
    CONFIRM(treasure_item_index < treasure_class->info.treasure_item_count) else continue;
    unsigned int item_type = treasure_class->treasure[treasure_item_index].item_type;
    int quantity = 1; // todo

    Item* item = acquireItem(quantity, item_type);
    CONFIRM(item) else continue;
    region->addItemOnGround(false, x, y, item);
  }

  if (number_of_items > 0) {
    region->broadcastUpdateItemsOnLocation(x, y);
  }
}




void GlobalItemManager::dropAvatarItemsOnDeath(Map* map,
                                               int x,
                                               int y,
                                               AvatarInventoryItem* inventory) {
  CONFIRM(map) else return;

  WorldRegion* region = map->getRegionFromPoint(x, y);
  CONFIRM(region) else return;

  int inventory_dropped = 0;

  // free all inventory items and count the number of equipped items
  int num_equipped_items = 0;
  int first_equipped_item = AVATAR_INVENTORY_SIZE, last_equipped_item = 0;
  for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i) {
    if (inventory[i].equipped) {
      first_equipped_item = min(first_equipped_item, i);
      last_equipped_item = max(last_equipped_item, i);
      num_equipped_items++;
    } else {
      Item* item = inventory[i].item;
      if (item) {
        region->addItemOnGround(true, x, y, item);
        inventory[i].item = 0;
        inventory[i].stackable = false;
        ++inventory_dropped;
      }
    }
  }

  // Equipped items have a 1/DROP_PROBABILITY_DENOMINATOR chance
  // of being dropped on death
  static const int DROP_PROBABILITY_DENOMINATOR = 16;
  int equipment_dropped = 0;
  for (int i = first_equipped_item; i <= last_equipped_item; ++i) {
    if (false == inventory[i].equipped) continue;
    if (0 < rand_uint32()%DROP_PROBABILITY_DENOMINATOR) continue; // probability
    Item* item = inventory[i].item;
    CONFIRM(item != NULL) else continue;
    ++equipment_dropped;
    region->addItemOnGround(true, x, y, item);
    inventory[i].item = 0;
    inventory[i].stackable = false;
    inventory[i].equipped = false;
  }

  if ((inventory_dropped > 0) ||
      (equipment_dropped > 0)) {
    region->broadcastUpdateItemsOnLocation(x, y);
  }
}




void GlobalItemManager::releaseWorldRegionItems(
    WorldRegion_LocalItems* local_items) {
  WorldRegion_LocalItems::iterator lower = local_items->begin();
  WorldRegion_LocalItems::iterator upper = local_items->end();
  for (WorldRegion_LocalItems::iterator i = lower; i != upper; ++i) {
    releaseItem(i->second);
  }
  local_items->clear();
}

void GlobalItemManager::releaseWorldRegionItems(
    WorldRegion_LocalItems* local_items,
    WorldRegion_LocalItems::iterator first,
    WorldRegion_LocalItems::iterator last) {
  for (WorldRegion_LocalItems::iterator i = first; i != last; ++i) {
    releaseItem(i->second);
  }
  local_items->erase(first, last);
}



bool GlobalItemManager::stack(AvatarInventoryItem* destination,
                              AvatarInventoryItem* source) {
  Item* dest_item = destination->item;
  Item* src_item = source->item;
  CONFIRM(dest_item && src_item) else return false;

  // only stack items of the same type
  const ServerItemDescription* description = dest_item->getDescription();
  if (description != src_item->getDescription()) return false;

  // can't stack unstackable items
  int max_stack_size = description->max_stack_size;
  CONFIRM(max_stack_size > 1) else return false;

  // how much space is left in the destination?
  int dest_quantity = dest_item->getQuantity();
  CONFIRM(dest_quantity > 0) else { // get rid of this item
    dest_item->release();
    releaseItem(dest_item);
    destination->item = NULL;
    destination->equipped = false;
    destination->stackable = false;
    return true;
  }
  int available_quantity_in_dest_stack = max_stack_size - dest_quantity;
  if (available_quantity_in_dest_stack <= 0) {

    // If this is not true, the destination item's quantity is greater
    // than the maximum stack size for the item.  Cut the stack size.
    CONFIRM(available_quantity_in_dest_stack == 0) else {
      dest_item->setQuantity(max_stack_size);
      return true;
    }

    // can't stack on to this item; it's full
    return false;
  }

  int source_quantity = src_item->getQuantity();
  if (available_quantity_in_dest_stack >= source_quantity) {
    { // get rid of the item because the destination stack
      // is large enough to hold this item completely
      src_item->release();
      releaseItem(src_item);
      source->item = NULL;
      source->equipped = false;
      source->stackable = false;
    }
    if (source_quantity > 0) {
      dest_item->setQuantity(dest_quantity +
                             source_quantity); // <= max_stack_size
    } else {
      DEBUG_WARNING("item with <= 0 quantity attempted to stack");
    }
  } else {
    // the destination stack will be filled by adding part of the
    // quantity from the source item
    src_item->setQuantity(source_quantity -
                          available_quantity_in_dest_stack); // > 0
    dest_item->setQuantity(max_stack_size);
  }

  return true;
}


void GlobalItemManager::releaseAvatarInventory(AvatarInventoryItem* inventory) {
  for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i) {
    inventory[i].equipped = false;
    inventory[i].stackable = false;

    Item* item = inventory[i].item;
    if (NULL == item) continue;
    releaseItem(item);

    inventory[i].item = NULL;
  }
}

void GlobalItemManager::releaseAvatarInventoryItem(AvatarInventoryItem* inventory_item) {
  inventory_item->equipped = false;
  inventory_item->stackable = false;
  Item* item = inventory_item->item;
  if (NULL == item) return;
  releaseItem(item);
  inventory_item->item = NULL;
}


bool GlobalItemManager::consumeAvatarInventoryItem(AvatarInventoryItem* inventory_item) {
  Item* item = inventory_item->item;
  CONFIRM(item) else return false;
  if (!item->isStackable()) return false;
  int quantity = item->getQuantity() - 1;
  if (quantity <= 0) {// || (false == item->isStackable())) {
    item->release();
    inventory_item->equipped = false;
    inventory_item->stackable = false;
    inventory_item->item = NULL;
  } else {
    item->setQuantity(quantity);
  }
  return true;
}


bool GlobalItemManager::translateAvatarItemToDatabaseItem(
    const AvatarInventoryItem* inventory_item,
    DatabaseInventoryItem* database_item) {
  Item* item = inventory_item->item;
  if (NULL == item) return false;
  database_item->id = item->getUniqueID();
  database_item->permanent_storage_hash = item->getDescription()->permanent_storage_hash;
  database_item->equipped = inventory_item->equipped;
  {
    int quantity = item->getQuantity();
    CONFIRM(quantity > 0) else return false;
    database_item->quantity = quantity;
  }
  return true;
}

bool GlobalItemManager::acquireAvatarItemForDatabaseItem(
    const DatabaseInventoryItem* database_item,
    AvatarInventoryItem* inventory_item) {
  unsigned int item_type;
  if (!translatePermanentStorageHashToItemType(database_item->permanent_storage_hash, &item_type)) return false;
  Item* item = acquireItem(database_item->quantity,
                           item_type);
  CONFIRM(item) else return false;

  inventory_item->stackable = item->isStackable();
  inventory_item->equipped = database_item->equipped;
  inventory_item->item = item;
  return true;
}


bool GlobalItemManager::translatePermanentStorageHashToItemType(unsigned int hash, unsigned int* item_type) {
  PermanentStorageHashToDescriptionIndexTable::iterator i =
    permanent_hash_to_description_index_table_.find(hash);
  if (i == permanent_hash_to_description_index_table_.end()) return false;
  *item_type = i->second;
  return true;
}





TreasureClassDescription* GlobalItemManager::getTreasureClassDescription(size_t index) {
  return index < number_of_treasure_classes_ ? &treasure_classes_[index] : NULL;
}


Item* GlobalItemManager::acquireItem(int quantity, unsigned int item_type) {
  unsigned __int64 lower = rand_uint32();
  unsigned __int64 upper = rand_uint32();
  ItemIDType unique_id = static_cast<__int64>((upper << 32) | lower);
  return acquireItem(unique_id, quantity, item_type);
}

Item* GlobalItemManager::acquireItem(ItemIDType unique_id,
                                     int quantity,
                                     unsigned int item_type) {
  CONFIRM(item_type < number_of_item_descriptions_) else return NULL;
  CONFIRM(quantity > 0) else { // assign a default quantity
    quantity = 1;
  }
  const ServerItemDescription* description = &item_descriptions_[item_type];

  Item* item = item_memory_pool_.acquire();
  CONFIRM(item) else return NULL;
  item->acquire(unique_id, quantity, description);
  return item;
}



void GlobalItemManager::releaseItem(Item* item) {
  CONFIRM(item) else return;
  item->release();
  item_memory_pool_.release(item);
}



//
//
////------------------------------------------------------------------------------------------------
//Item* GlobalItemManager::acquireNewItem(int quantity, size_t itemType, unsigned short* itemIndex)
//{
//  ItemIDType unique_id = 0;
//  GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();
//  CONFIRM(GlobalDatabaseManager::DBR_SUCCESS == database->itemGenerated(itemType, quantity, &unique_id)) else return NULL;
//  Item* item = item_memory_pool_.acquire();
//  if (APP_WARNING(!item)("Out of item structures!")) return NULL;
//
//  item->acquire(unique_id, quantity, itemType);
//  if (itemIndex) *itemIndex = item_memory_pool_.indexOf(item);
//
//  return item;
//}
//
//
////------------------------------------------------------------------------------------------------
//// Name: acquireStructForExistingItem
//// Desc: 
////------------------------------------------------------------------------------------------------
//Item* GlobalItemManager::acquireStructForExistingItem(ItemIDType unique_id, int quantity, size_t itemType, unsigned short* itemIndex)
//{
//  // Get an item structure from the memory pool
//  Item* item = item_memory_pool_.acquire();
//  if (APP_WARNING(!item)("Out of item structures!")) return NULL;
//
//  // Initialize the item
//  item->acquire(unique_id, quantity, itemType);
//  if (itemIndex) *itemIndex = item_memory_pool_.indexOf(item);
//
//  // Return the item
//  return item;
//}
//
////------------------------------------------------------------------------------------------------
//// Name: releaseItem
//// Desc: Releases an item
////------------------------------------------------------------------------------------------------
//void GlobalItemManager::releaseItem(unsigned short itemIndex, unsigned short itemHash)
//{
//  releaseItem(getItem(itemIndex, itemHash));
//}
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: releaseItem
//// Desc: Releases an item
////------------------------------------------------------------------------------------------------
//void GlobalItemManager::releaseItem(Item* item)
//{
//  if (!item) return;
//  item->release();
//  item_memory_pool_.release(item);
//}
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: getItem
//// Desc: Obtains a reference to the item at the requested item index, given that the hash is valid
////------------------------------------------------------------------------------------------------
//Item* GlobalItemManager::getItem(unsigned short itemIndex, unsigned short itemHash)
//{
//  // Get this item from the memory pool
//  Item* item = item_memory_pool_.get(itemIndex);
//
//  // Only return the item if the hash matches the item's current hash.  This is a security
//  // measure to prevent the possibility of duplicating items from different item structures
//  // becoming desynchronized.
//  return (!item || !item->validate(itemHash)) ? 0 : item;
//}
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: validateItem
//// Desc: Determines whether or not the item is valid.
////------------------------------------------------------------------------------------------------
//bool GlobalItemManager::validateItem(unsigned short itemIndex, unsigned short itemHash)
//{
//  return getItem(itemIndex, itemHash) != 0;
//}
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: removeItemsFrom
//// Desc: Removes all of the items from the given region
////------------------------------------------------------------------------------------------------
//void GlobalItemManager::removeItemsFrom(WorldRegion* region)
//{
//  World::RegionID region_id = region->getRegionID();
//
//  // Get iterators for this location
//  ActiveItems::iterator lower = active_items_.lower_bound(region);
//  ActiveItems::iterator upper = active_items_.upper_bound(region);
//
//  // Iterate through items on this location
//  GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();
//  for (ActiveItems::iterator i = lower; i != upper; ++i) {
//    if (i->second.importance_ > 0) {
//    inactive_items_.insert(InactiveItems::value_type(region_id, i->second));
//    } else {
//    if (i->second.is_money_ == false) {
//      Item* item = i->second.item;
//      database->itemOnGroundErased(item->getUniqueID());
//      releaseItem(item);      // Get rid of this item and return its memory
//    }
//    }
//
//    region->removeReference(); // Get rid of references in this region
//  }
//
//  active_items_.erase(lower, upper);
//}
//
//// Re-adds items to a world region when it becomes active again
////------------------------------------------------------------------------------------------------
//void GlobalItemManager::spawnItemsIn(WorldRegion* region) {
//  World::RegionID region_id = region->getRegionID();
//  InactiveItems::iterator lower = inactive_items_.lower_bound(region_id);
//  InactiveItems::iterator upper = inactive_items_.upper_bound(region_id);
//  for (InactiveItems::iterator i = lower; i != upper; ++i) {
//  --i->second.importance_;
//  active_items_.insert(ActiveItems::value_type(region, i->second));
//  region->addReference();
//  }
//  inactive_items_.erase(lower, upper);
//}
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: dropItemsFromInventory
//// Desc: Drops items that have already been created from a player's inventory
////------------------------------------------------------------------------------------------------
//void GlobalItemManager::dropItemsFromInventory(WorldRegion* client_region,
//  AvatarInventoryItem* inventory, unsigned char* indicesOfItems,
//  size_t numberOfItems, int x, int y)
//{
//  // Validate the number of items
//  if (numberOfItems > AVATAR_INVENTORY_SIZE)
//  {
//    DEBUG_WARNING("The number of items being dropped from the inventory is more than the inventory's actual size");
//    numberOfItems = AVATAR_INVENTORY_SIZE;
//  }
//
//  GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();
//
//  // Dropping due to death
//  if (!indicesOfItems) numberOfItems = AVATAR_INVENTORY_SIZE;
//  bool equippedItemDropped = false;
//
//  // Move each of the items to the ground
//  for (size_t i = 0; i < numberOfItems; ++i)
//  {
//    // Obtain the indexed item
//    unsigned int inventoryIndex = indicesOfItems ? indicesOfItems[i] : i;
//    CONFIRM(inventoryIndex < AVATAR_INVENTORY_SIZE) else continue;
//    if (inventory[inventoryIndex].invalid) continue;
//
//    // If we're dropping items due to death, make sure this is an inventory or an equipped item
//    if (!indicesOfItems && inventory[inventoryIndex].equipped && equippedItemDropped) continue;
//    if (inventory[inventoryIndex].equipped && !equippedItemDropped)
//    {
//      if (rand()%5 == 0) continue; // only drop 1/5 times
//      equippedItemDropped = true;
//    }
//
//    // Get the item from the inventory
//    Item* item = getItem(inventory[inventoryIndex].itemHashIndex, inventory[inventoryIndex].itemHashValue);
//    if (item == NULL) { // prevent invalid items
//      inventory[inventoryIndex].invalid = 1;
//      continue;
//    }
//    database->itemDroppedByOwner(item->getUniqueID()); // make sure this character loses the item in the database
//
//    // Make absolutely sure this inventory item in the inventory is now invalid
//    inventory[inventoryIndex].itemHashIndex = 0;
//    inventory[inventoryIndex].invalid = 1;
//    inventory[inventoryIndex].type = 0xFFFFFFFF;
//    inventory[inventoryIndex].quantity = 0;
//
//    // If this item was not found, move on
//    if (!item) continue;
//
//    // Drop this item on the ground and release it
//    item->regenerateHash();
//    bool dropped_willingly = indicesOfItems != NULL;
//    ItemOnGround itemOnGround;
//    itemOnGround.importance_ = dropped_willingly ? DROPPED_ITEMS_IMPORTANCE : DROP_ON_DEATH_ITEMS_IMPORTANCE;
//    itemOnGround.is_money_ = false;
//    itemOnGround.x = x;
//    itemOnGround.y = y;
//    itemOnGround.item = item;
//    active_items_.insert(ActiveItems::value_type(client_region, itemOnGround));
//    client_region->addReference();
//  }
//
//  //DEBUG_INFO("Dropped %lu items", numberOfItems);
//
//  // Update all of the clients with the new set of items at this location
//  broadcastGroundItemsUpdate(client_region, x, y);
//}
//
//
////------------------------------------------------------------------------------------------------
//// Name: pickUpItems
//// Desc: Gives the items at the designated coordinates to the client
////------------------------------------------------------------------------------------------------
//void GlobalItemManager::pickUpItems(WorldRegion* region, int x, int y, Client* client)
//{
//  // This array contains the indices of the items that were picked up, so that the client can
//  // be updated with their presence.
//  unsigned char inventoryIndices[MAX_ITEMS_PICKED_UP_AT_ONCE];
//  size_t numberOfItems = 0;
//
//  // The initial value should be zero, otherwise the bestowItem may be non-optimal
//  inventoryIndices[0] = 0;
//
//  // Get iterators for this location
//  ActiveItems::iterator start = active_items_.lower_bound(region);
//  ActiveItems::iterator end = active_items_.upper_bound(region);
//
//  GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();
//
//  // Iterate through items on this location and give them to the client
//  for (ActiveItems::iterator i = start;
//     i != end && numberOfItems < MAX_ITEMS_PICKED_UP_AT_ONCE;)
//  {
//    if (i->second.x != x || i->second.y != y)
//    {
//      ++i;
//      continue;
//    }
//
//    // Give this item to the client
//    if (i->second.is_money_) {
//      client->changeMoney(i->second.money_);
//    } else {
//      Item* item = i->second.item;
//      database->itemPickedUp(client->getLoggedIntoCharacterID(), item->getUniqueID());
//      if (client->bestowItem(item_memory_pool_.indexOf(item), item, &inventoryIndices[numberOfItems]))
//      {
//        int lastNumberOfItems = numberOfItems;
//        numberOfItems++;
//
//        // Optimize the next call
//        if (numberOfItems < MAX_ITEMS_PICKED_UP_AT_ONCE)
//          inventoryIndices[numberOfItems] = inventoryIndices[lastNumberOfItems];
//      }
//    }
//
//    // Remove this entry from the ground
//    ActiveItems::iterator next = i; ++next;
//    active_items_.erase(i);
//    i = next;
//    region->removeReference();
//  }
//
//  // Process picking up items 
//  if (numberOfItems)
//  {
//    // Tell the client about the items they just got
//    client->sendInventoryUpdate(inventoryIndices, numberOfItems);
//
//    // Update all of the clients with the new set of items at this location
//    broadcastGroundItemsUpdate(region, x, y);
//  }
//}
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: dropLootOnGround
//// Desc: Puts the given items on the ground
////------------------------------------------------------------------------------------------------
//void GlobalItemManager::dropLootOnGround(WorldRegion* region, int x, int y, Item** items, size_t numberOfItems)
//{
//  if (!region) return;
//
//  GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();
//
//  if (APP_ERROR(!items)("Invalid parameter to dropLootOnGround")) return;
//  for (size_t i = 0; i < numberOfItems; ++i)
//  {
//    // Skip this item if it is invalid
//    CONFIRM(items[i]) else continue;
//
//    // Add to the ground
//    items[i]->regenerateHash();
//    ItemOnGround itemOnGround;
//    itemOnGround.importance_ = LOOT_IMPORTANCE;
//    itemOnGround.is_money_ = false;
//    itemOnGround.x = x;
//    itemOnGround.y = y;
//    itemOnGround.item = items[i];
//    active_items_.insert(ActiveItems::value_type(region, itemOnGround));
//    region->addReference();
//    database->itemDroppedByOwner(items[i]->getUniqueID());
//
//    // Remove from source items list
//    items[i] = 0;
//  }
//
//  // Update all of the clients with the new set of items at this location
//  broadcastGroundItemsUpdate(region, x, y);
//}
//
//
//
////------------------------------------------------------------------------------------------------
//void GlobalItemManager::dropMoneyOnGround(WorldRegion* region, bool is_loot, int x, int y, int amount) {
//  ItemOnGround itemOnGround;
//  itemOnGround.importance_ = is_loot ? LOOT_IMPORTANCE : DROPPED_ITEMS_IMPORTANCE;
//  itemOnGround.is_money_ = true;
//  itemOnGround.x = x;
//  itemOnGround.y = y;
//  itemOnGround.money_ = amount;
//  if (region) {
//    active_items_.insert(
//      ActiveItems::value_type(region, itemOnGround));
//    region->addReference();
//  }
//}
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: updateClientWithItemsOnGroundInRegion
//// Desc: Sends all of the items in the given region to the client
////------------------------------------------------------------------------------------------------
//void GlobalItemManager::updateClientWithItemsOnGroundInRegion(
//  WorldRegion* region, Client* client)
//{
//  unsigned char entriesInPositions[World::REGION_SIZE][World::REGION_SIZE];
//  memset(entriesInPositions, 0, sizeof(entriesInPositions));
//
//  // Set flags for all of the entries
//  ActiveItems::iterator lower = active_items_.lower_bound(region);
//  ActiveItems::iterator upper = active_items_.upper_bound(region);
//  for (ActiveItems::iterator i = lower; i != upper; ++i)
//    ++entriesInPositions[i->second.x%World::REGION_SIZE][i->second.y%World::REGION_SIZE];
//
//  // Go through each of the entries and tell the items
//  for (int x = 0; x < World::REGION_SIZE; ++x)
//    for (int y = 0; y < World::REGION_SIZE; ++y)
//    {
//      int worldX = x + region->getRegionX() * World::REGION_SIZE;
//      int worldY = y + region->getRegionY() * World::REGION_SIZE;
//      size_t itemTypes[MAX_ITEMS_PER_LOCATION];
//      size_t numberOfItemsOnLocation = entriesInPositions[x][y];
//      numberOfItemsOnLocation = min(numberOfItemsOnLocation,MAX_ITEMS_PER_LOCATION); // prevent buffer overflow
//      size_t itemIndex = 0;
//      for (ActiveItems::iterator i = lower; i != upper && itemIndex < numberOfItemsOnLocation; ++i)
//      {
//        if (i->second.is_money_) continue;
//        CONFIRM(i->second.item) else continue;
//        if (i->second.x == worldX && i->second.y == worldY)
//          itemTypes[itemIndex++] = i->second.item->getItemType();
//      }
//
//      // Send if there are items here
//      if (numberOfItemsOnLocation)
//      {/*
//        // Build and send the update packet
//        ENetPacket* packet = EncodeServerPackets::tellGroundItems(worldX, worldY, itemTypes, numberOfItemsOnLocation);
//        enet_peer_send(client, 1, packet);*/
//        DEBUG_INFO("%lu updateClientWithItemsOnGroundInPartition sending packet", rand());
//      }
//    }
//}
//
//
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: broadcastGroundItemsUpdate
//// Desc: Sends an update for all of the items on the ground at the given location
////------------------------------------------------------------------------------------------------
//void GlobalItemManager::broadcastGroundItemsUpdate(WorldRegion* region, int x, int y)
//{
//  CONFIRM(region) else return;
//
//  // Start building the array that will hold the data to be sent to clients
//  size_t itemTypes[MAX_ITEMS_PER_LOCATION];
//  size_t numberOfItemsOnLocation = 0;
//  ActiveItems::iterator lower = active_items_.lower_bound(region);
//  ActiveItems::iterator upper = active_items_.upper_bound(region);
//  for (ActiveItems::iterator i = lower; i != upper && numberOfItemsOnLocation < MAX_ITEMS_PER_LOCATION; ++i)
//  {
//    if (i->second.is_money_) continue; // TODO: give the money an item type
//    CONFIRM(i->second.item);
//    if (i->second.x == x && i->second.y == y)
//      itemTypes[numberOfItemsOnLocation++] = i->second.item->getItemType();
//  }
///*
//  // Build the packet and broadcast it
//  ENetPacket* packet = EncodeServerPackets::tellGroundItems(x, y, itemTypes, numberOfItemsOnLocation);
//  if (packet)
//  {
//    //DEBUG_INFO("Telling about %i ground items at (%i, %i)", numberOfItemsOnLocation, x, y);
//    region->broadcastHereAndToNeighbors(packet);
//    if (!packet->referenceCount)
//      enet_packet_destroy(packet);
//  }*/
//}
//


}