#include "packetencoder.h"
#include "../../common/null.h"

namespace Evidyon {
namespace Network {


PacketEncoder::PacketEncoder() {
  packet_ = NULL;
  next_available_bytes_ = NULL;
  bytes_available_ = 0;
}

PacketEncoder::~PacketEncoder() {
  if (packet_) {
    enet_packet_destroy(packet_);
    packet_ = NULL;
  }
}

void PacketEncoder::allocate(bool reliable, int header_bytes) {
  if (packet_) {
    enet_packet_destroy(packet_);
  }
  enet_uint32 flags = reliable ? ENET_PACKET_FLAG_RELIABLE :
                                 ENET_PACKET_FLAG_UNSEQUENCED;
  packet_ = enet_packet_create(0, MAX_PACKET_BYTES, flags);
  header_bytes += Evidyon::Network::Packet::NETWORK_MESSAGE_SIZE;
  next_available_bytes_ = packet_->data + header_bytes;
  bytes_available_ = packet_->dataLength - header_bytes;
}

void* PacketEncoder::reserve(size_t bytes) {
  if (bytes_available_ < bytes) {
    return NULL;
  } else {
    void* reserved_space = next_available_bytes_;
    next_available_bytes_ = ((unsigned char*)next_available_bytes_) + bytes;
    bytes_available_ -= bytes;
    return reserved_space;
  }
}

NetworkPacket* PacketEncoder::compile(Packet::Message message_type,
                                      const void* header,
                                      size_t header_bytes) {
  NetworkPacket* packet = packet_;
  if (packet) {
    using namespace Evidyon::Network::Packet;
    char* buffer = encodeMessageType(message_type,
                                     (char*)(packet->data),
                                     (const char*)(packet->data + packet->dataLength));
    memcpy(buffer, header, header_bytes);
    size_t bytes_used = (reinterpret_cast<enet_uint8*>(next_available_bytes_)) - packet->data;
    enet_packet_resize(packet, bytes_used);
  }
  packet_ = NULL;
  bytes_available_ = 0;
  next_available_bytes_ = NULL;
  return packet;
}

}
}