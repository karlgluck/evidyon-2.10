#ifndef __TELLACTORFULLDESCRIPTIONS_ENCODER_H__
#define __TELLACTORFULLDESCRIPTIONS_ENCODER_H__

#include "../../shared/networkdecl.h"
#include "../../shared/client-server/packetencoder.h"

namespace Evidyon {
class Actor;

namespace Network {
namespace Packet {

struct ActorDescriptionData;

class TellActorFullDescriptionsPacketEncoder : public PacketEncoder {
public:
  // initializes the encoder
  TellActorFullDescriptionsPacketEncoder();
  void reset();

  // will flush (send & renew) packet if full
  //void add(ActorDescriptionData* actor_data, const char* name, NetworkPeer* destination);
  void add(ActorDescriptionData* actor_data,
           NetworkPeer* destination);

  // sends the packet if it has data
  void dispatch(NetworkPeer* destination);

private:
  unsigned int actors_;
};

}
}
}



#endif