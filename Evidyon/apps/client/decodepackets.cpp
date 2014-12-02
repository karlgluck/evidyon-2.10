#include "../shared/client-server/packets.h"
#include "../shared/evidyon_avatar.h"
#include "../shared/evidyon_actor.h"
#include "../shared/evidyon_enchantments.h"
#include "../shared/evidyon_accounts.h"
#include "../shared/evidyon_avatarinventory.h"
#include "../shared/client-server/actorsync.h"
#include "decodepackets.h"

#include "../common/templateserializer.h"
#include "../shared/client-server/fastsync.h"

#include "stdafx.h" // for ItemForTrade only
#include <dc/debug>

namespace Evidyon {
namespace Network {
namespace Packet {
namespace Client {
namespace Decode {


//------------------------------------------------------------------------------------------------
// Name:  tellServerVersionUpdateInfo
// Desc:  
//------------------------------------------------------------------------------------------------
bool tellServerVersionUpdateInfo(ENetPacket* packet, bool* versionIsValid, bool* needsNewGamefile, bool* needsNewClient, std::string* updateText) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !versionIsValid || !needsNewGamefile || !needsNewClient || !updateText)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLSERVERVERSIONUPDATEINFO)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<bool>(buffer, endOfBuffer, versionIsValid);
  buffer = decodeStructure<bool>(buffer, endOfBuffer, needsNewGamefile);
  buffer = decodeStructure<bool>(buffer, endOfBuffer, needsNewClient);
  buffer = decodeString(buffer, endOfBuffer, updateText);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellServerVersionUpdateInfo message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("tellServerVersionUpdateInfo message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  createAccountSucceeded
// Desc:  
//------------------------------------------------------------------------------------------------
bool createAccountSucceeded(ENetPacket* packet, unsigned int* id) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !id)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_CREATEACCOUNT_SUCCEEDED)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode createAccountSucceeded message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("createAccountSucceeded message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  logoutAccountConfirmed
// Desc:  
//------------------------------------------------------------------------------------------------
bool logoutAccountConfirmed(ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_LOGOUTACCOUNT_CONFIRMED)
    return false;

  // Provide feedback about processing
  if (buffer != endOfBuffer) DEBUG_WARNING("logoutAccount message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  tellAccountInfo
// Desc:  
//------------------------------------------------------------------------------------------------
bool tellAccountInfo(ENetPacket* packet, float* daysLeft, unsigned int* accountCharacterIDs, size_t* numberOfCharacters) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !daysLeft || !accountCharacterIDs)("invalid parameter"))
    return false;

  // Make sure to empty the output variable to prevent problems
  *numberOfCharacters = 0;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLACCOUNTINFO)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<float>(buffer, endOfBuffer, daysLeft);

  // Get the number of character IDs in the following array
  size_t characterIDArraySize;
  buffer = decodeStructure<size_t>(buffer, endOfBuffer, &characterIDArraySize);

  // Validate the number of characters
  if (APP_ERROR(characterIDArraySize > Evidyon::MAX_AVATARS_PER_ACCOUNT)
      ("Invalid number of characters in account array from tellAccountInfo: %lu", characterIDArraySize))
    return false;

  // Assign the number of characters
  *numberOfCharacters = characterIDArraySize;

  // Read each of the characters
  for (size_t i = 0; (i < characterIDArraySize) && (buffer != 0); ++i)
  {
    // Read the ID into the output array
    buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, &accountCharacterIDs[i]);
  }

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellAccountInfo message"))
    return false;

  // Check to make sure we exactly used up the buffer
  if (buffer != endOfBuffer) DEBUG_WARNING("tellAccountInfo message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  requestActionResult
// Desc:  
//------------------------------------------------------------------------------------------------
bool requestActionResult(ENetPacket* packet, Evidyon::Network::Packet::RequestActionResult* result) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !result)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_REQUESTACTIONRESULT)
    return false;

  // Get the result
  buffer = decodeStructure<RequestActionResult>(buffer, endOfBuffer, result);

  // Provide feedback about processing
  if (buffer != endOfBuffer) DEBUG_WARNING("requestActionResult message contained extra data");

  // Success
  return true;
}


