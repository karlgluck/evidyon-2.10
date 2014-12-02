#include "../shared/client-server/packets.h"
#include "../shared/server-editor/server_gamefile.h"
#include "../shared/evidyon_avatarinventory.h"
#include "decodepackets.h"
#include <dc/debug>
#include "../shared/client-server/askactorfulldescriptions.h"
#include "../shared/evidyon_network.h"

//#include "../shared/evidyon_avatar.h"
//#include "../shared/evidyon_accounts.h"

namespace Evidyon {
namespace Network {
namespace Packet {
namespace Server {
namespace Decode {


//------------------------------------------------------------------------------------------------
// Name:  tellClientVersionInfo
// Desc:  
//------------------------------------------------------------------------------------------------
bool tellClientVersionInfo(ENetPacket* packet, unsigned long* gamefileVersion, unsigned long* clientVersion) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet ||  !gamefileVersion || !clientVersion)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_TELLCLIENTVERSIONINFO)
    return false;

  // Obtain the password
  buffer = decodeStructure<unsigned long>(buffer, endOfBuffer, gamefileVersion);
  buffer = decodeStructure<unsigned long>(buffer, endOfBuffer, clientVersion);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode createAccount message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("createAccount message contained extra data");

  // Success
  return true;
}




//------------------------------------------------------------------------------------------------
// Name:  createAccount
// Desc:  
//------------------------------------------------------------------------------------------------
bool createAccount(ENetPacket* packet, char* passwordBuffer, size_t bufferSize) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !passwordBuffer)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_CREATEACCOUNT)
    return false;

  // Obtain the password
  buffer = decodeString(buffer, endOfBuffer, passwordBuffer, bufferSize);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode createAccount message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("createAccount message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  accessAccount
// Desc:  
//------------------------------------------------------------------------------------------------
bool accessAccount(ENetPacket* packet, unsigned int* id, char* passwordBuffer, size_t bufferSize) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !id || !passwordBuffer)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_ACCESSACCOUNT)
    return false;

  // Read out the ID
  buffer = decodeStructure(buffer, endOfBuffer, id);

  // Obtain the password
  buffer = decodeString(buffer, endOfBuffer, passwordBuffer, bufferSize);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode accessAccount message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("createAccount message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  logoutAccount
// Desc:  
//------------------------------------------------------------------------------------------------
bool logoutAccount(ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  return decodeMessageOnlyPacket(packet, NETMSG_LOGOUTACCOUNT);
}



//------------------------------------------------------------------------------------------------
// Name:  createCharacter
// Desc:  
//------------------------------------------------------------------------------------------------
bool createCharacter(ENetPacket* packet, char* nameBuffer, size_t bufferSize, bool* male, int* chRace, int* chClass, int* chStr, int* chAgi, int* chCon, int* chInt, int* chWis) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !nameBuffer)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_CREATECHARACTER)
    return false;

  // Obtain the name
  buffer = decodeString(buffer, endOfBuffer, nameBuffer, bufferSize);

  // Get the features of this character
  unsigned long features;
  buffer = decodeStructure<unsigned long>(buffer, endOfBuffer, &features);

  // Decode the features
  *male  = (features & 0x80000000) == 0x80000000;
  *chRace  = ((features >> 27) & 0xF);
  *chClass = ((features >> 23) & 0xF);
  *chStr   = ((features >> 19) & 0xF) + 10;
  *chAgi   = ((features >> 15) & 0xF) + 10;
  *chCon   = ((features >> 11) & 0xF) + 10;
  *chInt   = ((features >>  7) & 0xF) + 10;
  *chWis   = ((features >>  3) & 0xF) + 10;

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode createCharacter message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("createCharacter message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  deleteCharacter
// Desc:  
//------------------------------------------------------------------------------------------------
bool deleteCharacter(ENetPacket* packet, unsigned int* id, char* passwordBuffer, size_t bufferSize) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !id || !passwordBuffer)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_DELETECHARACTER)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);
  buffer = decodeString(buffer, endOfBuffer, passwordBuffer, bufferSize);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode deleteCharacter message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("deleteCharacter message contained extra data");

  // Success
  return true;
}

