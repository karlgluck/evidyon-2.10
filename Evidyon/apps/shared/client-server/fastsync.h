#ifndef __FASTSYNC_H__
#define __FASTSYNC_H__

#include "../networkdecl.h"
#include "actorsync.h"

namespace Evidyon {
namespace Network {
namespace Packet {

#pragma pack( push, 1 )

struct FastSyncPacket {
  // This flag is 1 if stat_value is HP.  For 1, the client should reset
  // a counter that is incremented each time a fast sync is received.
  unsigned stat_type_reset : 1;
  // Depending on the value of the client's counter, this value is:
  //  0, 2 - %hp  * (2^7-1)
  //  1, 3 - %mp  * (2^7-1)
  //  4    - %exp * (2^7-1)
  unsigned stat_value : 7;
  float time_since_last_sync;
  unsigned __int16 x_packed, y_packed;
  ActorSyncData::Flags sync_flags;
};

#pragma pack( pop )

}
}
}



#endif