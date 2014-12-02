#ifndef __DECODEPACKETS_H__
#define __DECODEPACKETS_H__

struct _ENetPacket;
typedef struct _ENetPacket ENetPacket;

#include "../shared/actorid.h"

namespace Evidyon {
namespace Network {
namespace Packet {
namespace Server {
namespace Decode {

bool tellClientVersionInfo(ENetPacket* packet, unsigned long* gamefileVersion, unsigned long* clientVersion);
bool createAccount(ENetPacket* packet, char* passwordBuffer, size_t bufferSize);
bool accessAccount(ENetPacket* packet, unsigned int* id, char* passwordBuffer, size_t bufferSize);
bool logoutAccount(ENetPacket* packet);
bool createCharacter(ENetPacket* packet, char* nameBuffer, size_t bufferSize, bool* male, int* chRace, int* chClass, int* chStr, int* chAgi, int* chCon, int* chInt, int* chWis);
bool deleteCharacter(ENetPacket* packet, unsigned int* id, char* passwordBuffer, size_t bufferSize);
bool askCharacterName(ENetPacket* packet, unsigned int* id);
bool askAccountInfo(ENetPacket* packet);
bool askCharacterMapLocation(ENetPacket* packet, unsigned int* id);

bool enterWorld(ENetPacket* packet, unsigned int* id);
bool leaveWorld(ENetPacket* packet);
bool requestMove(ENetPacket* packet, float* x, float* y, ActorID* targetActorID);
bool requestMoveStreaming(ENetPacket* packet, float* x, float* y);
//bool requestStopActionTurnBlock(ENetPacket* packet, float* angle, bool* stop_action, bool* now_blocking);
bool requestSpeak(ENetPacket* packet, char* speechBuffer, size_t bufferSize);

//bool requestMoveAttack(ENetPacket* packet, float* x, float* y, bool* change_attack_target, int* attack_target_actor_id, bool* defending);
//bool requestStopDefend(ENetPacket* packet, bool* enter, bool* full_defense);

bool requestAbility(ENetPacket* packet, unsigned int abilityIndices[3], float targetXs[3], float targetYs[3], int targetActors[3]);

bool changeEquippedState(ENetPacket* packet, unsigned char* inventoryIndices, size_t bufferSize, size_t* numberOfIndices);
bool dropInventoryItems(ENetPacket* packet, unsigned char* inventoryIndices, size_t bufferSize, size_t* numberOfIndices);

bool askActorDescription(ENetPacket* packet, int* actorID);

bool askMyAvatarStats(ENetPacket* packet, int* abilityToAddPoint);
//
//bool changeItemsToTrade(ENetPacket* packet, unsigned char* trade_item_indices, unsigned int* number_of_items);
//bool acceptTrade(ENetPacket* packet, int* trade_key);
//
//bool listItemsForSale(ENetPacket* packet, unsigned char* inventory_indices, unsigned int* prices, size_t buffer_size, unsigned int* number_of_indices);
//bool getItemsForSale(ENetPacket* packet, unsigned int* minimum_listing_id);
//bool buyItem(ENetPacket* packet, unsigned int* listing_id);
//bool getSellerStatus(ENetPacket* packet);
//bool withdrawSellerItems(ENetPacket* packet, unsigned int* listing_ids, size_t buffer_size, unsigned int* number_of_items);

bool askActorFullDescriptions(ENetPacket* packet, ActorID* actor_ids, size_t* number_of_actors);
bool castSpell(ENetPacket* packet, unsigned int spell_ids[3], ActorID* target_actor_id, float* target_x, float* target_y);
bool useItem(ENetPacket* packet, unsigned int* inventory_index, ActorID* target_actor_id, float* target_x, float* target_y);

bool askActorName(ENetPacket* packet, ActorID* actor_id);

bool storageOpen(ENetPacket* packet, ActorID* storage_npc_actor_id);
bool storageWithdraw(ENetPacket* packet, ActorID* storage_npc_actor_id, int* storage_index);
bool storageDeposit(ENetPacket* packet, ActorID* storage_npc_actor_id, int* inventory_index);

bool geosidSacrifice(ENetPacket* packet, unsigned char* inventory_index);
}
}
}
}
}


#endif