//------------------------------------------------------------------------------------------------
// Name:  askCharacterName
// Desc:  
//------------------------------------------------------------------------------------------------
bool askCharacterName(ENetPacket* packet, unsigned int* id) {
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
    message != NETMSG_ASKCHARACTERNAME)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode askCharacterName message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("askCharacterName message contained extra data");

  // Success
  return true;
}


//------------------------------------------------------------------------------------------------
// Name:  askAccountInfo
// Desc:  
//------------------------------------------------------------------------------------------------
bool askAccountInfo(ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  return decodeMessageOnlyPacket(packet, NETMSG_ASKACCOUNTINFO);
}

bool askCharacterMapLocation(ENetPacket* packet, unsigned int* id) {
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
    message != NETMSG_ASKCHARACTERMAPLOCATION)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode askCharacterMapLocation message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("askCharacterMapLocation message contained extra data");

  // Success
  return true;
}


//------------------------------------------------------------------------------------------------
// Name:  enterWorld
// Desc:  
//------------------------------------------------------------------------------------------------
bool enterWorld(ENetPacket* packet, unsigned int* id) {
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
    message != NETMSG_ENTERWORLD)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, id);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode enterWorld message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("enterWorld message contained extra data");

  // Success
  return true;
}


//------------------------------------------------------------------------------------------------
// Name:  leaveWorld
// Desc:  
//------------------------------------------------------------------------------------------------
bool leaveWorld(ENetPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  return decodeMessageOnlyPacket(packet, NETMSG_LEAVEWORLD);
}


//------------------------------------------------------------------------------------------------
// Name:  requestMove
// Desc:  
//------------------------------------------------------------------------------------------------
bool requestMove(ENetPacket* packet, float* x, float* y, ActorID* targetActorID) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !x || !y)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_REQUEST_MOVE)
    return false;

  // Obtain the data from the packet
  {
    unsigned int packed_coordinate;
    buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, &packed_coordinate);
    *x = (0xFFFF & (packed_coordinate >> 16)) * Network::FLOAT_UNPACK_MULTIPLIER;
    *y = (0xFFFF & (packed_coordinate >>  0)) * Network::FLOAT_UNPACK_MULTIPLIER;
  }
  buffer = decodeStructure<ActorID>(buffer, endOfBuffer, targetActorID);


  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode requestMove message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("requestMove message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  requestMoveStreaming
// Desc:  
//------------------------------------------------------------------------------------------------
bool requestMoveStreaming(ENetPacket* packet, float* x, float* y) {
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
    message != NETMSG_REQUEST_MOVE_STREAMING)
    return false;

  // Obtain the data from the packet
  {
    unsigned int packed_coordinate;
    buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, &packed_coordinate);
    *x = (0xFFFF & (packed_coordinate >> 16)) * Network::FLOAT_UNPACK_MULTIPLIER;
    *y = (0xFFFF & (packed_coordinate >>  0)) * Network::FLOAT_UNPACK_MULTIPLIER;
  }

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode requestMove message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("requestMove message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  requestSpeak
// Desc:  
//------------------------------------------------------------------------------------------------
bool requestSpeak(ENetPacket* packet, char* speechBuffer, size_t bufferSize) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !speechBuffer || !bufferSize)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_REQUEST_SPEAK)
    return false;

  // Obtain the data from the packet
  buffer = decodeString(buffer, endOfBuffer, speechBuffer, bufferSize);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode requestSpeak message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("requestSpeak message contained extra data");

  // Success
  return true;
}


