#ifndef __INSTANTIATEDMAGIC_H__
#define __INSTANTIATEDMAGIC_H__

#include "../shared/server-editor/magicdescription.h"
#include "magic.h"
#include "actorpointer.h"

namespace Evidyon {

class Actor;

// Once magic has been validated and casting begins, this structure defines
// how the spell progresses.
struct InstantiatedMagic {
  ActorPointer caster; // Actor that created this magic
  Magic magic;  // Original casting structure
  int references; // A reference-counter; when this is 0, get rid of the instance

  // Passed to each visualization of this magic effect to make it reflect
  // the power of the caster.
  int visualization_power_parameter;

  // Definition of each stage's attributes as calculated by the casting actor
  MagicDescriptionStageParameters stage_parameters[MagicDescription::NUM_STAGES];
};


}

#endif
