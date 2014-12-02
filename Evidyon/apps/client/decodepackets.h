#ifndef __DECODEPACKETS_H__
#define __DECODEPACKETS_H__

#include <string>
#include "../shared/networkdecl.h"
#include "../shared/client-server/actorsync.h"
#include "../shared/evidyon_world.h"
#include "../shared/evidyon_storage.h"
#include "../shared/evidyon_avatar.h"


#define MAX_ITEMS_PER_TRADE AVATAR_INVENTORY_SIZE

#pragma pack(push, 1)
struct TradeItem
{
  unsigned type : 16;
  unsigned quantity : 8;
};
#pragma pack(pop)

namespace Evidyon {
enum ActorState;
enum ActorAction;
struct ClientAvatarInventoryItem;
struct ItemForTrade;

namespace Network {
namespace Packet {
enum RequestActionResult;
struct ActorSyncData;
struct FastSyncPacket;

namespace Client {
namespace Decode {

bool tellServerVersionUpdateInfo(ENetPacket* packet, bool* versionIsValid, bool* needsNewGamefile, bool* needsNewClient, std::string* updateText);

bool createAccountSucceeded(ENetPacket* packet, unsigned int* id);
bool logoutAccountConfirmed(ENetPacket* packet);
bool tellAccountInfo(ENetPacket* packet, float* daysLeft, unsigned int* characterIDArray, size_t* numberOfCharacters);
bool requestActionResult(ENetPacket* packet, ::Evidyon::Network::Packet::RequestActionResult* result);
bool tellCharacterName(ENetPacket* packet, unsigned int* id, char* characterNameBuffer, size_t bufferSize, float* world_x, float* world_y, unsigned int* actor_type, size_t visible_equipment[Evidyon::ACTORATTACHMENTPOINT_COUNT]);
bool createCharacterSucceeded(ENetPacket* packet, unsigned int* characterID);
bool tellCharacterMapLocation(ENetPacket* packet, unsigned int* id, int* mapX, int* mapY);

bool enterWorldSucceeded(ENetPacket* packet, ActorID* actorID, unsigned long* time_of_day, float* yourX, float* yourY, int* map_index, bool* male, int* raceIndex, int* classIndex, int* currentHP, int* maxHP, int* currentMP, int* maxMP,  unsigned int* money, unsigned int* geonite, Evidyon::SpellAvailabilityMask* spellAvailabilityMask);
bool leaveWorldConfirmed(ENetPacket* packet);
bool tellActorActivity(ENetPacket* packet, int* actorID, float* x, float* y, float* angle, float* speed, float* distanceToMove, ActorState* currentState, ActorAction* action, float* actionSpeed);
bool destroyActor(ENetPacket* packet, int* actorID);
bool actorSpeaks(ENetPacket* packet, int* actorID, char* source_name_buffer, size_t source_name_buffer_size, char* speechBuffer, size_t bufferSize); // the name is specified if the actor ID == -1
//bool updateYourCurrentHPMPEXP(ENetPacket* packet, float* currentHPPercent, float* currentMPPercent, float* currentEXPPercent);

//bool removeInventoryItems(ENetPacket* packet, Evidyon::ClientAvatarInventoryItem fullClientInventory[AVATAR_INVENTORY_SIZE]);
//bool tellGroundItems(ENetPacket* packet, int* x, int* y, size_t* itemTypes, size_t bufferSize, size_t* numberOfItems);

//bool tellActorDescription(ENetPacket* packet, std::string* actorName, int* actorID, size_t* actorType, ActorState* currentState, size_t visibleEquipment[ACTORATTACHMENTPOINT_COUNT], size_t visibleEnchantments[Evidyon::NUMBER_OF_ENCHANTMENT_SLOTS]);

//bool updateYourActivePartition(ENetPacket* packet, short* px, short* py);

bool updateYourEffectAvailabilityMask(ENetPacket* packet, Evidyon::SpellAvailabilityMask* mask);

bool tellYourAvatarStats(ENetPacket* packet, unsigned char* level, unsigned char abilities[5], unsigned char* availableAbilityPoints,
              int* hp, int* hpMax, int* mp, int* mpMax, int* exp, int* nextLevelExp, int* defense, AvatarRace* avatar_race, AvatarClass* avatar_class);


bool tellCurrentTrade(ENetPacket* packet,
            int* trade_key,
            unsigned char* your_trade_item_indices,
            unsigned int* number_of_items,
            TradeItem other_trade_items[MAX_ITEMS_PER_TRADE], 
            unsigned int* other_number_of_items);
bool finishedTrading(ENetPacket* packet);

bool tellYourMoney(ENetPacket* packet, unsigned int* money);
bool bazaar_tellItemsForSale(ENetPacket* packet, Evidyon::ItemForTrade* items_buffer, unsigned int buffer_size, unsigned int* number_of_items);
bool bazaar_tellSellerStatus(ENetPacket* packet, Evidyon::ItemForTrade* items_buffer, unsigned int buffer_size, unsigned int* number_of_items);
bool bazaar_removeItemListings(ENetPacket* packet, unsigned int* item_listings_buffer, unsigned int buffer_size, unsigned int* number_of_items);

bool changeMap(ENetPacket* packet, int* map_id);
bool eraseActorFromWorld(ENetPacket* packet, int* actor_id);
bool slowSync(ENetPacket* packet, float* time_since_last_sync, unsigned int* actors, ActorSyncData* updates_buffer, unsigned int buffer_size);
bool fastSync(ENetPacket* packet, Evidyon::Network::Packet::FastSyncPacket* data);

bool createMagicFX(ENetPacket* packet, int* type, int* src_actor_id,
           float* src_x, float* src_y,
           int* power, int* target_ids, int target_buffer_entries,
           int* number_of_targets);

bool createProjectile(ENetPacket* packet, int* projectile_id, int* projectile_type, float* x, float* y, float* dx, float* dy);
bool destroyProjectile(ENetPacket* packet, int* projectile_id);

// the item_types buffer should have MAX_ITEMS_VISIBLE_PER_LOCATION elements available
bool tellItemsOnMapLocation(ENetPacket* packet,
                            int* world_x,
                            int* world_y,
                            unsigned short* item_types,
                            int* number_of_item_types);


bool updateOrTellInventory(ENetPacket* packet,
                           Evidyon::ClientAvatarInventoryItem fullClientInventory[Evidyon::AVATAR_INVENTORY_SIZE],
                           int* highest_inventory_update_index,
                           int inventory_update_indices[Evidyon::AVATAR_INVENTORY_SIZE]);

bool tellActorName(ENetPacket* packet,
                   ActorID* actor_id,
                   std::string* name);
bool updateLocalWorldRegion(ENetPacket* packet,
                            World::RegionID* region_id);

bool storageTell(ENetPacket* packet,
                 Evidyon::ItemInStorage items[Evidyon::MAX_ITEMS_IN_STORAGE],
                 int* number_of_items);

bool geosidTellGeonite(ENetPacket* packet,
                       unsigned int* geonite);

}
}
}
}
}


#endif