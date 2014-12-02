#ifndef __SLOWSYNC_H__
#define __SLOWSYNC_H__

#include "../networkdecl.h"

namespace Evidyon {
namespace Network {
namespace Packet {

#pragma pack( push, 1 )

struct SlowSyncPacketHeader {
  float time_since_last_sync;
  unsigned char actors;
};

#pragma pack( pop )

}
}
}



#endif