//------------------------------------------------------------------------------------------------
// Name:  tellCharacterName
// Desc:  
//------------------------------------------------------------------------------------------------
bool tellCharacterName(ENetPacket* packet, unsigned int* id,
  char* characterNameBuffer, size_t bufferSize, float* world_x,
  float* world_y, unsigned int* actor_type,
  size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT]) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !id || !characterNameBuffer || !world_x || !world_y || !actor_type || !visible_equipment)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLCHARACTERNAME)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);
  buffer = decodeString(buffer, endOfBuffer, characterNameBuffer, bufferSize);
  buffer = decodeStructure<float>(buffer, endOfBuffer, world_x);
  buffer = decodeStructure<float>(buffer, endOfBuffer, world_y);
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, actor_type);
  for (unsigned int i = 0; i < ACTORATTACHMENTPOINT_COUNT; ++i)
    buffer = decodeStructure<size_t>(buffer, endOfBuffer, &visible_equipment[i]);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellCharacterName message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("tellCharacterName message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  createCharacterSucceeded
// Desc:  
//------------------------------------------------------------------------------------------------
bool createCharacterSucceeded(ENetPacket* packet, unsigned int* id) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !id)("invalid parameter to createCharacterSucceeded"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_CREATECHARACTER_SUCCEEDED)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode createCharacterSucceeded message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("createCharacterSucceeded message contained extra data");

  // Success
  return true;
}

//------------------------------------------------------------------------------------------------
// Name:  tellCharacterMapLocation
// Desc:  
//------------------------------------------------------------------------------------------------
bool tellCharacterMapLocation(ENetPacket* packet, unsigned int* id, int* mapX, int* mapY) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !id)("invalid parameter to tellCharacterMapLocation"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLCHARACTERMAPLOCATION)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);
  buffer = decodeStructure<int>(buffer, endOfBuffer, mapX);
  buffer = decodeStructure<int>(buffer, endOfBuffer, mapY);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellCharacterMapLocation message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("tellCharacterMapLocation message contained extra data");

  // Success
  return true;
}

//------------------------------------------------------------------------------------------------
// Name:  enterWorldSucceeded
// Desc:  
//------------------------------------------------------------------------------------------------
bool enterWorldSucceeded(ENetPacket* packet,
                         ActorID* actorID,
                         unsigned long* time_of_day,
                         float* yourX,
                         float* yourY,
                         int* map_index,
                         bool* male,
                         int* raceIndex,
                         int* classIndex,
                         int* currentHP,
                         int* maxHP,
                         int* currentMP,
                         int* maxMP,
                         unsigned int* money,
                         unsigned int* geonite,
                         SpellAvailabilityMask* spellAvailabilityMask) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !actorID || !spellAvailabilityMask)("invalid parameter to enterWorldSucceeded"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_ENTERWORLD_SUCCEEDED)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<ActorID>(buffer, endOfBuffer, actorID);
  buffer = decodeStructure<unsigned long>(buffer, endOfBuffer, time_of_day);
  buffer = decodeStructure<float>(buffer, endOfBuffer, yourX);
  buffer = decodeStructure<float>(buffer, endOfBuffer, yourY);
  buffer = decodeStructure<int>(buffer, endOfBuffer, map_index);
  buffer = decodeStructure<bool>(buffer, endOfBuffer, male);
  buffer = decodeStructure<int>(buffer, endOfBuffer, raceIndex);
  buffer = decodeStructure<int>(buffer, endOfBuffer, classIndex);
  buffer = decodeStructure<int>(buffer, endOfBuffer, currentHP);
  buffer = decodeStructure<int>(buffer, endOfBuffer, maxHP);
  buffer = decodeStructure<int>(buffer, endOfBuffer, currentMP);
  buffer = decodeStructure<int>(buffer, endOfBuffer, maxMP);
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, money);
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, geonite);
  buffer = decodeStructure<SpellAvailabilityMask>(buffer, endOfBuffer, spellAvailabilityMask);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode enterWorldSucceeded message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("enterWorldSucceeded message contained extra data");

  // Success
  return true;
}

//------------------------------------------------------------------------------------------------
// Name:  leaveWorldConfirmed
// Desc:  
//------------------------------------------------------------------------------------------------
bool leaveWorldConfirmed(ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  return decodeMessageOnlyPacket(packet, NETMSG_LEAVEWORLD_CONFIRMED);
}


//------------------------------------------------------------------------------------------------
// Name:  tellActorActivity
// Desc:  
//------------------------------------------------------------------------------------------------
bool tellActorActivity(ENetPacket* packet, int* actorID, float* x, float* y, float* angle, float* speed, float* distanceToMove, ActorState* currentState, ActorAction* action, float* actionSpeed) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !actorID || !x || !y || !angle || !speed || !distanceToMove || !currentState || !action || !actionSpeed)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLACTORACTIVITY)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<int>(buffer, endOfBuffer, actorID);
  buffer = decodeStructure<float>(buffer, endOfBuffer, x);
  buffer = decodeStructure<float>(buffer, endOfBuffer, y);
  buffer = decodeStructure<float>(buffer, endOfBuffer, angle);
  buffer = decodeStructure<float>(buffer, endOfBuffer, speed);
  buffer = decodeStructure<float>(buffer, endOfBuffer, distanceToMove);
  buffer = decodeStructure<ActorState>(buffer, endOfBuffer, currentState);
  buffer = decodeStructure<ActorAction>(buffer, endOfBuffer, action);
  buffer = decodeStructure<float>(buffer, endOfBuffer, actionSpeed);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellActorActivity message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("tellActorActivity message contained extra data");

  // Success
  return true;
}

