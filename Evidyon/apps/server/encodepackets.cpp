#include "../shared/client-server/packets.h"
#include "../shared/server-editor/server_gamefile.h"
#include "../shared/evidyon_avatarinventory.h"
#include "encodepackets.h"
#include <dc/debug>
#include "../shared/evidyon_avatar.h"
#include "../shared/evidyon_accounts.h"
#include "avatarattributes.h"
#include "../shared/client-server/actorsync.h"

#include "../common/templateserializer.h"

#include "../shared/client-server/actordescription.h"
#include "../shared/client-server/tellactorfulldescriptions.h"
#include "../shared/client-server/fastsync.h"
#include "item.h"
#include "avatarinventoryitem.h"

#include "../shared/evidyon_storage.h"


namespace Evidyon {
namespace Network {
namespace Packet {
namespace Server {
namespace Encode {


#pragma pack(push, 1)

struct ItemForTrade {
  unsigned listing_id  : 32;
  unsigned price     : 32;
  unsigned quantity  : 8;
  unsigned type    : 15;
  unsigned purchased   : 1;
  unsigned you_sell  : 1; // only valid for "get listed items" by client; denotes that the viewer is the seller
  unsigned __void    : 7;
};

#pragma pack(pop)


  
//------------------------------------------------------------------------------------------------
// Name:  tellServerVersionUpdateInfo
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* tellServerVersionUpdateInfo(bool versionIsValid, bool needsNewGamefile, bool needsNewClient, const char* updateText){
  using namespace ::Evidyon::Network::Packet;


  //typedef ::Evidyon::Network::Packet
  // Allocate a packet for this message.  Using MAX_AVATAR_NAME_LENGTH here passively
  // constrains the length of 'name'
  ENetPacket* packet = enet_packet_create(0,NETWORK_MESSAGE_SIZE +
    sizeof(bool) * 4 + NETWORK_STRING_PADDING_BYTES + 256, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_TELLSERVERVERSIONUPDATEINFO, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<bool>(versionIsValid, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<bool>(needsNewGamefile, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<bool>(needsNewClient, packetBuffer, packetBufferEnd);
    packetBuffer = encodeString(updateText ? updateText : "", packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellServerVersionUpdateInfo"))
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
// Name:  createAccountSucceeded
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* createAccountSucceeded(unsigned int accountID){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  Using MAX_AVATAR_NAME_LENGTH here passively
  // constrains the length of 'name'
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_CREATEACCOUNT_SUCCEEDED, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(accountID, packetBuffer, packetBufferEnd);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during createAccountSucceeded"))
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
// Name:  tellAccountInfo
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* tellAccountInfo(float daysLeft, const unsigned int* characterIDArray, size_t numberOfCharacters){
  using namespace ::Evidyon;
  using namespace ::Evidyon::Network::Packet;

  // Validate parameters
  if (APP_ERROR(daysLeft < 0.0f || !characterIDArray ||
    (numberOfCharacters > Evidyon::MAX_AVATARS_PER_ACCOUNT))("Invalid parameter to tellAccountInfo"))
    return 0;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(float) +
                        sizeof(size_t) +
                        sizeof(unsigned int) * numberOfCharacters, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet header
    packetBuffer = encodeMessageType(NETMSG_TELLACCOUNTINFO, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(daysLeft, packetBuffer, packetBufferEnd);

    // Write the number of characters in the account
    packetBuffer = encodeStructure<size_t>(numberOfCharacters, packetBuffer, packetBufferEnd);

    // Write the array of character IDs
    for (size_t i = 0; i < numberOfCharacters; ++i)
    {
      packetBuffer = encodeStructure<unsigned int>(characterIDArray[i], packetBuffer, packetBufferEnd);
    }
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellAccountInfo"))
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
// Name:  requestActionResult
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* requestActionResult(::Evidyon::Network::Packet::RequestActionResult result){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  Using MAX_AVATAR_NAME_LENGTH here passively
  // constrains the length of 'name'
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(RequestActionResult), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_REQUESTACTIONRESULT, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<RequestActionResult>(result, packetBuffer, packetBufferEnd);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during requestActionResult"))
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
// Name:  logoutAccountConfirmed
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* logoutAccountConfirmed(){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_LOGOUTACCOUNT_CONFIRMED, packetBuffer, packetBufferEnd);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during logoutAccountConfirmed"))
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
// Name:  tellCharacterName
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* tellCharacterName(unsigned int id, const char* characterName,
  float world_x, float world_y, unsigned int actor_type, size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT]){
  using namespace ::Evidyon;
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message.  Using MAX_AVATAR_NAME_LENGTH here passively
  // constrains the length of 'name'
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int) +
                        NETWORK_STRING_PADDING_BYTES +
                        MAX_AVATAR_NAME_LENGTH +
                        sizeof(float)*2 + sizeof(actor_type) +
                        sizeof(size_t)*ACTORATTACHMENTPOINT_COUNT, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_TELLCHARACTERNAME, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(id, packetBuffer, packetBufferEnd);
    packetBuffer = encodeString(characterName, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(world_x, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(world_y, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(actor_type, packetBuffer, packetBufferEnd);
    for (size_t i = 0; i < ACTORATTACHMENTPOINT_COUNT; ++i)
      packetBuffer = encodeStructure<size_t>(visible_equipment[i], packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellCharacterName"))
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
// Name:  createCharacterSucceeded
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* createCharacterSucceeded(unsigned int characterID){
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
    packetBuffer = encodeMessageType(NETMSG_CREATECHARACTER_SUCCEEDED, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(characterID, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during createCharacterSucceeded"))
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
// Name:  tellCharacterMapLocation
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* tellCharacterMapLocation(unsigned int id, int mapX, int mapY){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned int) +
                        sizeof(int) * 2, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_TELLCHARACTERMAPLOCATION, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(id, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(mapX, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(mapY, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellCharacterMapLocation"))
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
// Name:  enterWorldSucceeded
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* enterWorldSucceeded(ActorID actorID,
                                unsigned long time_of_day,
                                float yourX,
                                float yourY,
                                int map_index,
                                const AvatarAttributes* avatar,
                                const SpellAvailabilityMask* spellAvailabilityMask) {
  using namespace ::Evidyon::Network::Packet;

  bool male = avatar->genderIndex == MALE;
  int raceIndex = avatar->raceIndex;
  int classIndex =avatar->classIndex;
  int hp = avatar->hp;
  int maxHP = avatar->current.max_hp;
  int currentMP = avatar->mp;
  int maxMP = avatar->current.max_mp;
  unsigned int money = avatar->money;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(ActorID) +
                        sizeof(unsigned long) +
                        sizeof(int) * 7 +
                        sizeof(float) * 2 +
                        sizeof(unsigned int)*2 +
                        sizeof(bool) + sizeof(SpellAvailabilityMask), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_ENTERWORLD_SUCCEEDED, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<ActorID>(actorID, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned long>(time_of_day, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(yourX, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(yourY, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(map_index, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<bool>(male, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(raceIndex, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(classIndex, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(hp, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(maxHP, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(currentMP, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(maxMP, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(money, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned int>(avatar->geonite, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<SpellAvailabilityMask>(*spellAvailabilityMask, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during enterWorldSucceeded"))
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
// Name:  leaveWorldConfirmed
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* leaveWorldConfirmed(){
  using namespace ::Evidyon::Network::Packet;

  return encodeMessageOnlyPacket(NETMSG_LEAVEWORLD_CONFIRMED);
}



//------------------------------------------------------------------------------------------------
// Name:  tellActorActivity
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* tellActorActivity(int actorID, float angle, float speed, float x, float y, float distanceToMove, ActorState currentState, ActorAction action, float actionSpeed){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(int) + sizeof(float) * 5 + sizeof(ActorState) + sizeof(ActorAction) + sizeof(float), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  //DEBUG_INFO("tellActorActivity:  actor %i, position (%.02f, %.02f) angle %.02f speed %.02f", actorID, x, y, angle, speed);

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_TELLACTORACTIVITY, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(actorID, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(x, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(y, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(angle, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(speed, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(distanceToMove, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<ActorState>(currentState, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<ActorAction>(action, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(actionSpeed, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellActorActivity"))
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
// Name:  destroyActor
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* destroyActor(int actorID){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(int), ENET_PACKET_FLAG_RELIABLE); // TODO: make this unreliable/unsequenced...but it causes problems clientside
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_DESTROYACTOR, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(actorID, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during destroyActor"))
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
// Name:  actorSpeaks
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* actorSpeaks(int actorID, const char* name, const char* speech){
  using namespace ::Evidyon;
  using namespace ::Evidyon::Network::Packet;

  if (name) actorID = -1;
  bool include_name = actorID == -1;
  if (include_name && name == NULL) name = ""; // Don't let the name be invalid

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(int) +
                        (include_name ? NETWORK_STRING_PADDING_BYTES + MAX_AVATAR_NAME_LENGTH*2 : 0) +
                        NETWORK_STRING_PADDING_BYTES +
                        MAX_SPOKEN_TEXT_CHARACTERS,
                        ENET_PACKET_FLAG_RELIABLE); // TODO: make this unreliable
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_ACTORSPEAKS, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(actorID, packetBuffer, packetBufferEnd);
    if (include_name) packetBuffer = encodeString(name, packetBuffer, packetBufferEnd);
    packetBuffer = encodeString(speech, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during actorSpeaks"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



////------------------------------------------------------------------------------------------------
//// Name:  updateYourCurrentHPMPEXP
//// Desc:  
////------------------------------------------------------------------------------------------------
//ENetPacket* updateYourCurrentHPMPEXP(float currentHPPercent, float currentMPPercent, float currentEXPPercent){
//  using namespace ::Evidyon::Network::Packet;
//
//  // Allocate a packet for this message
//  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
//                        sizeof(float) * 3, ENET_PACKET_FLAG_RELIABLE); // TODO: make this unreliable/unsequenced...but it causes problems clientside
//  char* packetBuffer = 0;
//
//  // If we obtained the packet, write information into it
//  if (packet)
//  {
//    packetBuffer = (char*)packet->data;
//    const char* packetBufferEnd = packetBuffer + packet->dataLength;
//
//    // Write the packet
//    packetBuffer = encodeMessageType(NETMSG_UPDATEYOURCURRENTHPMPEXP, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<float>(currentHPPercent,  packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<float>(currentMPPercent,  packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<float>(currentEXPPercent, packetBuffer, packetBufferEnd);
//
//    // Make sure the packet is only as big as necessary
//    if (packetBuffer)
//      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
//  }
//
//  // Make sure compilation succeeded
//  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during updateYourCurrentHPMP"))
//  {
//    // Free the memory for the packet
//    enet_packet_destroy(packet);
//
//    // Fail
//    return 0;
//  }
//
//  // Success
//  return packet;
//}




//
////------------------------------------------------------------------------------------------------
//// Name:  createSpecialFX
//// Desc:  
////------------------------------------------------------------------------------------------------
//ENetPacket* createSpecialFX(
//  int sfxID, int srcActor, int srcProjectile, float srcX,
//  float srcY, int destActor, float destX, float destY){
//  using namespace ::Evidyon::Network::Packet;
//
//  // Allocate a packet for this message
//  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
//                        sizeof(int) * 4 +
//                        sizeof(float)*4, ENET_PACKET_FLAG_RELIABLE); // TODO: make this unreliable/unsequenced...but it causes problems clientside
//  char* packetBuffer = 0;
//
//  // If we obtained the packet, write information into it
//  if (packet)
//  {
//    packetBuffer = (char*)packet->data;
//    const char* packetBufferEnd = packetBuffer + packet->dataLength;
//
//    // Write the packet
//    packetBuffer = encodeMessageType(NETMSG_CREATESPECIALFX, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<int>(sfxID, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<int>(srcActor, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<int>(srcProjectile, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<float>(srcX, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<float>(srcY, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<int>(destActor, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<float>(destX, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<float>(destY, packetBuffer, packetBufferEnd);
//
//    // Make sure the packet is only as big as necessary
//    if (packetBuffer)
//      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
//  }
//
//  // Make sure compilation succeeded
//  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during updateYourCurrentHPMP"))
//  {
//    // Free the memory for the packet
//    enet_packet_destroy(packet);
//
//    // Fail
//    return 0;
//  }
//
//  // Success
//  return packet;
//}


//------------------------------------------------------------------------------------------------
// Name:  createProjectile
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* createProjectile(int projectile_id, int projectile_type, float x, float y, float dx, float dy){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(int)*2 +
                        sizeof(float)*4, ENET_PACKET_FLAG_RELIABLE); // TODO: make this unreliable/unsequenced...but it causes problems clientside
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_CREATEPROJECTILE, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(projectile_id, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(projectile_type, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(x, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(y, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(dx, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<float>(dy, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during createProjectile"))
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
// Name:  destroyProjectile
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* destroyProjectile(int projectile_id){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(int), ENET_PACKET_FLAG_RELIABLE); // TODO: make this unreliable/unsequenced...but it causes problems clientside
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_DESTROYPROJECTILE, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(projectile_id, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during destroyProjectile"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



//
////------------------------------------------------------------------------------------------------
//// Name:  updateInventory
//// Desc:  
////------------------------------------------------------------------------------------------------
//ENetPacket* updateInventory(bool isFullInventoryUpdate, const AvatarInventoryItem* itemsToScan, size_t numberOfItemsToScan){
//  using namespace ::Evidyon::Network::Packet;
//
//  // Look through the items, and find out how many are valid
//  size_t validItems = 0;
//  for (size_t i = 0; i < numberOfItemsToScan; ++i)
//  {
//    if (itemsToScan[i].invalid) continue;
//    ++validItems;
//  }
//
//  // Allocate a packet for this message
//  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
//                        sizeof(bool) + sizeof(unsigned char) +
//                        sizeof(AvatarInventoryItem) * validItems, ENET_PACKET_FLAG_RELIABLE); // TODO: make this unreliable/unsequenced...but it causes problems clientside
//  char* packetBuffer = 0;
//
//  // If we obtained the packet, write information into it
//  if (packet)
//  {
//    packetBuffer = (char*)packet->data;
//    const char* packetBufferEnd = packetBuffer + packet->dataLength;
//
//    // Write the packet
//    packetBuffer = encodeMessageType(NETMSG_UPDATEINVENTORY, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<bool>(isFullInventoryUpdate, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<unsigned char>((unsigned char)validItems, packetBuffer, packetBufferEnd);
//
//    // Write each of the items
//    for (size_t i = 0; i < numberOfItemsToScan; ++i)
//    {
//      // If this item is invalid, don't put it in the packet
//      if (itemsToScan[i].invalid) continue;
//
//      // TODO: this can be replaced by a MEMCOPY
//      AvatarInventoryItem cei;
//      cei.equipped = itemsToScan[i].equipped;
//      cei.inventory_index = itemsToScan[i].inventory_index;
//      cei.quantity = itemsToScan[i].quantity;
//      cei.type = itemsToScan[i].type;
//
//      // Add this entry to the buffer
//      packetBuffer = encodeStructure<AvatarInventoryItem>(cei, packetBuffer, packetBufferEnd);
//    }
//
//    // Make sure the packet is only as big as necessary
//    if (packetBuffer)
//      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
//  }
//
//  // Make sure compilation succeeded
//  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during updateInventory"))
//  {
//    // Free the memory for the packet
//    enet_packet_destroy(packet);
//
//    // Fail
//    return 0;
//  }
//
//  // Success
//  return packet;
//}



////------------------------------------------------------------------------------------------------
//// Name:  updateInventory
//// Desc:  
////------------------------------------------------------------------------------------------------
//ENetPacket* updateInventory(
//  const AvatarInventoryItem* inventory, unsigned char* indicesToScan, size_t numberOfItemsToUpdate){
//  using namespace ::Evidyon::Network::Packet;
//
//  if (numberOfItemsToUpdate >= AVATAR_INVENTORY_SIZE) return 0;
//
//  // Allocate a packet for this message
//  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
//                        sizeof(bool) + sizeof(unsigned char) +
//                        sizeof(AvatarInventoryItem) * numberOfItemsToUpdate, ENET_PACKET_FLAG_RELIABLE); // TODO: make this unreliable/unsequenced...but it causes problems clientside
//  char* packetBuffer = 0;
//
//  // If we obtained the packet, write information into it
//  if (packet)
//  {
//    packetBuffer = (char*)packet->data;
//    const char* packetBufferEnd = packetBuffer + packet->dataLength;
//
//    // Write the packet
//    packetBuffer = encodeMessageType(NETMSG_UPDATEINVENTORY, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<bool>(false, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<unsigned char>(numberOfItemsToUpdate&0xFF, packetBuffer, packetBufferEnd);
//
//    // Write each of the items
//    for (size_t i = 0; i < numberOfItemsToUpdate; ++i)
//    {
//      // If this item is invalid, don't put it in the packet
//      int index = indicesToScan[i];
//      if (APP_WARNING(index >= AVATAR_INVENTORY_SIZE)("Invalid index in updateInventory")) index = 0;
//
//      // Make sure the index is correct
//      CONFIRM(inventory[index].inventory_index == index);
//
//      // TODO: this can be replaced by a MEMCOPY
//      AvatarInventoryItem cei;
//      if (inventory[index].invalid) {
//        cei.equipped = 0;
//        cei.inventory_index = index; // == inventory[index].inventory_index
//        cei.quantity = 0;
//        cei.type = 0;
//      } else {
//        cei.equipped = inventory[index].equipped;
//        cei.inventory_index = index; // == inventory[index].inventory_index
//        cei.quantity = inventory[index].quantity;
//        cei.type = inventory[index].type;
//      }
//
//      // Add this entry to the buffer
//      packetBuffer = encodeStructure<AvatarInventoryItem>(cei, packetBuffer, packetBufferEnd);
//    }
//
//    // Make sure the packet is only as big as necessary
//    if (packetBuffer)
//      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
//  }
//
//  // Make sure compilation succeeded
//  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during updateInventory"))
//  {
//    // Free the memory for the packet
//    enet_packet_destroy(packet);
//
//    // Fail
//    return 0;
//  }
//
//  // Success
//  return packet;
//}






////------------------------------------------------------------------------------------------------
//// Name:  removeInventoryItems
//// Desc:  
////------------------------------------------------------------------------------------------------
//ENetPacket* removeInventoryItems(unsigned char* inventoryIndices, size_t numberOfIndices){
//  using namespace ::Evidyon::Network::Packet;
//
//  // Allocate a packet for this message
//  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE + sizeof(numberOfIndices) +
//                        sizeof(unsigned char) * numberOfIndices, ENET_PACKET_FLAG_RELIABLE); // TODO: make this unreliable/unsequenced...but it causes problems clientside
//  char* packetBuffer = 0;
//
//  // If we obtained the packet, write information into it
//  if (packet)
//  {
//    packetBuffer = (char*)packet->data;
//    const char* packetBufferEnd = packetBuffer + packet->dataLength;
//
//    // Write the packet
//    packetBuffer = encodeMessageType(NETMSG_REMOVEINVENTORYITEMS, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<size_t>(numberOfIndices, packetBuffer, packetBufferEnd);
//    for (size_t i = 0; i < numberOfIndices; ++i)
//      packetBuffer = encodeStructure<unsigned char>(inventoryIndices[i], packetBuffer, packetBufferEnd);
//
//    // Make sure the packet is only as big as necessary
//    if (packetBuffer)
//      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
//  }
//
//  // Make sure compilation succeeded
//  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during removeInventoryItems"))
//  {
//    // Free the memory for the packet
//    enet_packet_destroy(packet);
//
//    // Fail
//    return 0;
//  }
//
//  // Success
//  return packet;
//}






////------------------------------------------------------------------------------------------------
//// Name:  tellGroundItems
//// Desc:  
////------------------------------------------------------------------------------------------------
//ENetPacket* tellGroundItems(int x, int y, size_t* itemTypes, size_t numberOfItems){
//  using namespace ::Evidyon::Network::Packet;
//
//  // Allocate a packet for this message
//  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE + sizeof(int) * 2 + sizeof(numberOfItems) +
//                        sizeof(size_t) * numberOfItems, ENET_PACKET_FLAG_RELIABLE); // TODO: make this unreliable/unsequenced...but it causes problems clientside
//  char* packetBuffer = 0;
//
//  // If we obtained the packet, write information into it
//  if (packet)
//  {
//    packetBuffer = (char*)packet->data;
//    const char* packetBufferEnd = packetBuffer + packet->dataLength;
//
//    // Write the packet
//    packetBuffer = encodeMessageType(NETMSG_TELLGROUNDITEMS, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<int>(x, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<int>(y, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<size_t>(numberOfItems, packetBuffer, packetBufferEnd);
//    for (size_t i = 0; i < numberOfItems; ++i)
//    {
//      packetBuffer = encodeStructure<size_t>(itemTypes[i], packetBuffer, packetBufferEnd);
//    }
//
//    // Make sure the packet is only as big as necessary
//    if (packetBuffer)
//      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
//  }
//
//  // Make sure compilation succeeded
//  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellGroundItems"))
//  {
//    // Free the memory for the packet
//    enet_packet_destroy(packet);
//
//    // Fail
//    return 0;
//  }
//
//  // Success
//  return packet;
//}

//
//
////------------------------------------------------------------------------------------------------
//// Name:  tellActorDescription
//// Desc:  
////------------------------------------------------------------------------------------------------
//ENetPacket* tellActorDescription(
//  const char* actorName, int actorID, size_t actorType, ActorState currentState,
//  size_t visibleEquipment[ACTORATTACHMENTPOINT_COUNT],
//  size_t visibleEnchantments[NUMBER_OF_ENCHANTMENT_SLOTS]){
//  using namespace ::Evidyon::Network::Packet;
//
//  // Allocate a packet for this message
//  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE + NETWORK_STRING_PADDING_BYTES + MAX_AVATAR_NAME_LENGTH*2 +   // TODO: this is *2 because AI character names might be longer
//                        sizeof(int) + sizeof(size_t) + sizeof(ActorState) + sizeof(size_t) * ACTORATTACHMENTPOINT_COUNT +
//                        sizeof(size_t) * NUMBER_OF_ENCHANTMENT_SLOTS, ENET_PACKET_FLAG_RELIABLE);
//  char* packetBuffer = 0;
//
//  // If we obtained the packet, write information into it
//  if (packet)
//  {
//    packetBuffer = (char*)packet->data;
//    const char* packetBufferEnd = packetBuffer + packet->dataLength;
//
//    // Write the packet
//    packetBuffer = encodeMessageType(NETMSG_TELLACTORDESCRIPTION, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeString(actorName, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<int>(actorID, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<size_t>(actorType, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<ActorState>(currentState, packetBuffer, packetBufferEnd);
//    for (size_t i = 0; i < ACTORATTACHMENTPOINT_COUNT; ++i)  packetBuffer = encodeStructure<size_t>(visibleEquipment[i], packetBuffer, packetBufferEnd);
//    for (size_t i = 0; i < NUMBER_OF_ENCHANTMENT_SLOTS; ++i) packetBuffer = encodeStructure<size_t>(visibleEnchantments[i], packetBuffer, packetBufferEnd);
//
//    // Make sure the packet is only as big as necessary
//    if (packetBuffer)
//      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
//  }
//
//  // Make sure compilation succeeded
//  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellActorDescription"))
//  {
//    // Free the memory for the packet
//    enet_packet_destroy(packet);
//
//    // Fail
//    return 0;
//  }
//
//  // Success
//  return packet;
//}
//
//

////------------------------------------------------------------------------------------------------
//// Name:  updateYourActivePartition
//// Desc:  
////------------------------------------------------------------------------------------------------
//ENetPacket* updateYourActivePartition(short px, short py){
//  using namespace ::Evidyon::Network::Packet;
//
//  // Allocate a packet for this message
//  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
//                        sizeof(short)*2, ENET_PACKET_FLAG_RELIABLE);
//  char* packetBuffer = 0;
//
//  // If we obtained the packet, write information into it
//  if (packet)
//  {
//    packetBuffer = (char*)packet->data;
//    const char* packetBufferEnd = packetBuffer + packet->dataLength;
//
//    // Write the packet
//    packetBuffer = encodeMessageType(NETMSG_UPDATEYOURACTIVEPARTITION, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<short>(px, packetBuffer, packetBufferEnd);
//    packetBuffer = encodeStructure<short>(py, packetBuffer, packetBufferEnd);
//
//    // Make sure the packet is only as big as necessary
//    if (packetBuffer)
//      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
//  }
//
//  // Make sure compilation succeeded
//  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during updateYourActivePartition"))
//  {
//    // Free the memory for the packet
//    enet_packet_destroy(packet);
//
//    // Fail
//    return 0;
//  }
//
//  // Success
//  return packet;
//}


//------------------------------------------------------------------------------------------------
// Name:  updateYourEffectAvailabilityMask
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* updateYourEffectAvailabilityMask(const SpellAvailabilityMask* mask){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(SpellAvailabilityMask), ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_UPDATEYOUREFFECTAVAILABILITYMASK, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<SpellAvailabilityMask>(*mask, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during updateYourEffectAvailabilityMask"))
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
// Name:  tellYourAvatarStats
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* tellYourAvatarStats(
  unsigned char level, unsigned char abilities[5], unsigned char availableAbilityPoints,
  int hp, int hpMax, int mp, int mpMax, int exp, int nextLevelExp, int defense,
                                Evidyon::AvatarRace avatar_race,
                                Evidyon::AvatarClass avatar_class){
  using namespace ::Evidyon::Network::Packet;

  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE +
                        sizeof(unsigned char) * 7 +
                        sizeof(int) * 7, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
    packetBuffer = (char*)packet->data;
    const char* packetBufferEnd = packetBuffer + packet->dataLength;

    // Write the packet
    packetBuffer = encodeMessageType(NETMSG_TELLYOURAVATARSTATS, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(level, packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(abilities[0], packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(abilities[1], packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(abilities[2], packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(abilities[3], packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(abilities[4], packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<unsigned char>(availableAbilityPoints, packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned short>(hp,       packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned short>(hpMax,    packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned short>(mp,       packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned short>(mpMax,    packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(exp,      packetBuffer, packetBufferEnd);
    packetBuffer = encodeStructure<int>(nextLevelExp, packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<int, unsigned short>(defense,    packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<AvatarRace, unsigned char>(avatar_race, packetBuffer, packetBufferEnd);
    packetBuffer = encodePackedStructure<AvatarClass, unsigned char>(avatar_class, packetBuffer, packetBufferEnd);

    // Make sure the packet is only as big as necessary
    if (packetBuffer)
      enet_packet_resize(packet, packetBuffer - (char*)packet->data);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during tellYourAvatarStats"))
  {
    // Free the memory for the packet
    enet_packet_destroy(packet);

    // Fail
    return 0;
  }

  // Success
  return packet;
}



/*
//------------------------------------------------------------------------------------------------
// Name:  tellCurrentTrade
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* tellCurrentTrade(
  int trade_key,
  unsigned char* your_trade_item_indices,
  unsigned int number_of_items,
  AvatarInventoryItem* other_inventory[AVATAR_INVENTORY_SIZE],  // parsed into a list of item-descriptors (doesn't send other inventory)
  unsigned char* other_trade_item_indices,
  unsigned int other_number_of_items) {
  using namespace ::Evidyon::Network::Packet;

  // Check parameters
  CONFIRM((!number_of_items || your_trade_item_indices) &&
       (!other_number_of_items || (other_inventory && other_trade_item_indices))) else return 0;

  // Get data from the packet
  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true, NETMSG_TELLCURRENTTRADE,
           sizeof(trade_key) +
           sizeof(unsigned char) +
             sizeof(unsigned char) * number_of_items +
           sizeof(unsigned char) +
             sizeof(TradeItem) * other_number_of_items, &packet, &buffer, &buffer_end))
    return false;

  // Write the trade key
  buffer = encodeStructure<int>(trade_key, buffer, buffer_end);

  {   // Write in the client's items
    unsigned char unsigned_char_to_write = number_of_items;
    buffer = encodeStructure<unsigned char>(unsigned_char_to_write, buffer, buffer_end);
    for (unsigned int i = 0; buffer && i < number_of_items; ++i)
      buffer = encodeStructure<unsigned char>(your_trade_item_indices[i], buffer, buffer_end);
  }

  {   // Write in the partner's items
    unsigned char unsigned_char_to_write = other_number_of_items;
    buffer = encodeStructure<unsigned char>(unsigned_char_to_write, buffer, buffer_end);
    for (unsigned int i = 0; buffer && i < other_number_of_items; ++i)
    {
      unsigned int other_item_index = other_trade_item_indices[i];
      CONFIRM(other_item_index < AVATAR_INVENTORY_SIZE) else continue;
      AvatarInventoryItem* source_item = other_inventory[other_item_index];

      TradeItem item_to_write;
      item_to_write.type   = source_item->type;
      item_to_write.quantity = source_item->quantity;

      buffer = encodeStructure<TradeItem>(item_to_write, buffer, buffer_end);
    }
  }

  // Make sure the packet was created successfully
  if (!validateAndTrimPacket(packet, buffer)) {
    return false;
  }

  // Exit
  return packet;
}


//------------------------------------------------------------------------------------------------
// Name:  finishedTrading
// Desc:  
//------------------------------------------------------------------------------------------------
ENetPacket* finishedTrading() {
  return encodeMessageOnlyPacket(NETMSG_FINISHEDTRADING);
}*/

//
////------------------------------------------------------------------------------------------------
//ENetPacket* tellYourMoney(unsigned int money) {
//  using namespace ::Evidyon::Network::Packet;
//
//  ENetPacket* packet;
//  char* buffer;
//  const char* buffer_end;
//  if (!createPacket(true,
//          NETMSG_TELLYOURMONEY,
//          sizeof(unsigned int),
//          &packet,
//          &buffer,
//          &buffer_end)) {
//    return 0;
//  }
//
//  buffer = encodeStructure<unsigned int>(money, buffer, buffer_end);
//
//  if (!validateAndTrimPacket(packet, buffer)) return NULL;
//  return packet;
//}


/*
//------------------------------------------------------------------------------------------------
ENetPacket* bazaar_tellItemsForSale(ItemForTrade* items, unsigned int number_of_items)  {
  CONFIRM(items != NULL && number_of_items < 255) else return NULL;
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_BAZAAR_TELLITEMSFORSALE,
          sizeof(unsigned char) + sizeof(ItemForTrade) * number_of_items,
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  unsigned char char_number_of_items = number_of_items;
  buffer = encodeArray<ItemForTrade,unsigned char>(items,
                           char_number_of_items,
                           buffer,
                           buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}
*/

//
////------------------------------------------------------------------------------------------------
//ENetPacket* bazaar_tellSellerStatus(ItemForTrade* items, unsigned int number_of_items) {
//  CONFIRM(items != NULL && number_of_items < 255) else return NULL;
//  using namespace ::Evidyon::Network::Packet;
//
//  ENetPacket* packet;
//  char* buffer;
//  const char* buffer_end;
//  if (!createPacket(true,
//          NETMSG_BAZAAR_TELLSELLERSTATUS,
//          sizeof(unsigned char) + sizeof(ItemForTrade) * number_of_items,
//          &packet,
//          &buffer,
//          &buffer_end)) {
//    return 0;
//  }
//
//  unsigned char char_number_of_items = number_of_items;
//  buffer = encodeArray<ItemForTrade,unsigned char>(items,
//                           char_number_of_items,
//                           buffer,
//                           buffer_end);
//
//  if (!validateAndTrimPacket(packet, buffer)) return NULL;
//  return packet;
//}
//
//
////------------------------------------------------------------------------------------------------
//ENetPacket* bazaar_removeItemListings(unsigned int* item_listings, unsigned int number_of_items) {
//  CONFIRM(item_listings != NULL && number_of_items < 255) else return NULL;
//  using namespace ::Evidyon::Network::Packet;
//
//  ENetPacket* packet;
//  char* buffer;
//  const char* buffer_end;
//  if (!createPacket(true,
//          NETMSG_BAZAAR_REMOVEITEMLISTINGS,
//          sizeof(unsigned char) + sizeof(unsigned int) * number_of_items,
//          &packet,
//          &buffer,
//          &buffer_end)) {
//    return 0;
//  }
//
//  unsigned char char_number_of_items = number_of_items;
//  buffer = encodeArray<unsigned int,unsigned char>(item_listings,
//                           char_number_of_items,
//                           buffer,
//                           buffer_end);
//
//  if (!validateAndTrimPacket(packet, buffer)) return NULL;
//  return packet;
//}


ENetPacket* changeMap(int map_id) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_CHANGEMAP,
          sizeof(int),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<int>(map_id, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;

}


ENetPacket* eraseActorFromWorld(int actor_id) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_ERASEACTORFROMWORLD,
          sizeof(int),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<int>(actor_id, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}




ENetPacket* tellSingleActorFullDescription(const ActorDescriptionData& description, const char* name) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_TELLACTORFULLDESCRIPTIONS,
          sizeof(unsigned char) +
            sizeof(ActorDescriptionData) +
            NETWORK_STRING_PADDING_BYTES + Evidyon::MAX_AVATAR_NAME_LENGTH,
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  TellActorFullDescriptionsPacketHeader header = {1};
  buffer = encodeStructure<TellActorFullDescriptionsPacketHeader>(header, buffer, buffer_end);
  buffer = encodeStructure<ActorDescriptionData>(description, buffer, buffer_end);
  buffer = encodeString(name, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}



ENetPacket* fastSync(const ::Evidyon::Network::Packet::FastSyncPacket& data) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_FASTSYNC,
          sizeof(FastSyncPacket),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<FastSyncPacket>(data, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}

ENetPacket* createMagicFX(int type, int src_actor_id, float src_x, float src_y,
              int power, int* target_ids, int number_of_targets) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_CREATEMAGICFX,
          sizeof(int) * (3 + number_of_targets) + sizeof(float)*2,
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<int>(type, buffer, buffer_end);
  buffer = encodeStructure<int>(src_actor_id, buffer, buffer_end);
  buffer = encodeStructure<float>(src_x, buffer, buffer_end);
  buffer = encodeStructure<float>(src_y, buffer, buffer_end);
  buffer = encodeStructure<unsigned char>((unsigned char)power, buffer, buffer_end);

  unsigned char byte_size =
    static_cast<unsigned char>(number_of_targets);
  buffer = encodeArray<int, unsigned char>(target_ids,
                                           byte_size,
                                           buffer,
                                           buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}



ENetPacket* tellItemsOnMapLocation(unsigned int world_x,
                                   unsigned int world_y,
                                   unsigned short* item_types,
                                   int number_of_item_types) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_TELLITEMSONMAPLOCATION,
          sizeof(unsigned int) +
          sizeof(unsigned char) +
          sizeof(unsigned short) * (number_of_item_types),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  // pack the coordinates
  buffer = encodeStructure<unsigned int>(((world_x & 0xFFFF) << 16) | (world_y & 0xFFFF),
                                         buffer,
                                         buffer_end);

  // save the items here
  unsigned char byte_size =
    static_cast<unsigned char>(number_of_item_types);
  buffer = encodeArray<unsigned short, unsigned char>(item_types,
                                                      byte_size,
                                                      buffer,
                                                      buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}



ENetPacket* updateLocalWorldRegion(World::RegionID region_id) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_UPDATELOCALWORLDREIGON,
          sizeof(World::RegionID),
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<World::RegionID>(region_id,
                                            buffer,
                                            buffer_end);


  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}




ENetPacket* updateInventory(bool complete_update,
                            AvatarInventoryItem* inventory,
                            int* inventory_indices,
                            int number_of_indices) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          complete_update ? NETMSG_TELLINVENTORY : NETMSG_UPDATEINVENTORY,
          sizeof(unsigned char) + sizeof(ClientAvatarInventoryItem) * number_of_indices,
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  // pack the size
  CONFIRM(number_of_indices <= AVATAR_INVENTORY_SIZE) else return NULL;
  unsigned char byte_number_of_indices = (unsigned char)number_of_indices;
  buffer = encodeStructure<unsigned char>(byte_number_of_indices,
                                          buffer,
                                          buffer_end);

  // add each entry
  for (int i = 0; i < number_of_indices; ++i) {
    int index = inventory_indices[i];
    CONFIRM(index >= 0 && index < AVATAR_INVENTORY_SIZE) else return false;
    AvatarInventoryItem* inventory_item = &inventory[index];
    Item* item = inventory_item->item;
    ClientAvatarInventoryItem item_description;
    item_description.equipped = inventory_item->equipped;
    item_description.inventory_index = (unsigned)index;

    if (item != NULL) {
      item_description.quantity = item->getQuantity();
      item_description.type = item->getDescription()->type_index;
    } else {
      item_description.quantity = 0;
      item_description.type = (unsigned)(~0L);
    }

    buffer = encodeStructure<ClientAvatarInventoryItem>(item_description,
                                                        buffer,
                                                        buffer_end);
  }


  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
                                  
}

ENetPacket* tellActorName(ActorID actor_id,
                          const char* name) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
          NETMSG_TELLACTORNAME,
          sizeof(actor_id) + NETWORK_STRING_PADDING_BYTES + MAX_AVATAR_NAME_LENGTH,
          &packet,
          &buffer,
          &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<unsigned long>(actor_id,
                                          buffer,
                                          buffer_end);
  buffer = encodeString(name, buffer, buffer_end);


  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}

ENetPacket* storageTell(ItemInStorage* storage, int number_of_items) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
                    NETMSG_STORAGE_TELL,
                    sizeof(unsigned char) + sizeof(ItemInStorage) * number_of_items,
                    &packet,
                    &buffer,
                    &buffer_end)) {
    return 0;
  }

  unsigned char byte_number_of_items = (unsigned char)number_of_items;
  buffer = encodeArray<ItemInStorage, unsigned char>(storage,
                                                     byte_number_of_items,
                                                     buffer,
                                                     buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}


ENetPacket* geosidTellGeonite(unsigned int geonite) {
  using namespace ::Evidyon::Network::Packet;

  ENetPacket* packet;
  char* buffer;
  const char* buffer_end;
  if (!createPacket(true,
                    NETMSG_GEOSID_TELLGEONITE,
                    sizeof(unsigned int),
                    &packet,
                    &buffer,
                    &buffer_end)) {
    return 0;
  }

  buffer = encodeStructure<unsigned int>(geonite, buffer, buffer_end);

  if (!validateAndTrimPacket(packet, buffer)) return NULL;
  return packet;
}



}
}
}
}
}
