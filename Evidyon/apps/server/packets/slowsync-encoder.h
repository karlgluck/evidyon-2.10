#ifndef __SLOWSYNC_ENCODER_H__
#define __SLOWSYNC_ENCODER_H__

#include "../../shared/networkdecl.h"
#include "../../shared/client-server/packetencoder.h"

namespace Evidyon {
namespace Network {
namespace Packet {

struct ActorSyncData;

class SlowSyncPacketEncoder : public PacketEncoder {
public:
  // initializes the encoder
  SlowSyncPacketEncoder();
  void reset();
  void setTimeSinceLastSync(double time_since_last_sync);

  // will flush (send & renew) packet if full
  void add(ActorSyncData* actor_data, NetworkPeer* destination);

  // sends the packet if it has data
  void dispatch(NetworkPeer* destination);

private:
  unsigned int actors_;
  float time_since_last_sync_;
};

}
}
}



#endif