//------------------------------------------------------------------------------------------------
// Name:  destroyActor
// Desc:  
//------------------------------------------------------------------------------------------------
bool destroyActor(ENetPacket* packet, int* actorID) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !actorID)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_DESTROYACTOR)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<int>(buffer, endOfBuffer, actorID);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode destroyActor message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("destroyActor message contained extra data");

  // Success
  return true;
}

//------------------------------------------------------------------------------------------------
// Name:  actorSpeaks
// Desc:  
//------------------------------------------------------------------------------------------------
bool actorSpeaks(ENetPacket* packet,
                    int* actorID,
                    char* source_name_buffer,
                    size_t source_name_buffer_size,
                    char* speechBuffer,
                    size_t bufferSize) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !actorID || !source_name_buffer ||
           !source_name_buffer_size || !speechBuffer || !bufferSize)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_ACTORSPEAKS)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<int>(buffer, endOfBuffer, actorID);
  if (*actorID == -1) buffer = decodeString(buffer, endOfBuffer, source_name_buffer, source_name_buffer_size);
  buffer = decodeString(buffer, endOfBuffer, speechBuffer, bufferSize);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode actorSpeaks message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("actorSpeaks message contained extra data");

  // Success
  return true;
}



////------------------------------------------------------------------------------------------------
//// Name:  updateYourCurrentHPMPEXP
//// Desc:  
////------------------------------------------------------------------------------------------------
//bool updateYourCurrentHPMPEXP(ENetPacket* packet, float* currentHPPercent, float* currentMPPercent, float* currentEXPPercent) {
//  using namespace ::Evidyon::Network::Packet;
//  // Make sure the packet is valid
//  if (APP_ERROR(!packet || !currentHPPercent || !currentMPPercent || !currentEXPPercent)("invalid parameter"))
//    return false;
//
//  // Obtain pointers to packet data
//  const char* buffer = (const char*)packet->data;
//  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;
//
//  // Make sure that the message type is correct
//  Message message;
//  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
//    message != NETMSG_UPDATEYOURCURRENTHPMPEXP)
//    return false;
//
//  // Obtain the data from the packet
//  buffer = decodeStructure<float>(buffer, endOfBuffer, currentHPPercent);
//  buffer = decodeStructure<float>(buffer, endOfBuffer, currentMPPercent);
//  buffer = decodeStructure<float>(buffer, endOfBuffer, currentEXPPercent);
//
//  // Provide feedback about processing
//  if (APP_ERROR(!buffer)("Failed to decode updateYourCurrentHPMP message")) return false;
//  if (buffer != endOfBuffer) DEBUG_WARNING("updateYourCurrentHPMP message contained extra data");
//
//  // Success
//  return true;
//}





//------------------------------------------------------------------------------------------------
// Name:  createProjectile
// Desc:  
//------------------------------------------------------------------------------------------------
bool createProjectile(ENetPacket* packet,
             int* projectile_id,
             int* projectile_type,
             float* x,
             float* y,
             float* dx,
             float* dy) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !projectile_id || !x || !y || !dx || !dy)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_CREATEPROJECTILE)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<int>(buffer, endOfBuffer, projectile_id);
  buffer = decodeStructure<int>(buffer, endOfBuffer, projectile_type);
  buffer = decodeStructure<float>(buffer, endOfBuffer, x);
  buffer = decodeStructure<float>(buffer, endOfBuffer, y);
  buffer = decodeStructure<float>(buffer, endOfBuffer, dx);
  buffer = decodeStructure<float>(buffer, endOfBuffer, dy);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode createProjectile message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("createProjectile message contained extra data");

  // Success
  return true;
}


//------------------------------------------------------------------------------------------------
// Name:  destroyProjectile
// Desc:  
//------------------------------------------------------------------------------------------------
bool destroyProjectile(ENetPacket* packet, int* projectile_id) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !projectile_id)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_DESTROYPROJECTILE)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<int>(buffer, endOfBuffer, projectile_id);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode destroyProjectile message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("destroyProjectile message contained extra data");

  // Success
  return true;
}