//------------------------------------------------------------------------------------------------
// Name:  requestAbility
// Desc:  
//------------------------------------------------------------------------------------------------
bool requestAbility(ENetPacket* packet, unsigned int abilityIndices[3], float targetXs[3], float targetYs[3], int targetActors[3]) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !abilityIndices || !targetXs || !targetYs)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_REQUEST_ABILITY)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, &abilityIndices[0]);
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, &abilityIndices[1]);
  buffer = decodeStructure<unsigned int>(buffer, endOfBuffer, &abilityIndices[2]);
  buffer = decodeStructure<float>(buffer, endOfBuffer, &targetXs[0]);
  buffer = decodeStructure<float>(buffer, endOfBuffer, &targetXs[1]);
  buffer = decodeStructure<float>(buffer, endOfBuffer, &targetXs[2]);
  buffer = decodeStructure<float>(buffer, endOfBuffer, &targetYs[0]);
  buffer = decodeStructure<float>(buffer, endOfBuffer, &targetYs[1]);
  buffer = decodeStructure<float>(buffer, endOfBuffer, &targetYs[2]);
  buffer = decodeStructure<int>(buffer, endOfBuffer, &targetActors[0]);
  buffer = decodeStructure<int>(buffer, endOfBuffer, &targetActors[1]);
  buffer = decodeStructure<int>(buffer, endOfBuffer, &targetActors[2]);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode requestAbility message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("requestAbility message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  changeEquippedState
// Desc:  
//------------------------------------------------------------------------------------------------
bool changeEquippedState(ENetPacket* packet, unsigned char* inventoryIndices, size_t bufferSize, size_t* numberOfIndices) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !inventoryIndices || !numberOfIndices)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_CHANGEEQUIPPEDSTATE)
    return false;

  // Obtain the data from the packet
  size_t indices = 0xFFFFFFFF;
  buffer = decodeStructure<size_t>(buffer, endOfBuffer, &indices);
  if (APP_ERROR(bufferSize < indices || indices > 255)("Invalid number of items")) return false;
  *numberOfIndices = indices;
  for (size_t i = 0; i < indices; ++i)
    buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &inventoryIndices[i]);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode changeEquippedState message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("changeEquippedState message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  dropInventoryItems
// Desc:  
//------------------------------------------------------------------------------------------------
bool dropInventoryItems(ENetPacket* packet, unsigned char* inventoryIndices, size_t bufferSize, size_t* numberOfIndices) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !inventoryIndices || !numberOfIndices)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_DROPINVENTORYITEMS)
    return false;

  // Obtain the data from the packet
  size_t indices = 0xFFFFFFFF;
  buffer = decodeStructure<size_t>(buffer, endOfBuffer, &indices);
  if (APP_ERROR(bufferSize < indices || indices > 255)("Invalid number of items")) return false;
  *numberOfIndices = indices;
  for (size_t i = 0; i < indices; ++i)
    buffer = decodeStructure<unsigned char>(buffer, endOfBuffer, &inventoryIndices[i]);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode dropInventoryItems message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("dropInventoryItems message contained extra data");

  // Success
  return true;
}




//------------------------------------------------------------------------------------------------
// Name:  askActorDescription
// Desc:  
//------------------------------------------------------------------------------------------------
bool askActorDescription(ENetPacket* packet, int* actorID) {
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
    message != NETMSG_ASKACTORDESCRIPTION)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<int>(buffer, endOfBuffer, actorID);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode askActorDescription message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("askActorDescription message contained extra data");

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name:  askMyAvatarStats
// Desc:  
//------------------------------------------------------------------------------------------------
bool askMyAvatarStats(ENetPacket* packet, int* abilityToAddPoint) {
  using namespace ::Evidyon::Network::Packet;
  // Make sure the packet is valid
  if (APP_ERROR(!packet || !abilityToAddPoint)("invalid parameter"))
    return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
    message != NETMSG_ASKMYAVATARSTATS)
    return false;

  // Obtain the data from the packet
  buffer = decodeStructure<int>(buffer, endOfBuffer, abilityToAddPoint);

  // Provide feedback about processing
  if (APP_ERROR(!buffer)("Failed to decode askMyAvatarStats message")) return false;
  if (buffer != endOfBuffer) DEBUG_WARNING("askMyAvatarStats message contained extra data");

  // Success
  return true;
}

