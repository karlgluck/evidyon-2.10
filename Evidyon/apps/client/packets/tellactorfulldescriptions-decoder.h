#ifndef __TELLACTORFULLDESCRIPTIONS_DECODER_H__
#define __TELLACTORFULLDESCRIPTIONS_DECODER_H__

#include "../../shared/networkdecl.h"
#include "../../shared/client-server/packetencoder.h"
#include "../../shared/client-server/actordescription.h"
#include "../../shared/evidyon_avatar.h"

namespace Evidyon {
namespace Network {
namespace Packet {

struct DecodedFullActorDescription {
  ActorDescriptionData data;
  std::string name;
};

class TellActorFullDescriptionsPacketDecoder {
public:
  TellActorFullDescriptionsPacketDecoder(NetworkPacket* packet);
  bool read(DecodedFullActorDescription* actor_data);

private:
  unsigned int actors_;
  const char* buffer_;
  const char* buffer_end_;
};

}
}
}



#endif