////------------------------------------------------------------------------------------------------
//// Name:  updateInventory
//// Desc:  
////------------------------------------------------------------------------------------------------
//bool updateInventory(ENetPacket* packet,
//                       ClientAvatarInventoryItem fullClientInventory[AVATAR_INVENTORY_SIZE],
//                       int* highest_inventory_update_index,
//                       int inventory_update_indices[AVATAR_INVENTORY_SIZE]) {
//  using namespace ::Evidyon::Network::Packet;
//  // Make sure the packet is valid
//  if (APP_ERROR(!packet || !fullClientInventory)("invalid parameter"))
//    return false;
//
//  // Obtain pointers to packet data
//  const char* buffer = (const char*)packet->data;
//  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;
//
//  // Make sure that the message type is correct
//  Message message;
//  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
//    message != NETMSG_UPDATEINVENTORY)
//    return false;
//
//  // Obtain the data from the packet
//  bool isFullInventoryUpdate;
//  buffer = decodeStructure<bool>(buffer, endOfBuffer, &isFullInventoryUpdate);
//  unsigned char inventoryItems;
//  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &inventoryItems);
//
//  // Make sure we have enough inventory space
//  if (APP_ERROR(AVATAR_INVENTORY_SIZE < inventoryItems)("updateInventory contained an invalid number of items")) return false;
//
//  // Reset the inventory if this message completely changes the inventory
//  if (isFullInventoryUpdate)
//  {
//    *highest_inventory_update_index = 0;
//    for (unsigned i = 0; i < AVATAR_INVENTORY_SIZE; ++i)
//    {
//      fullClientInventory[i].inventory_index = 0xFF;
//      fullClientInventory[i].quantity = 0;
//      fullClientInventory[i].type = 0xFFFFFFFF;
//      inventory_update_indices[i] = 0;
//    }
//  }
//
//  int new_inventory_update_index = (*highest_inventory_update_index) + 1;
//  *highest_inventory_update_index = new_inventory_update_index;
//
//  // Update each element
//  for (unsigned i = 0; (i < inventoryItems) && buffer; ++i)
//  {
//    ClientAvatarInventoryItem item;
//    buffer = decodeStructure<ClientAvatarInventoryItem>(buffer, endOfBuffer, &item);
//    memcpy(&fullClientInventory[item.inventory_index], &item, sizeof(item));
//    inventory_update_indices[item.inventory_index] = new_inventory_update_index;
//  }
//
//  // Provide feedback about processing
//  if (APP_ERROR(!buffer)("Failed to decode updateInventory message")) return false;
//  if (buffer != endOfBuffer) DEBUG_WARNING("updateInventory message contained extra data");
//
//  // Success
//  return true;
//}


//
////------------------------------------------------------------------------------------------------
//// Name:  removeInventoryItems
//// Desc:  
////------------------------------------------------------------------------------------------------
//bool removeInventoryItems(ENetPacket* packet, ClientAvatarInventoryItem fullClientInventory[AVATAR_INVENTORY_SIZE]) {
//  using namespace ::Evidyon::Network::Packet;
//  // Make sure the packet is valid
//  if (APP_ERROR(!packet || !fullClientInventory)("invalid parameter"))
//    return false;
//
//  // Obtain pointers to packet data
//  const char* buffer = (const char*)packet->data;
//  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;
//
//  // Make sure that the message type is correct
//  Message message;
//  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
//    message != NETMSG_REMOVEINVENTORYITEMS)
//    return false;
//
//  // Obtain the data from the packet
//  size_t numberOfIndices = 0xFFFFFFFF;
//  buffer = decodeStructure<size_t>(buffer, endOfBuffer, &numberOfIndices);
//  if (APP_ERROR(numberOfIndices > 127)("Invalid number of items")) return false;
//  for (size_t i = 0; i < numberOfIndices; ++i)
//  {
//    unsigned char inventoryIndex;
//    buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &inventoryIndex);
//
//    // Make sure the index is valid
//    if (!buffer) break;
//    if (APP_WARNING(inventoryIndex > 127)("Invalid inventory index in removeInventoryItems")) continue;
//
//    // Get rid of this item
//    fullClientInventory[inventoryIndex].inventory_index = 0xFF;
//    fullClientInventory[inventoryIndex].quantity = 0;
//    fullClientInventory[inventoryIndex].type = 0xFFFFFFFF;
//  }
//
//
//  // Provide feedback about processing
//  if (APP_ERROR(!buffer)("Failed to decode removeInventoryItems message")) return false;
//  if (buffer != endOfBuffer) DEBUG_WARNING("removeInventoryItems message contained extra data");
//
//  // Success
//  return true;
//}

