#ifndef __EVIDYON_PACKETS_SERVER_ENCODE_H__
#define __EVIDYON_PACKETS_SERVER_ENCODE_H__

struct _ENetPacket;
typedef struct _ENetPacket ENetPacket;

#include "../shared/evidyon_actor.h"
#include "../shared/evidyon_enchantments.h"
#include "../shared/client-server/actorsync.h"
#include "../shared/evidyon_world.h"
#include "../shared/evidyon_avatar.h"

namespace Evidyon {

struct AvatarInventoryItem;
struct ItemForTrade;
struct SpellAvailabilityMask;
enum ActorState;
enum ActorAction;
struct AvatarAttributes;
class Actor;
class Client;
struct ItemInStorage;

namespace Network {
enum RequestActionResult;

namespace Packet {
struct ActorDescriptionData;
struct FastSyncPacket;

namespace Server {
namespace Encode {

ENetPacket* tellServerVersionUpdateInfo(bool versionIsValid, bool needsNewGamefile, bool needsNewClient, const char* updateText = 0);

ENetPacket* createAccountSucceeded(unsigned int accountID);
ENetPacket* logoutAccountConfirmed();
ENetPacket* tellAccountInfo(float daysLeft, const unsigned int* characterIDArray, size_t numberOfCharacters);
ENetPacket* requestActionResult(RequestActionResult result);
ENetPacket* tellCharacterName(unsigned int id, const char* characterName, float world_x, float world_y, unsigned int actor_type, size_t* visible_equipment);
ENetPacket* createCharacterSucceeded(unsigned int characterID);
ENetPacket* tellCharacterMapLocation(unsigned int id, int mapX, int mapY);

ENetPacket* enterWorldSucceeded(ActorID actorID,
                                unsigned long time_of_day,
                                float yourX,
                                float yourY,
                                int map_index,
                                const AvatarAttributes* avatar,
                                const SpellAvailabilityMask* spellAvailabilityMask);
ENetPacket* leaveWorldConfirmed();
//ENetPacket* tellActorActivity(int actorID, float angle, float speed, float x, float y, float distanceToMove, ActorState currentState, ActorAction action, float actionSpeed);
ENetPacket* destroyActor(int actorID);
ENetPacket* actorSpeaks(int actorID, const char* name, const char* speech);


ENetPacket* updateYourEffectAvailabilityMask(const SpellAvailabilityMask* mask);

ENetPacket* tellYourAvatarStats(unsigned char level,
                                unsigned char abilities[5],
                                unsigned char availableAbilityPoints,
                                int hp,
                                int hpMax,
                                int mp,
                                int mpMax,
                                int exp,
                                int nextLevelExp,
                                int defense,
                                Evidyon::AvatarRace avatar_race,
                                Evidyon::AvatarClass avatar_class);

ENetPacket* tellCurrentTrade(int trade_key,
               unsigned char* your_trade_item_indices,
               unsigned int number_of_items,
               AvatarInventoryItem* other_inventory,  // parsed into a list of item-descriptors (doesn't send other inventory)
               unsigned char* other_trade_item_indices,
               unsigned int other_number_of_items);
ENetPacket* finishedTrading();  // message-only

ENetPacket* tellYourMoney(unsigned int money);
ENetPacket* bazaar_tellItemsForSale(ItemForTrade* items, unsigned int number_of_items);
ENetPacket* bazaar_tellSellerStatus(ItemForTrade* items, unsigned int number_of_items);
ENetPacket* bazaar_removeItemListings(unsigned int* item_listings, unsigned int number_of_items);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ENetPacket* changeMap(int map_id);
ENetPacket* eraseActorFromWorld(int actor_id);

// Builds a NETMSG_TELLACTORFULLDESCRIPTIONS for only one actor.  This method
// should be used when an actor enters the world for the first time, such as
// when a client logs in.
ENetPacket* tellSingleActorFullDescription(const ActorDescriptionData& description, const char* name);

ENetPacket* fastSync(const FastSyncPacket& data);
ENetPacket* createMagicFX(int type, int src_actor_id, float src_x, float src_y,
              int power, int* target_ids, int number_of_targets);
ENetPacket* createProjectile(int projectile_id, int projectile_type, float x, float y, float dx, float dy);
ENetPacket* destroyProjectile(int projectile_id);

// since we have defined the world as being no bigger than 2^16 tiles wide,
// (it is actually substantially less) world_x and world_y can be packed
// into 16 bits
ENetPacket* tellItemsOnMapLocation(unsigned int world_x,
                                   unsigned int world_y,
                                   unsigned short* item_types,  // derived from item description
                                   int number_of_item_types);   // entries in array; always <= MAX_ITEMS_VISIBLE_PER_LOCATION

// The client should get rid of state-dependent structures whose location is
// beyond a world region bordering the one with this ID number.
// This means, erase items on the ground in far-off areas.
ENetPacket* updateLocalWorldRegion(World::RegionID region_id);


ENetPacket* updateInventory(bool complete_update,
                            AvatarInventoryItem* inventory,
                            int* inventory_indices,
                            int number_of_indices);

ENetPacket* tellActorName(ActorID actor_id,
                          const char* name);

ENetPacket* storageTell(ItemInStorage* storage, int number_of_items);

ENetPacket* geosidTellGeonite(unsigned int geonite);

}
}
}
}
}

#endif