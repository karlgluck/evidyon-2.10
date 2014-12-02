#include "actorid.h"


namespace Evidyon {

ActorID PackActorID(unsigned int incremented_value,
                    unsigned int actor_structure_mempool_index,
                    unsigned int description_template) {
  description_template |= actor_structure_mempool_index << MAX_ACTOR_DESCRIPTION_TEMPLATES_BITS;
  description_template |= incremented_value << (MAX_ACTOR_DESCRIPTION_TEMPLATES_BITS +
                                                MAX_ACTORS_IN_WORLD_BITS);
  return static_cast<ActorID>(description_template);
}

void UnpackActorID(ActorID actor_id,
                   unsigned int* description_template) {
  static const unsigned int TEMPLATE_MASK =
    ((~(unsigned)(0L))>>(sizeof(unsigned int)*8 - MAX_ACTOR_DESCRIPTION_TEMPLATES_BITS));
  *description_template = actor_id & TEMPLATE_MASK;
}

}

