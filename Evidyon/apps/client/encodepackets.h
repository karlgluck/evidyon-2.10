#ifndef __EVIDYON_PACKETS_CLIENT_ENCODE_H__
#define __EVIDYON_PACKETS_CLIENT_ENCODE_H__

struct _ENetPacket;
typedef struct _ENetPacket ENetPacket;

#include "../shared/actorid.h"
#include "../shared/evidyon_actor.h"
#include "../shared/evidyon_enchantments.h"
#include "actorset.h"

namespace Evidyon {

struct ItemForTrade;
struct SpellAvailabilityMask;
enum ActorState;
enum ActorAction;
struct AvatarAttributes;
enum EquipmentSlot;

namespace Network {

namespace Packet {
enum RequestActionResult;

namespace Client {
namespace Encode {

ENetPacket* tellClientVersionInfo(unsigned long gamefileVersion, unsigned long clientVersion);

ENetPacket* createAccount(const char* password);
ENetPacket* accessAccount(unsigned int id, const char* password);
ENetPacket* logoutAccount();
ENetPacket* createCharacter(const char* name, bool male, int chRace, int chClass, int chStr, int chAgi, int chCon, int chInt, int chWis);
ENetPacket* deleteCharacter(unsigned int id, const char* password);
ENetPacket* askCharacterName(unsigned int id);
ENetPacket* askAccountInfo();
ENetPacket* askCharacterMapLocation(unsigned int id);

ENetPacket* enterWorld(unsigned int id);
ENetPacket* leaveWorld();
ENetPacket* requestMove(float x, float y, ActorID targetActorID);
ENetPacket* requestMoveStreaming(float x, float y);
//ENetPacket* requestStopActionTurnBlock(float angle, bool stop_action, bool now_blocking);
//ENetPacket* requestStopDefensiveToggle(bool now_blocking);

//ENetPacket* requestMoveAttack(float x, float y, bool change_attack_target, int attack_target_actor_id, bool defending);
//ENetPacket* requestStopDefend(bool enter, bool full_defense);


ENetPacket* changeEquippedState(unsigned char* inventoryIndices, size_t numberOfIndices);
ENetPacket* dropInventoryItems(unsigned char* inventoryIndices, size_t numberOfIndices);

ENetPacket* askActorDescription(int actorID);

/**
 * Requests the avatar's stats from the server; if abilityToAddPoint is between 0 and 5, the
 * client would like to add an ability point to that stat.
 */
ENetPacket* askMyAvatarStats(int abilityToAddPoint);

ENetPacket* changeItemsToTrade(unsigned char* trade_item_indices, unsigned int number_of_items);
ENetPacket* acceptTrade(int trade_key);

ENetPacket* listItemsForSale(unsigned char* inventory_indices, unsigned int* prices, unsigned int number_of_items);
ENetPacket* getItemsForSale(unsigned int minimum_listing_id);
ENetPacket* buyItem(unsigned int listing_id);
ENetPacket* getSellerStatus();
ENetPacket* withdrawSellerItems(unsigned int* listing_ids, unsigned int number_of_items);

ENetPacket* geosid_sacrificeItems(unsigned char* inventory_indices, unsigned char number_of_items);
ENetPacket* useItemAbility(unsigned char inventory_index, EquipmentSlot equipment_slot);



ENetPacket* askActorFullDescriptions(const ActorSet* actor_set);
ENetPacket* castSpell(const unsigned int spell_ids[3], ActorID target_actor_id, float target_x, float target_y);
ENetPacket* useItem(const unsigned char inventory_index, ActorID target_actor_id, float target_x, float target_y);
ENetPacket* requestSpeak(const char* speech);

ENetPacket* askActorName(ActorID actor_id);

ENetPacket* storageOpen(ActorID storage_npc_actor_id);
ENetPacket* storageWithdraw(ActorID storage_npc_actor_id, int storage_index);
ENetPacket* storageDeposit(ActorID storage_npc_actor_id, int inventory_index);

ENetPacket* geosidSacrifice(unsigned char inventory_index);


}
}
}
}
}

#endif