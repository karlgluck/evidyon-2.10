#ifndef __ACTORSYNC_H__
#define __ACTORSYNC_H__

#include "../actorid.h"

namespace Evidyon {

static const int ACTORSYNC_ANGLE_BITS = 7;

namespace Network {
namespace Packet {

#pragma pack( push, 1 )

struct ActorSyncData {
  ActorID id;
  unsigned __int16 x_packed, y_packed;
  struct Flags {
    unsigned moving     : 1;    // boolean
    unsigned teleport   : 1;    // 0- walk/swim/run/whatev.  1- jump instantly
    unsigned mesh_state : 2;    // 0- dead, 1- normal, 2- combat, 3- dont render mesh
    unsigned action     : 4;    // 0- nothing
    unsigned blood      : 1;    // boolean; 1 if this actor was hit by a weapon
    unsigned angle      : 7;    // if moving=0, turn to pi/380/(1<<ACTORSYNC_ANGLE_BITS)*angle
  } flags;
};

#pragma pack( pop )

}
}
}



#endif