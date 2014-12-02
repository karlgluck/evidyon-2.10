#include "tellactorfulldescriptions-encoder.h"
#include "../../shared/client-server/tellactorfulldescriptions.h"
#include "../../shared/client-server/actordescription.h"
#include "../globalnetworkmanager.h"
#include "../../shared/client-server/packets.h"

namespace Evidyon {
namespace Network {
namespace Packet {

TellActorFullDescriptionsPacketEncoder::TellActorFullDescriptionsPacketEncoder() {
  actors_ = 0;
}

void TellActorFullDescriptionsPacketEncoder::reset() {
  actors_ = 0;
}

//void TellActorFullDescriptionsPacketEncoder::add(ActorDescriptionData* actor_data, const char* name, NetworkPeer* destination) {
//void TellActorFullDescriptionsPacketEncoder::add(ActorDescriptionData* actor_data, NetworkPeer* destination) {
//  size_t name_length = strlen(name);
//  size_t description_bytes = sizeof(ActorDescriptionData) +
//               NETWORK_STRING_PADDING_BYTES + name_length;
//
//  void* buffer = reserve(description_bytes);
//
//  if (!buffer) { // renew the packet if there isn't room in the buffer
//    dispatch(destination);
//    allocate(false, sizeof(TellActorFullDescriptionsPacketHeader));
//    buffer = reserve(description_bytes);
//  }
//
//  if (buffer) { // copy actor update into the packet payload
//    memcpy(buffer, actor_data, sizeof(ActorDescriptionData));
//    char* name_buffer = ((char*)buffer) + sizeof(ActorDescriptionData);
//    const char* name_buffer_end = ((char*)buffer) + description_bytes;
//    encodeString(name, name_buffer, name_buffer_end);
//    ++actors_;
//  }
//}
void TellActorFullDescriptionsPacketEncoder::add(ActorDescriptionData* actor_data, NetworkPeer* destination) {
  void* buffer = reserve(sizeof(ActorDescriptionData));

  if (!buffer) { // renew the packet if there isn't room in the buffer
    dispatch(destination);
    allocate(false, sizeof(TellActorFullDescriptionsPacketHeader));
    buffer = reserve(sizeof(ActorDescriptionData));
  }

  if (buffer) { // copy actor update into the packet payload
    memcpy(buffer, actor_data, sizeof(ActorDescriptionData));
    ++actors_;
  }
}

void TellActorFullDescriptionsPacketEncoder::dispatch(NetworkPeer* destination) {
  if (actors_ > 0) {
  TellActorFullDescriptionsPacketHeader header = { actors_ };
  NetworkPacket* packet = compile(NETMSG_TELLACTORFULLDESCRIPTIONS, &header, sizeof(header));
  GlobalNetworkManager::sendAsyncPacket(destination, packet);
  actors_ = 0;
  }
}


}
}
}
