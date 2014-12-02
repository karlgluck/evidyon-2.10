#ifndef __PACKETENCODER_H__
#define __PACKETENCODER_H__

#include "packets.h"
#include "../networkdecl.h"

namespace Evidyon {
namespace Network {

// Derive from this class to gain access to methods that help build network
// packets for which all of the data that it needs to hold is not immediately
// available.
class PacketEncoder {
  const static int MAX_PACKET_BYTES = 512;

public:
  PacketEncoder();
  ~PacketEncoder();

protected:
  // Creates a packet from which data can be allocated.  The header is a
  // special region reserved at the start of building a packet and filled
  // when the packet is compiled.
  // In this way the header can contain information, such as the number
  // of entries in the packet, that isn't known until the packet is built.
  void allocate(bool reliable, int header_bytes);

  // Allocates 'bytes' from the payload of the current packet.  If the packet
  // is full, this method returns NULL.  In this case, 'allocate'
  void* reserve(size_t bytes);

  // Finalizes the packet and returns it.  The caller now has responsibility
  // for the packet, and reserve() will fail unil allocate() is called.
  NetworkPacket* compile(Packet::Message message_type,
                         const void* header,
                         size_t header_bytes);

private:
  NetworkPacket* packet_;
  void* next_available_bytes_;
  size_t bytes_available_;
};

}
}

#endif