////------------------------------------------------------------------------------------------------
//// Name:  tellGroundItems
//// Desc:  
////------------------------------------------------------------------------------------------------
//bool tellGroundItems(ENetPacket* packet, int* x, int* y, size_t* itemTypes, size_t bufferSize, size_t* numberOfItems) {
//  using namespace ::Evidyon::Network::Packet;
//  // Make sure the packet is valid
//  if (APP_ERROR(!packet || !itemTypes || !bufferSize || !numberOfItems)("invalid parameter"))
//    return false;
//
//  // Obtain pointers to packet data
//  const char* buffer = (const char*)packet->data;
//  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;
//
//  // Make sure that the message type is correct
//  Message message;
//  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
//    message != NETMSG_TELLGROUNDITEMS)
//    return false;
//
//  // Read the coordinates
//  buffer = decodeStructure<int>(buffer, endOfBuffer, x);
//  buffer = decodeStructure<int>(buffer, endOfBuffer, y);
//
//  // Obtain the data from the packet
//  size_t itemsInPacket = 0xFFFFFFFF;
//  buffer = decodeStructure<size_t>(buffer, endOfBuffer, &itemsInPacket);
//  if (APP_ERROR(itemsInPacket > 127 || itemsInPacket > bufferSize)("Invalid number of items")) return false;
//  for (size_t i = 0; i < itemsInPacket; ++i)
//  {
//    buffer = decodeStructure<size_t>(buffer, endOfBuffer, &itemTypes[i]);
//    //DEBUG_INFO("Receiving item %lu", itemTypes[i]);
//  }
//
//  // Save the output items
//  *numberOfItems = itemsInPacket;
//
//  // Provide feedback about processing
//  if (APP_ERROR(!buffer)("Failed to decode tellGroundItems message")) return false;
//  if (buffer != endOfBuffer) DEBUG_WARNING("tellGroundItems message contained extra data");
//
//  // Success
//  return true;
//}


//
////------------------------------------------------------------------------------------------------
//// Name:  tellActorDescription
//// Desc:  
////------------------------------------------------------------------------------------------------
//bool tellActorDescription(
//  ENetPacket* packet, std::string* actorName,
//  int* actorID, size_t* actorType, ActorState* currentState,
//  size_t visibleEquipment[ACTORATTACHMENTPOINT_COUNT],
//  size_t visibleEnchantments[NUMBER_OF_ENCHANTMENT_SLOTS]) {
//  using namespace ::Evidyon::Network::Packet;
//  // Make sure the packet is valid
//  if (APP_ERROR(!packet || !actorID || !actorType || !currentState || !visibleEquipment || !visibleEnchantments)("invalid parameter"))
//    return false;
//
//  // Obtain pointers to packet data
//  const char* buffer = (const char*)packet->data;
//  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;
//
//  // Make sure that the message type is correct
//  Message message;
//  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
//    message != NETMSG_TELLACTORDESCRIPTION)
//    return false;
//
//  // Obtain the data from the packet
//  buffer = decodeString(buffer, endOfBuffer, actorName);
//  buffer = decodeStructure<int>(buffer, endOfBuffer, actorID);
//  buffer = decodeStructure<size_t>(buffer, endOfBuffer, actorType);
//  buffer = decodeStructure<ActorState>(buffer, endOfBuffer, currentState);
//  for (size_t i = 0; i < ACTORATTACHMENTPOINT_COUNT; ++i) buffer = decodeStructure<size_t>(buffer, endOfBuffer, &visibleEquipment[i]);
//  for (size_t i = 0; i < NUMBER_OF_ENCHANTMENT_SLOTS; ++i)   buffer = decodeStructure<size_t>(buffer, endOfBuffer, &visibleEnchantments[i]);
//
//  // Provide feedback about processing
//  if (APP_ERROR(!buffer)("Failed to decode tellActorDescription message")) return false;
//  if (buffer != endOfBuffer) DEBUG_WARNING("tellActorDescription message contained extra data");
//
//  // Success
//  return true;
//}


//
////------------------------------------------------------------------------------------------------
//// Name:  updateYourActivePartition
//// Desc:  
////------------------------------------------------------------------------------------------------
//bool updateYourActivePartition(ENetPacket* packet, short* px, short* py) {
//  using namespace ::Evidyon::Network::Packet;
//  // Make sure the packet is valid
//  if (APP_ERROR(!packet || !px || !py)("invalid parameter"))
//    return false;
//
//  // Obtain pointers to packet data
//  const char* buffer = (const char*)packet->data;
//  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;
//
//  // Make sure that the message type is correct
//  Message message;
//  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
//    message != NETMSG_UPDATEYOURACTIVEPARTITION)
//    return false;
//
//  // Obtain the data from the packet
//  buffer = decodeStructure<short>(buffer, endOfBuffer, px);
//  buffer = decodeStructure<short>(buffer, endOfBuffer, py);
//
//  // Provide feedback about processing
//  if (APP_ERROR(!buffer)("Failed to decode updateYourActivePartition message")) return false;
//  if (buffer != endOfBuffer) DEBUG_WARNING("updateYourActivePartition message contained extra data");
//
//  // Success
//  return true;
//}



