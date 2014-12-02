#include "../shared/client-server/packets.h"
#include "../shared/evidyon_avatar.h"
#include "../shared/evidyon_actor.h"
#include "../shared/evidyon_enchantments.h"
#include "../shared/evidyon_accounts.h"
#include "../shared/evidyon_avatarinventory.h"
#include "encodepackets.h"
#include "actor.h"
#include "../shared/client-server/askactorfulldescriptions.h"
#include "../shared/actorid.h"
#include "../shared/evidyon_network.h"

#include <dc/debug>

namespace Evidyon {
namespace Network {
namespace Packet {
namespace Client {
namespace Encode {


//------------------------------------------------------------------------------------------------
// Name:  tellClientVersionInfo
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* tellClientVersionInfo(unsigned long gamefileVersion, unsigned long clientVersion) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  By using the maximum password length and attempting
  // to write the message into the packet, we passively constrain the password to the maximum
  // length (no explicit check is performed in this function)
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned long) * 2, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_TELLCLIENTVERSIONINFO, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned long>(gamefileVersion, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned long>(clientVersion, packetBuffer, packetBufferEnd);

    // Resize the packet to snugly fit the data
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellClientVersionInfo"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}





//------------------------------------------------------------------------------------------------
// Name:  createAccount
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* createAccount(const char* password) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  By using the maximum password length and attempting
  // to write the message into the packet, we passively constrain the password to the maximum
  // length (no explicit check is performed in this function)
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        NETWORK_STRING_PADDING_BYTES +
                        MAX_PASSWORD_LENGTH, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_CREATEACCOUNT, packetBuffer, packetBufferEnd);
    packetBuffer = encodeString(password, packetBuffer, packetBufferEnd);

    // Resize the packet to snugly fit the data
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during createAccount"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



//------------------------------------------------------------------------------------------------
// Name:  accessAccount
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* accessAccount(unsigned int id, const char* password) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  By using the maximum password length and attempting
  // to write the message into the packet, we passively constrain the password to the maximum
  // length (no explicit check is performed in this function)
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int) +
                        NETWORK_STRING_PADDING_BYTES +
                        MAX_PASSWORD_LENGTH, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_ACCESSACCOUNT, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(id, packetBuffer, packetBufferEnd);
    packetBuffer = encodeString(password, packetBuffer, packetBufferEnd);

