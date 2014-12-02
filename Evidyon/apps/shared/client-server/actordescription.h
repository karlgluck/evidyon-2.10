#ifndef __ACTORDESCRIPTION_H__
#define __ACTORDESCRIPTION_H__

#include "actorsync.h"
#include "../evidyon_actor.h"

namespace Evidyon {

#if (CORPOREALSTATE_COUNT >= (1<<3))
#error too many corporeal states to fit into actor state description
#endif
  
#pragma pack( push, 1 )

struct ActorStateDescriptionData {
  unsigned actor_type      : 12;
  unsigned corporeal_state :  3;
  unsigned enchantments    : 16;
  unsigned short equipped_items[ACTORATTACHMENTPOINT_COUNT];
};

#pragma pack( pop )

namespace Network {
namespace Packet {

#pragma pack( push, 1 )

// this structure is followed by the actor's name string in a full description
struct ActorDescriptionData {
  ActorSyncData sync_;
  ActorStateDescriptionData state_;
};

#pragma pack( pop )

}
}
}


#endif