//------------------------------------------------------------------------------------------------
// Name:  updateYourEffectAvailabilityMask
// Desc:  
//------------------------------------------------------------------------------------------------
bool updateYourEffectAvailabilityMask(ENetPacket* packet, SpellAvailabilityMask* mask) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !mask)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_UPDATEYOUREFFECTAVAILABILITYMASK)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<SpellAvailabilityMask>(buffer, endOfBuffer, mask);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode updateYourEffectAvailabilityMask message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("updateYourEffectAvailabilityMask message contained extra data");

  // Success
  return true;
}


//------------------------------------------------------------------------------------------------
// Name:  tellYourAvatarStats
// Desc:  
//------------------------------------------------------------------------------------------------
bool tellYourAvatarStats(ENetPacket* packet, unsigned char* level, unsigned char abilities[5], unsigned char* availableAbilityPoints,
                         int* hp, int* hpMax, int* mp, int* mpMax, int* exp, int* nextLevelExp, int* defense,
                         AvatarRace* avatar_race, AvatarClass* avatar_class) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !level || !abilities || !availableAbilityPoints || !hp || !hpMax || !mp || !mpMax ||!exp || !nextLevelExp || !defense)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLYOURAVATARSTATS)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, level);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &abilities[0]);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &abilities[1]);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &abilities[2]);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &abilities[3]);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &abilities[4]);
  buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, availableAbilityPoints);
  buffer = decodePackedStructure<int, unsigned short>(buffer, endOfBuffer, hp      );
  buffer = decodePackedStructure<int, unsigned short>(buffer, endOfBuffer, hpMax     );
  buffer = decodePackedStructure<int, unsigned short>(buffer, endOfBuffer, mp      );
  buffer = decodePackedStructure<int, unsigned short>(buffer, endOfBuffer, mpMax     );
  buffer = decodeStructure<int>(buffer, endOfBuffer, exp      );
  buffer = decodeStructure<int>(buffer, endOfBuffer, nextLevelExp);
  buffer = decodePackedStructure<int, unsigned short>(buffer, endOfBuffer, defense);
  buffer = decodePackedStructure<AvatarRace, unsigned char>(buffer, endOfBuffer, avatar_race);
  buffer = decodePackedStructure<AvatarClass, unsigned char>(buffer, endOfBuffer, avatar_class);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode tellYourAvatarStats message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("tellYourAvatarStats message contained extra data");

  // Success
  return true;
}


////------------------------------------------------------------------------------------------------
//bool tellCurrentTrade(ENetPacket* packet,
//                      int* trade_key,
//                      unsigned char* your_trade_item_indices,
//                      unsigned int* number_of_items,
//                      TradeItem other_trade_items[MAX_ITEMS_PER_TRADE], 
//                      unsigned int* other_number_of_items) {
//  using namespace ::Evidyon::Network::Packet;
//  
//
//  CONFIRM(trade_key && your_trade_item_indices && number_of_items && other_trade_items && other_number_of_items);
//
//  const char* buffer;
//  const char* buffer_end;
//  if (!openPacket(packet, NETMSG_TELLCURRENTTRADE, &buffer, &buffer_end))
//    return false;
//
//  // Decode the trade key
//  buffer = decodeStructure<int>(buffer, buffer_end, trade_key);
//
//  {   // Decode this character's item indices
//    unsigned char unsigned_char;
//    buffer = decodeStructure<unsigned char>(buffer, buffer_end, &unsigned_char);
//    unsigned int number_of_items_local = (unsigned int)unsigned_char;
//    CONFIRM(number_of_items_local < AVATAR_INVENTORY_SIZE) else return false;
//    for (unsigned int i = 0; buffer && i < number_of_items_local; ++i) {
//      buffer = decodeStructure<unsigned char>(buffer, buffer_end, &your_trade_item_indices[i]);
//    }
//    *number_of_items = number_of_items_local;
//  }
//
//  {   // Decode the trade partner's item descriptions
//    unsigned char unsigned_char;
//    buffer = decodeStructure<unsigned char>(buffer, buffer_end, &unsigned_char);
//    unsigned int other_number_of_items_local = (unsigned int)unsigned_char;
//    CONFIRM(other_number_of_items_local < MAX_ITEMS_PER_TRADE) else return false;
//    for (unsigned int i = 0; buffer && i < other_number_of_items_local; ++i) {
//      buffer = decodeStructure<TradeItem>(buffer, buffer_end, &other_trade_items[i]);
//    }
//    *other_number_of_items = other_number_of_items_local;
//    
//  }
//
//  // Success
//  return true;
//}


//
////------------------------------------------------------------------------------------------------
//bool finishedTrading(ENetPacket* packet) {
//  using namespace ::Evidyon::Network::Packet;
//  return decodeMessageOnlyPacket(packet, NETMSG_FINISHEDTRADING);
//}
//
//
////------------------------------------------------------------------------------------------------
//bool tellYourMoney(
//  ENetPacket* packet,
//  unsigned int* money) {
//  using namespace ::Evidyon::Network::Packet;
//  
//  const char* buffer, *buffer_end;
//  if (!openPacket(packet, NETMSG_TELLYOURMONEY, &buffer, &buffer_end))
//  return false;
//
//  buffer = decodeStructure<unsigned int>(buffer, buffer_end, money);
//
//  return buffer != NULL;
//}

