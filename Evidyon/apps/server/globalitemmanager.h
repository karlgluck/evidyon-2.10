#ifndef __GLOBALITEMMANAGER_H__
#define __GLOBALITEMMANAGER_H__

#include <dcx/memorypool>
#include <map>
#include "itemidtype.h"
#include "../shared/evidyon_world.h"
#include <hash_map>
#include "worldregion_items.h"

struct _iobuf;
typedef struct _iobuf FILE;


namespace Evidyon {

class Item;
class WorldRegion;
class Client;
struct AvatarInventoryItem;
struct ServerItemDescription;
struct CompiledTreasureClass;
class Map;
struct AvatarInventoryItem;
struct DatabaseInventoryItem;

// 16 bits per item
// quality: 
// quantity: 
// qualtiyquantity 1 byte each

struct TreasureClassDescription;


class GlobalItemManager {
public:
  static const int MAX_ITEMS = 1<<16;


private:
  typedef dcx::dcxMemoryPool<Item, MAX_ITEMS> ItemMemoryPool;
  //typedef std::multimap<WorldRegion*, ItemOnGround> ActiveItems;

  // This table holds items that are in partitions that are not visible
  //typedef std::multimap<World::RegionID,ItemOnGround> InactiveItems;

  typedef stdext::hash_map<unsigned int, unsigned int> PermanentStorageHashToDescriptionIndexTable;

public:
  static GlobalItemManager* singleton();


public:
  // this loads both item and treasure data from the game file
  bool create(FILE* game_file);
  void destroy();

  // Creates items on the ground when a monster dies
  void dropMonsterTreasure(Map* map,
                           int x,
                           int y,
                           int treasure_class_index,
                           unsigned int treasure_randomizer_mask);
  void dropAvatarItemsOnDeath(Map* map,
                              int x,
                              int y,
                              AvatarInventoryItem* inventory_item);

  // Called by a WorldRegion when it is being released to erase all
  // of the items in its local set.
  void releaseWorldRegionItems(WorldRegion_LocalItems* local_items);
  void releaseWorldRegionItems(WorldRegion_LocalItems* local_items,
                               WorldRegion_LocalItems::iterator first,
                               WorldRegion_LocalItems::iterator last);

  // Tries to stack the quantity from "source" into "destination".  If the
  // method has an effect on either item, the return value is 'true'.  There
  // is a possibility that the item stored in 'source' is invalidated as
  // a result of this operation.
  bool stack(AvatarInventoryItem* destination,
             AvatarInventoryItem* source);

  // Erases all items from the client's inventory
  void releaseAvatarInventory(AvatarInventoryItem* inventory);

  // Erase a single item from the avatar's inventory
  void releaseAvatarInventoryItem(AvatarInventoryItem* inventory_item);

  // Reduces the quantity of this item.  If the quantity is 0 or this entry
  // is not stackable, the location is reset and the item resource freed.
  // Returns 'true' if an item was successfully consumed.
  bool consumeAvatarInventoryItem(AvatarInventoryItem* inventory_item);

  // Takes the data from an item in the avatar's inventory and creates a
  // structure containing information that is saved to the database
  // If the return value is 'false', the translation failed.
  bool translateAvatarItemToDatabaseItem(const AvatarInventoryItem* inventory_item,
                                         DatabaseInventoryItem* database_item);

  // Creates a new item and configures the inventory slot for an item from
  // the database.  If the item couldn't be acquired, the return value is 'false'
  bool acquireAvatarItemForDatabaseItem(const DatabaseInventoryItem* database_item,
                                        AvatarInventoryItem* inventory_item);

  bool translatePermanentStorageHashToItemType(unsigned int hash, unsigned int* item_type);

public:
  // made public for the sole purpose of being able to generate items w/ an admin command
  Item* acquireItem(int quantity, unsigned int item_type);

private:
  TreasureClassDescription* getTreasureClassDescription(size_t index);
  Item* acquireItem(ItemIDType unique_id,
                    int quantity,
                    unsigned int item_type);
  void releaseItem(Item* item);



private:
  ItemMemoryPool item_memory_pool_;
  //ActiveItems active_items_;
  //InactiveItems inactive_items_;

  // This is built when the game-file is loaded to interpret saved avatar item
  // hashes into item types whose descriptions can be directly indexed.
  PermanentStorageHashToDescriptionIndexTable permanent_hash_to_description_index_table_;

  ServerItemDescription* item_descriptions_;
  size_t number_of_item_descriptions_;
  TreasureClassDescription* treasure_classes_;
  size_t number_of_treasure_classes_;

private:
  static GlobalItemManager* singleton_;

private:

  /**
   * Sends an update for all of the items on the ground at the given location
   */
  //void broadcastGroundItemsUpdate(WorldRegion* region, int x, int y);
  //Item* acquireNewItem(int quantity, size_t itemType, unsigned short* itemIndex);
  //Item* acquireStructForExistingItem(ItemIDType unique_id, int quantity, size_t itemType, unsigned short* itemIndex);
  //void releaseItem(unsigned short itemIndex, unsigned short itemHash);
  //void releaseItem(Item* item);
  //Item* getItem(unsigned short itemIndex, unsigned short itemHash);
  //bool validateItem(unsigned short itemIndex, unsigned short itemHash);
  //void removeItemsFrom(WorldRegion* region);
  //void spawnItemsIn(WorldRegion* region);

  ///**
  // * Sends all of the items in the given partition to the client
  // */
  //void updateClientWithItemsOnGroundInRegion(WorldRegion* region, Client* client);
  ///**
  // * Drops items that have already been created from a player's inventory.  This method
  // * will both invalidate the items in the inventory array and regenerate the items'
  // * hashes as a security measure to prevent duping.
  // * If 'null' is passed for indicesOfItems, items are dropped due to the character dying
  // */
  //void dropItemsFromInventory(WorldRegion* client_region,
  //                            AvatarInventoryItem* inventory,
  //                            unsigned char* indicesOfItems, size_t numberOfItems,
  //                            int x,
  //                            int y);

  ///**
  // * Gives the items at the designated coordinates to the client
  // */
  //void pickUpItems(WorldRegion* region, int x, int y, Client* client);

  ///**
  // * Puts the given items on the ground.  These items should have been spawned specifically for this purpose; i.e.
  // * they should be created when a monster dies and not stored in an inventory somewhere.  The item hashes
  // * are regenerated and the source items list is nulled as security measures.
  // */
  //void dropLootOnGround(WorldRegion* region, int x, int y, Item** items, size_t numberOfItems);
  //void dropMoneyOnGround(WorldRegion* region, bool is_loot, int x, int y, int amount);
};

}

#endif