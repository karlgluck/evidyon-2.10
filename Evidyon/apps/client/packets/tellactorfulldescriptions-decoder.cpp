#include "tellactorfulldescriptions-decoder.h"
#include "../../shared/client-server/tellactorfulldescriptions.h"
#include "../../shared/client-server/actordescription.h"
#include "../../shared/client-server/packets.h"
#include "../../common/enet/enet.h"
#include "../../shared/client-server/packets.h"
#include <dc/debug>

namespace Evidyon {
namespace Network {
namespace Packet {

TellActorFullDescriptionsPacketDecoder::TellActorFullDescriptionsPacketDecoder(NetworkPacket* packet) {
  buffer_ = reinterpret_cast<const char*>(packet->data);
  buffer_end_ = reinterpret_cast<const char*>(packet->data + packet->dataLength);
  TellActorFullDescriptionsPacketHeader header;
  Message message;
  buffer_ = decodeMessageType(buffer_, buffer_end_, &message);
  CONFIRM(message == NETMSG_TELLACTORFULLDESCRIPTIONS);
  buffer_ = decodeStructure<TellActorFullDescriptionsPacketHeader>(buffer_, buffer_end_, &header);
  actors_ = header.actors;
}


bool TellActorFullDescriptionsPacketDecoder::read(DecodedFullActorDescription* actor_data) {
  if (actors_ <= 0) return false;
  actors_--;
  buffer_ = decodeStructure<ActorDescriptionData>(buffer_, buffer_end_, &actor_data->data);
  actor_data->name.clear();
  //buffer_ = decodeString(buffer_, buffer_end_, &actor_data->name);
  return buffer_ != NULL;
}


}
}
}