//
////------------------------------------------------------------------------------------------------
//bool bazaar_tellItemsForSale(
//  ENetPacket* packet,
//  Evidyon::ItemForTrade* items_buffer,
//  unsigned int buffer_size,
//  unsigned int* number_of_items) {
//  using namespace ::Evidyon::Network::Packet;
//  
//  const char* buffer, *buffer_end;
//  if (!openPacket(packet, NETMSG_BAZAAR_TELLITEMSFORSALE, &buffer, &buffer_end))
//  return false;
//
//  unsigned char char_number_of_items;
//  buffer = decodeArray<Evidyon::ItemForTrade,unsigned char>(buffer, buffer_end,
//                           items_buffer,
//                           buffer_size,
//                           &char_number_of_items);
//  *number_of_items = char_number_of_items;
//
//  return buffer != NULL;
//}
//
////------------------------------------------------------------------------------------------------
//bool bazaar_tellSellerStatus(
//  ENetPacket* packet,
//  ItemForTrade* items_buffer,
//  unsigned int buffer_size,
//  unsigned int* number_of_items) {
//  using namespace ::Evidyon::Network::Packet;
//  
//  const char* buffer, *buffer_end;
//  if (!openPacket(packet, NETMSG_BAZAAR_TELLSELLERSTATUS, &buffer, &buffer_end))
//  return false;
//
//  unsigned char char_number_of_items;
//  buffer = decodeArray<ItemForTrade,unsigned char>(buffer, buffer_end,
//                           items_buffer,
//                           buffer_size,
//                           &char_number_of_items);
//  *number_of_items = char_number_of_items;
//
//  return buffer != NULL;
//}
//
////------------------------------------------------------------------------------------------------
//bool bazaar_removeItemListings(
//  ENetPacket* packet,
//  unsigned int* item_listings_buffer,
//  unsigned int buffer_size,
//  unsigned int* number_of_items) {
//  using namespace ::Evidyon::Network::Packet;
//  
//  const char* buffer, *buffer_end;
//  if (!openPacket(packet, NETMSG_BAZAAR_REMOVEITEMLISTINGS, &buffer, &buffer_end))
//  return false;
//
//  unsigned char char_number_of_items;
//  buffer = decodeArray<unsigned int,unsigned char>(buffer, buffer_end,
//                           item_listings_buffer,
//                           buffer_size,
//                           &char_number_of_items);
//  *number_of_items = char_number_of_items;
//
//  return buffer != NULL;
//}


//-----------------------------------------------------------------------------
bool changeMap(ENetPacket* packet, int* map_id) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_CHANGEMAP, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<int>(buffer, buffer_end, map_id);

  return buffer != NULL;
}



//-----------------------------------------------------------------------------
bool eraseActorFromWorld(ENetPacket* packet, int* actor_id) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_ERASEACTORFROMWORLD, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<int>(buffer, buffer_end, actor_id);

  return buffer != NULL;
}



//-----------------------------------------------------------------------------
bool slowSync(ENetPacket* packet,
        float* time_since_last_sync,
        unsigned int* actors,
        ActorSyncData* updates_buffer,
        unsigned int buffer_size) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_SLOWSYNC, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<float>(buffer, buffer_end, time_since_last_sync);
  unsigned char byte_number_of_actors;
  buffer = decodeArray<ActorSyncData,unsigned char>(buffer, buffer_end,
                          updates_buffer,
                          buffer_size,
                           &byte_number_of_actors);
  *actors = byte_number_of_actors;

  return buffer != NULL;
}

//-----------------------------------------------------------------------------
bool fastSync(ENetPacket* packet,
        Evidyon::Network::Packet::FastSyncPacket* data) {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_FASTSYNC, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<FastSyncPacket>(buffer, buffer_end, data);

  return buffer != NULL;
}


//-----------------------------------------------------------------------------
bool tellItemsOnMapLocation(ENetPacket* packet,
                            int* world_x,
                            int* world_y,
                            unsigned short* item_types,
                            int* number_of_item_types) {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_TELLITEMSONMAPLOCATION, &buffer, &buffer_end))
  return false;

  unsigned long packed_location;
  buffer = decodeStructure<unsigned long>(buffer, buffer_end, &packed_location);
  *world_x = static_cast<int>(packed_location>>16);
  *world_y = static_cast<int>(packed_location&0xFFFF);

  {
    unsigned char byte_number = 0;
    buffer = decodeArray<unsigned short, unsigned char>(buffer,
                                                        buffer_end,
                                                        item_types,
                                                        MAX_ITEMS_VISIBLE_PER_LOCATION,
                        &byte_number);
    *number_of_item_types = byte_number;
  }

  return buffer != NULL;
}