//
//bool changeItemsToTrade(
//  ENetPacket* packet,
//  unsigned char* trade_item_indices,
//  unsigned int* number_of_items) {
//
//  using namespace ::Evidyon::Network::Packet;
//
//  const char* buffer;
//  const char* buffer_end;
//  if (!openPacket(packet, NETMSG_CHANGEITEMSTOTRADE, &buffer, &buffer_end)) return false;
//
//  unsigned char number_of_indices = 0; // sent as an unsigned char because its always << 255
//  buffer = decodeStructure<unsigned char>(buffer, buffer_end, &number_of_indices);
//  for (unsigned int i=0; buffer && i<number_of_indices; ++i) {
//  buffer = decodeStructure<unsigned char>(buffer, buffer_end, &trade_item_indices[i]);
//  }
//  *number_of_items = (unsigned int)number_of_indices;
//
//  // Provide feedback about processing
//  if (APP_ERROR(!buffer)("Failed to decode changeItemsToTrade message")) return false;
//  if (buffer != buffer_end) DEBUG_WARNING("changeItemsToTrade message contained extra data");
//
//  // Success
//  return true;
//}
//
//
//bool acceptTrade(ENetPacket* packet, int* trade_key) {
//  using namespace ::Evidyon::Network::Packet;
//
//  const char* buffer;
//  const char* buffer_end;
//  if (!openPacket(packet, NETMSG_ACCEPTTRADE, &buffer, &buffer_end)) return false;
//
//  buffer = decodeStructure<int>(buffer, buffer_end, trade_key);
//
//  // Provide feedback about processing
//  if (APP_ERROR(!buffer)("Failed to decode acceptTrade message")) return false;
//  if (buffer != buffer_end) DEBUG_WARNING("acceptTrade message contained extra data");
//
//  // Success
//  return true;
//}
//
//
////------------------------------------------------------------------------------------------------
//bool listItemsForSale(
//  ENetPacket* packet,
//  unsigned char* inventory_indices,
//  unsigned int* prices,
//  size_t buffer_size,
//  unsigned int* number_of_indices) {
//  using namespace ::Evidyon::Network::Packet;
//  const char* buffer, *buffer_end;
//  if (!openPacket(packet, NETMSG_BAZAAR_LISTITEMSFORSALE, &buffer, &buffer_end))
//  return false;
//
//  unsigned char char_number_of_items = 0;
//  buffer = decodeArray<unsigned char, unsigned char>(
//    buffer,
//    buffer_end,
//    inventory_indices,
//    buffer_size,
//    &char_number_of_items);
//  buffer = decodeArray<unsigned int, unsigned char>(
//    buffer,
//    buffer_end,
//    prices,
//    buffer_size,
//    &char_number_of_items);
//  *number_of_indices = char_number_of_items;
//
//  return buffer != NULL;
//}
//
////------------------------------------------------------------------------------------------------
//bool getItemsForSale(ENetPacket* packet,
//   unsigned int* minimum_listing_id) {
//  using namespace ::Evidyon::Network::Packet;
//
//  using namespace ::Evidyon::Network::Packet;
//  const char* buffer, *buffer_end;
//  if (!openPacket(packet, NETMSG_BAZAAR_GETITEMSFORSALE, &buffer, &buffer_end))
//  return false;
//
//  buffer = decodeStructure<unsigned int>(buffer, buffer_end, minimum_listing_id);
//
//  return buffer != NULL;
//}
//
////------------------------------------------------------------------------------------------------
//bool buyItem(ENetPacket* packet,
//   unsigned int* listing_id) {
//  using namespace ::Evidyon::Network::Packet;
//  const char* buffer, *buffer_end;
//  if (!openPacket(packet, NETMSG_BAZAAR_BUYITEM, &buffer, &buffer_end))
//  return false;
//
//  buffer = decodeStructure<unsigned int>(buffer, buffer_end, listing_id);
//
//  return buffer != NULL;
//}
//
////------------------------------------------------------------------------------------------------
//bool getSellerStatus(ENetPacket* packet) {
//  using namespace ::Evidyon::Network::Packet;
//  return decodeMessageOnlyPacket(packet, NETMSG_BAZAAR_GETSELLERSTATUS);
//}
//
////------------------------------------------------------------------------------------------------
//bool withdrawSellerItems(ENetPacket* packet,
//   unsigned int* listing_ids,
//   size_t buffer_size,
//   unsigned int* number_of_items) {
//  using namespace ::Evidyon::Network::Packet;
//
//  const char* buffer, *buffer_end;
//  if (!openPacket(packet, NETMSG_BAZAAR_WITHDRAWSELLERITEMS, &buffer, &buffer_end))
//  return false;
//
//  unsigned char char_number_of_items = 0;
//  buffer = decodeArray<unsigned int, unsigned char>(buffer, buffer_end,
//                          listing_ids,
//                          buffer_size,
//                          &char_number_of_items);
//  *number_of_items = char_number_of_items;
//
//  return buffer != NULL;
//}


