#ifndef __MAGIC_H__
#define __MAGIC_H__

#include "actorpointer.h"

namespace Evidyon {

struct MagicDescription;
class Actor;

// Structure passed by an Actor to its MagicController when it wants to cast
// a spell.
struct Magic {
  const MagicDescription* description;
  ActorPointer target;
  float x, y;
};


}

#endif