bool createMagicFX(ENetPacket* packet, int* type, int* src_actor_id,
           float* src_x, float* src_y,
           int* power, int* target_ids,
           int target_buffer_entries,
           int* number_of_targets) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_CREATEMAGICFX, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<int>(buffer, buffer_end, type);
  buffer = decodeStructure<int>(buffer, buffer_end, src_actor_id);
  buffer = decodeStructure<float>(buffer, buffer_end, src_x);
  buffer = decodeStructure<float>(buffer, buffer_end, src_y);

  {
    unsigned char byte_power = 0;
    buffer = decodeStructure<unsigned char>(buffer, buffer_end, &byte_power);
    *power = byte_power;
  }

  {
    unsigned char byte_number_of_targets = 0;
    buffer = decodeArray<int, unsigned char>(buffer,
                         buffer_end,
                         target_ids,
                         target_buffer_entries,
                        &byte_number_of_targets);
    *number_of_targets = byte_number_of_targets;
  }

  return buffer != NULL;
}



//-----------------------------------------------------------------------------
bool updateOrTellInventory(ENetPacket* packet,
                           Evidyon::ClientAvatarInventoryItem fullClientInventory[Evidyon::AVATAR_INVENTORY_SIZE],
                           int* highest_inventory_update_index,
                           int inventory_update_indices[Evidyon::AVATAR_INVENTORY_SIZE]) {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_UPDATEINVENTORY, &buffer, &buffer_end)) {
    if (!openPacket(packet, NETMSG_TELLINVENTORY, &buffer, &buffer_end)) {
      return false;
    }

    // Clear the inventory
    *highest_inventory_update_index = Evidyon::AVATAR_INVENTORY_SIZE;

    for (int i = 0; i < Evidyon::AVATAR_INVENTORY_SIZE; ++i) {
      fullClientInventory[i].equipped = false;
      fullClientInventory[i].inventory_index = Evidyon::AVATAR_INVENTORY_SIZE;
      fullClientInventory[i].quantity = 0;
      fullClientInventory[i].type = (unsigned)(~0L);
      inventory_update_indices[i] = 0;
    }
  }

  int new_inventory_update_index = (*highest_inventory_update_index) + 1;
  *highest_inventory_update_index = new_inventory_update_index;

  int number_of_indices = 0;
  {
    unsigned char byte_number = 0;
    buffer = decodeStructure<unsigned char>(buffer, buffer_end, &byte_number);
    number_of_indices = byte_number;
  }

  for (int i = 0; i < number_of_indices; ++i) {
    ClientAvatarInventoryItem inventory_item;
    buffer = decodeStructure<ClientAvatarInventoryItem>(buffer,
                                                        buffer_end,
                                                       &inventory_item);

    int inventory_index = inventory_item.inventory_index;
    CONFIRM(inventory_index < Evidyon::AVATAR_INVENTORY_SIZE) else continue;
    memcpy(&fullClientInventory[inventory_index],
           &inventory_item,
            sizeof(inventory_item));
    inventory_update_indices[inventory_index] = new_inventory_update_index;
  }

  return buffer != NULL;
}


bool updateLocalWorldRegion(ENetPacket* packet,
                            World::RegionID* region_id) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_UPDATELOCALWORLDREIGON, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<World::RegionID>(buffer, buffer_end, region_id);

  return buffer != NULL;
}


bool tellActorName(ENetPacket* packet,
                   ActorID* actor_id,
                   std::string* name) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_TELLACTORNAME, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<ActorID>(buffer, buffer_end, actor_id);
  buffer = decodeString(buffer, buffer_end, name);

  return buffer != NULL;
}


bool storageTell(ENetPacket* packet,
                 Evidyon::ItemInStorage items[Evidyon::MAX_ITEMS_IN_STORAGE],
                 int* number_of_items) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_STORAGE_TELL, &buffer, &buffer_end))
  return false;

  unsigned char byte_number_of_items = 0;
  buffer = decodeArray<ItemInStorage, unsigned char>(buffer, buffer_end, items, MAX_ITEMS_IN_STORAGE, &byte_number_of_items);
  *number_of_items = byte_number_of_items;
  for (int i = (int)byte_number_of_items; i < MAX_ITEMS_IN_STORAGE; ++i) {
    items[i].index = -1;
    items[i].type = -1;
  }

  return buffer != NULL;
}



bool geosidTellGeonite(ENetPacket* packet, unsigned int* geonite) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_GEOSID_TELLGEONITE, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<unsigned int>(buffer, buffer_end, geonite);

  return buffer != NULL;
}

}
}
}
}
}