    // Resize the packet to fit the data
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during accessAccount"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}


//------------------------------------------------------------------------------------------------
// Name:  logoutAccount
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* logoutAccount() {
  using namespace ::Evidyon::Network::Packet;

  return encodeMessageOnlyPacket(NETMSG_LOGOUTACCOUNT);
}


//------------------------------------------------------------------------------------------------
// Name:  createCharacter
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* createCharacter(const char* name, bool male, int chRace,
                          int chClass, int chStr, int chAgi, int chCon,
                          int chInt, int chWis) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  Using MAX_AVATAR_NAME_LENGTH here passively
  // constrains the length of 'name'
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE + 
                        NETWORK_STRING_PADDING_BYTES +
                        MAX_AVATAR_NAME_LENGTH +
                        sizeof(unsigned long) // gender, race, class, str, agi, con, int, wis; 3 bits left over
                        , ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Generate the features
    unsigned long features = 3;
    features |= male ? 0x80000000 : 0x00000000;
    features |= (chRace   & 0xF) << 27;
    features |= (chClass  & 0xF) << 23;
    features |= ((chStr-10) & 0xF) << 19;
    features |= ((chAgi-10) & 0xF) << 15;
    features |= ((chCon-10) & 0xF) << 11;
    features |= ((chInt-10) & 0xF) <<  7;
    features |= ((chWis-10) & 0xF) <<  3;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_CREATECHARACTER, packetBuffer, packetBufferEnd);
    packetBuffer = encodeString(name, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned long>(features, packetBuffer, packetBufferEnd);

    // Resize the packet so that it perfectly fits the data
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during createCharacter"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}


//------------------------------------------------------------------------------------------------
// Name:  deleteCharacter
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* deleteCharacter(unsigned int id, const char* password) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  Using MAX_PASSWORD_LENGTH here passively
  // constrains the length of 'password'
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int) +
                        NETWORK_STRING_PADDING_BYTES +
                        MAX_PASSWORD_LENGTH, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_DELETECHARACTER, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(id, packetBuffer, packetBufferEnd);
    packetBuffer = encodeString(password, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during deleteCharacter"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



//------------------------------------------------------------------------------------------------
// Name:  askCharacterName
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* askCharacterName(unsigned int id) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_ASKCHARACTERNAME, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(id, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during askCharacterName"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}

//------------------------------------------------------------------------------------------------
// Name:  askAccountInfo
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* askAccountInfo() {
  using namespace ::Evidyon::Network::Packet;

  return encodeMessageOnlyPacket(NETMSG_ASKACCOUNTINFO);
}


//------------------------------------------------------------------------------------------------
// Name:  askCharacterMapLocation
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* askCharacterMapLocation(unsigned int id) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_ASKCHARACTERMAPLOCATION, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(id, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during askCharacterMapLocation"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}


//------------------------------------------------------------------------------------------------
// Name:  enterWorld
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* enterWorld(unsigned int id) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_ENTERWORLD, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(id, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during enterWorld"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



//------------------------------------------------------------------------------------------------
// Name:  leaveWorld
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* leaveWorld() {
  using namespace ::Evidyon::Network::Packet;

  return encodeMessageOnlyPacket(NETMSG_LEAVEWORLD);
}



//------------------------------------------------------------------------------------------------
// Name:  requestMove
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* requestMove(float x, float y, ActorID targetActorID) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int) + sizeof(int), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_REQUEST_MOVE,
                                     packetBuffer,
                                     packetBufferEnd);
    unsigned int coordinate;
    { // pack the coordinates
      unsigned short x_packed, y_packed;
      float fract;
      fract = fmodf(x, 1.0f);
      x_packed = ((int)(floor(x - fract)) << Network::FLOAT_PACK_DECIMAL_BITS) | static_cast<int>(floorf(fract * (1<<Network::FLOAT_PACK_DECIMAL_BITS)));
      fract = fmodf(y, 1.0f);
      y_packed = ((int)(floor(y - fract)) << Network::FLOAT_PACK_DECIMAL_BITS) | static_cast<int>(floorf(fract * (1<<Network::FLOAT_PACK_DECIMAL_BITS)));
      coordinate = (x_packed << 16) | y_packed;
    }
    packetBuffer = encodeStructure<unsigned int>(coordinate, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<ActorID>(targetActorID, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer) {
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
    }
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during requestMove"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



//------------------------------------------------------------------------------------------------
// Name:  requestMoveStreaming
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* requestMoveStreaming(float x, float y) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int), ENET_PACKET_FLAG_UNSEQUENCED);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_REQUEST_MOVE_STREAMING,
                                     packetBuffer,
                                     packetBufferEnd);
    unsigned int coordinate;
    { // pack the coordinates
      unsigned short x_packed, y_packed;
      float fract;
      fract = fmodf(x, 1.0f);
      x_packed = ((int)(floor(x - fract)) << Network::FLOAT_PACK_DECIMAL_BITS) | static_cast<int>(floorf(fract * (1<<Network::FLOAT_PACK_DECIMAL_BITS)));
      fract = fmodf(y, 1.0f);
      y_packed = ((int)(floor(y - fract)) << Network::FLOAT_PACK_DECIMAL_BITS) | static_cast<int>(floorf(fract * (1<<Network::FLOAT_PACK_DECIMAL_BITS)));
      coordinate = (x_packed << 16) | y_packed;
    }
    packetBuffer = encodeStructure<unsigned int>(coordinate, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer) {
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
    }
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during requestMove"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



//------------------------------------------------------------------------------------------------
// Name:  requestSpeak
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* requestSpeak(const char* speech) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        NETWORK_STRING_PADDING_BYTES +
                        MAX_SPOKEN_TEXT_CHARACTERS, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_REQUEST_SPEAK, packetBuffer, packetBufferEnd);
    packetBuffer = encodeString(speech, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during requestSpeak"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}




ENetPacket* askActorName(ActorID actor_id) {

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
            NETMSG_ASKACTORNAME,
            sizeof(ActorID),
            &packet,
            &buffer,
            &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<ActorID>(actor_id, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return 0;
  return packet;
}






//------------------------------------------------------------------------------------------------
// Name:  changeEquippedState
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* changeEquippedState(unsigned char* inventoryIndices, size_t numberOfIndices) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE + sizeof(numberOfIndices) +
                        sizeof(unsigned char) * numberOfIndices, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_CHANGEEQUIPPEDSTATE, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<size_t>(numberOfIndices, packetBuffer, packetBufferEnd);
    for (size_t i = 0; i < numberOfIndices; ++i)
      packetBuffer = encodeStructure<unsigned char>(inventoryIndices[i], packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during changeEquippedState"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}




//------------------------------------------------------------------------------------------------
// Name:  dropInventoryItems
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* dropInventoryItems(unsigned char* inventoryIndices, size_t numberOfIndices) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE + sizeof(numberOfIndices) +
                        sizeof(unsigned char) * numberOfIndices, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_DROPINVENTORYITEMS, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<size_t>(numberOfIndices, packetBuffer, packetBufferEnd);
    for (size_t i = 0; i < numberOfIndices; ++i)
      packetBuffer = encodeStructure<unsigned char>(inventoryIndices[i], packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during dropInventoryItems"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}






//------------------------------------------------------------------------------------------------
// Name:  askActorDescription
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* askActorDescription(int actorID) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(int), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_ASKACTORDESCRIPTION, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(actorID, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during askActorDescription"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}


//------------------------------------------------------------------------------------------------
// Name:  askActorDescription
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* askMyAvatarStats(int abilityToAddPoint) {
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(int), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_ASKMYAVATARSTATS, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(abilityToAddPoint, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during askMyAvatarStats"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



//------------------------------------------------------------------------------------------------
ENetPacket* changeItemsToTrade(unsigned char* trade_item_indices,
                          unsigned int number_of_items) {
  ;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
            NETMSG_CHANGEITEMSTOTRADE,
            sizeof(unsigned int) + number_of_items*sizeof(unsigned char),
            &packet,
            &buffer,
            &buffer_end)) {
    return 0;
  }

  unsigned char number_of_indices = (unsigned char)number_of_items;
  buffer = encodeStructure<unsigned char>(number_of_indices, buffer, buffer_end);
  for (unsigned int i=0; i<number_of_items; ++i) {
    buffer = encodeStructure<unsigned char>(trade_item_indices[i], buffer, buffer_end);
  }
  if (!validateAndTrimPacket(packet, buffer)) return 0;
  return packet;
}


//------------------------------------------------------------------------------------------------
ENetPacket* acceptTrade(int trade_key) {

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
            NETMSG_ACCEPTTRADE,
            sizeof(int),
            &packet,
            &buffer,
            &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<int>(trade_key, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return 0;
  return packet;
}




//------------------------------------------------------------------------------------------------
ENetPacket* listItemsForSale(
  unsigned char* inventory_indices,
  unsigned int* prices,
  unsigned int number_of_items) {
  CONFIRM(inventory_indices != NULL && prices != NULL && number_of_items < 255) else return NULL;
  ;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_BAZAAR_LISTITEMSFORSALE,
          sizeof(unsigned char)*2 + (sizeof(unsigned char)+sizeof(unsigned int)) * number_of_items,
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  unsigned char char_number_of_items = number_of_items;
  buffer = encodeArray<unsigned char,unsigned char>(
    inventory_indices,
    char_number_of_items,
    buffer,
    buffer_end);
  buffer = encodeArray<unsigned int,unsigned char>(
    prices,
    char_number_of_items,
    buffer,
    buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}

//------------------------------------------------------------------------------------------------
ENetPacket* getItemsForSale(unsigned int minimum_listing_id) {
  ;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_BAZAAR_GETITEMSFORSALE,
          sizeof(unsigned int),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<unsigned int>(minimum_listing_id, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}


//------------------------------------------------------------------------------------------------
ENetPacket* buyItem(unsigned int listing_id) {
  ;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_BAZAAR_BUYITEM,
          sizeof(unsigned int),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<unsigned int>(listing_id, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}

//------------------------------------------------------------------------------------------------
ENetPacket* getSellerStatus() {
  ;
  return encodeMessageOnlyPacket(NETMSG_BAZAAR_GETSELLERSTATUS);
}

//------------------------------------------------------------------------------------------------
ENetPacket* withdrawSellerItems(unsigned int* listing_ids, unsigned int number_of_items) {
  CONFIRM(listing_ids != NULL && number_of_items < 255) else return NULL;
  ;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_BAZAAR_WITHDRAWSELLERITEMS,
          sizeof(unsigned char) + sizeof(unsigned int) * number_of_items,
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  unsigned char char_number_of_items = number_of_items;
  buffer = encodeArray<unsigned int,unsigned char>(listing_ids,
                           char_number_of_items,
                           buffer,
                           buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}



ENetPacket* askActorFullDescriptions(const ActorSet* actor_set) {

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_ASKACTORFULLDESCRIPTIONS,
          sizeof(unsigned char) + sizeof(int) * actor_set->size(),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  size_t actors = actor_set->size();
  actors = min(Evidyon::Network::Packet::MAX_FULL_DESCRIPTIONS_TO_REQUEST_PER_PACKET, actors);
  unsigned char byte_size = (unsigned char)actors;
  buffer = encodeStructure<unsigned char>(byte_size, buffer, buffer_end);
  for (ActorSet::const_iterator i = actor_set->begin(); i != actor_set->end(); ++i) {
    buffer = encodeStructure<ActorID>((*i)->getID(), buffer, buffer_end);
  }

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}


ENetPacket* castSpell(const unsigned int spell_ids[3],
                      ActorID target_actor_id,
                      float target_x,
                      float target_y) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(false,
                    NETMSG_CAST_SPELL,
                    sizeof(unsigned char) * 3 +
                       sizeof(ActorID) +
                       sizeof(float) * 2,
                    &packet,
                    &buffer,
                    &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<unsigned char>(spell_ids[0], buffer, buffer_end);
  buffer = encodeStructure<unsigned char>(spell_ids[1], buffer, buffer_end);
  buffer = encodeStructure<unsigned char>(spell_ids[2], buffer, buffer_end);
  buffer = encodeStructure<ActorID>(target_actor_id, buffer, buffer_end);
  buffer = encodeStructure<float>(target_x, buffer, buffer_end);
  buffer = encodeStructure<float>(target_y, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}


ENetPacket* useItem(const unsigned char inventory_index,
                    ActorID target_actor_id,
                    float target_x,
                    float target_y) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(false,
                    NETMSG_USE_ITEM,
                    sizeof(unsigned char) * 2 +
                       sizeof(ActorID) +
                       sizeof(float) * 2,
                    &packet,
                    &buffer,
                    &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<unsigned char>(inventory_index, buffer, buffer_end);
  buffer = encodeStructure<ActorID>(target_actor_id, buffer, buffer_end);
  buffer = encodeStructure<float>(target_x, buffer, buffer_end);
  buffer = encodeStructure<float>(target_y, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}


ENetPacket* storageOpen(ActorID storage_npc_actor_id) {
  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
            NETMSG_STORAGE_OPEN,
            sizeof(ActorID),
            &packet,
            &buffer,
            &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<ActorID>(storage_npc_actor_id, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return 0;
  return packet;
}


ENetPacket* storageWithdraw(ActorID storage_npc_actor_id, int storage_index) {
  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
            NETMSG_STORAGE_WITHDRAW,
            sizeof(ActorID) + sizeof(unsigned char),
            &packet,
            &buffer,
            &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<ActorID>(storage_npc_actor_id, buffer, buffer_end);
  unsigned char byte_index = storage_index;
  buffer = encodeStructure<unsigned char>(byte_index, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return 0;
  return packet;
}


ENetPacket* storageDeposit(ActorID storage_npc_actor_id, int inventory_index) {
  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
            NETMSG_STORAGE_DEPOSIT,
            sizeof(ActorID) + sizeof(unsigned char),
            &packet,
            &buffer,
            &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<ActorID>(storage_npc_actor_id, buffer, buffer_end);
  unsigned char byte_inventory_index = inventory_index;
  buffer = encodeStructure<unsigned char>(byte_inventory_index, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return 0;
  return packet;
}


ENetPacket* geosidSacrifice(unsigned char inventory_index) {
  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
            NETMSG_GEOSID_SACRIFICE,
            sizeof(unsigned char),
            &packet,
            &buffer,
            &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<unsigned char>(inventory_index, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return 0;
  return packet;
}


}
}
}
}
}