bool askActorFullDescriptions(ENetPacket* packet, ActorID* actor_ids, size_t* number_of_actors) {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  CONFIRM(openPacket(packet, NETMSG_ASKACTORFULLDESCRIPTIONS, &buffer, &buffer_end)) else return false;

  unsigned char byte_size = 0;
  buffer = decodeArray<ActorID, unsigned char>(buffer, buffer_end,
                       actor_ids,
                       MAX_FULL_DESCRIPTIONS_TO_REQUEST_PER_PACKET,
                       &byte_size);
  *number_of_actors = byte_size;

  return !APP_ERROR(buffer == NULL)("Couldn't decode actor array");
}




bool castSpell(ENetPacket* packet, unsigned int spell_ids[3],
         ActorID* target_actor_id,
         float* target_x, float* target_y) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_CAST_SPELL, &buffer, &buffer_end))
  return false;

  for (int i = 0; i < 3; ++i) {
    unsigned char spell_id_local;
    buffer = decodeStructure<unsigned char>(buffer, buffer_end, &spell_id_local);
    spell_ids[i] = spell_id_local;
  }
  buffer = decodeStructure<ActorID>(buffer, buffer_end, target_actor_id);
  buffer = decodeStructure<float>(buffer, buffer_end, target_x);
  buffer = decodeStructure<float>(buffer, buffer_end, target_y);


  return buffer != NULL;
}

bool useItem(ENetPacket* packet, unsigned int* inventory_index,
         ActorID* target_actor_id,
         float* target_x, float* target_y) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_USE_ITEM, &buffer, &buffer_end))
  return false;

  unsigned char inventory_index_local;
  buffer = decodeStructure<unsigned char>(buffer, buffer_end, &inventory_index_local);
  *inventory_index = inventory_index_local;
  buffer = decodeStructure<ActorID>(buffer, buffer_end, target_actor_id);
  buffer = decodeStructure<float>(buffer, buffer_end, target_x);
  buffer = decodeStructure<float>(buffer, buffer_end, target_y);


  return buffer != NULL;
}




bool askActorName(ENetPacket* packet, ActorID* actor_id) {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_ASKACTORNAME, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<ActorID>(buffer, buffer_end, actor_id);
  return buffer != NULL;
}


bool storageOpen(ENetPacket* packet, ActorID* storage_npc_actor_id) {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_STORAGE_OPEN, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<ActorID>(buffer, buffer_end, storage_npc_actor_id);
  return buffer != NULL;
}


bool storageWithdraw(ENetPacket* packet, ActorID* storage_npc_actor_id, int* storage_index) {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_STORAGE_WITHDRAW, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<ActorID>(buffer, buffer_end, storage_npc_actor_id);
  {
    unsigned char byte_index = 0;
    buffer = decodeStructure<unsigned char>(buffer, buffer_end, &byte_index);
    *storage_index = byte_index;
  }
  return buffer != NULL;
}


bool storageDeposit(ENetPacket* packet, ActorID* storage_npc_actor_id, int* inventory_index)  {
  using namespace ::Evidyon::Network::Packet;

  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_STORAGE_DEPOSIT, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<ActorID>(buffer, buffer_end, storage_npc_actor_id);
  {
    unsigned char byte_index = 0;
    buffer = decodeStructure<unsigned char>(buffer, buffer_end, &byte_index);
    *inventory_index = byte_index;
  }
  return buffer != NULL;
}


bool geosidSacrifice(ENetPacket* packet, unsigned char* inventory_index) {
  using namespace ::Evidyon::Network::Packet;
  
  const char* buffer, *buffer_end;
  if (!openPacket(packet, NETMSG_GEOSID_SACRIFICE, &buffer, &buffer_end))
  return false;

  buffer = decodeStructure<unsigned char>(buffer, buffer_end, inventory_index);

  return buffer != NULL;
}



}
}
}
}
}