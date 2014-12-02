#ifndef __TELLACTORFULLDESCRIPTIONS_H__
#define __TELLACTORFULLDESCRIPTIONS_H__


namespace Evidyon {
namespace Network {
namespace Packet {

#pragma pack( push, 1 )

struct TellActorFullDescriptionsPacketHeader {
  unsigned char actors;
};

#pragma pack( pop )

}
}
}



#endif