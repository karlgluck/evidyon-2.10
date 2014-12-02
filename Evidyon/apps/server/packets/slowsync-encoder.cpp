#include "slowsync-encoder.h"
#include "../../shared/client-server/actorsync.h"
#include "../../shared/client-server/slowsync.h"
#include "../globalnetworkmanager.h"

namespace Evidyon {
namespace Network {
namespace Packet {

SlowSyncPacketEncoder::SlowSyncPacketEncoder() {
  actors_ = 0;
  time_since_last_sync_ = 0.0f;
}

void SlowSyncPacketEncoder::reset() {
  actors_ = 0;
}

void SlowSyncPacketEncoder::setTimeSinceLastSync(double time_since_last_sync) {
  time_since_last_sync_ = time_since_last_sync;
}

void SlowSyncPacketEncoder::add(ActorSyncData* actor_data, NetworkPeer* destination) {
  void* buffer = reserve(sizeof(ActorSyncData));

  if (!buffer) { // renew the packet if there isn't room in the buffer
    dispatch(destination);
    allocate(true, sizeof(SlowSyncPacketHeader));
    buffer = reserve(sizeof(ActorSyncData));
  }

  if (buffer) { // copy actor update into the packet payload
    memcpy(buffer, actor_data, sizeof(ActorSyncData));
    ++actors_;
  }
}

void SlowSyncPacketEncoder::dispatch(NetworkPeer* destination) {
  if (actors_ > 0) {
    SlowSyncPacketHeader header = { time_since_last_sync_, actors_ };
    NetworkPacket* packet = compile(NETMSG_SLOWSYNC, &header, sizeof(header));
    GlobalNetworkManager::sendAsyncPacket(destination, packet);
    actors_ = 0;
  }
}


}
}
}
