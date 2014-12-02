#ifndef __ACTORID_H__
#define __ACTORID_H__


namespace Evidyon {

typedef unsigned int ActorID;

static const int MAX_ACTOR_DESCRIPTION_TEMPLATES_BITS = 12;
static const int MAX_ACTOR_DESCRIPTION_TEMPLATES = 1<<MAX_ACTOR_DESCRIPTION_TEMPLATES_BITS;

static const int MAX_ACTORS_IN_WORLD_BITS = 11; // 2^* max simultaneous actors
static const int MAX_ACTORS_IN_WORLD = 1<<MAX_ACTORS_IN_WORLD_BITS;

//#if (MAX_ACTORS_IN_WORLD_BITS + MAX_ACTOR_DESCRIPTION_TEMPLATES_BITS >= (sizeof(unsigned int)*8))
//#error The number of actor description templates and max actors in world is too high!
//#endif

// used by the server to create a unique actor ID
ActorID PackActorID(unsigned int incremented_value,             // just some number
                    unsigned int actor_structure_mempool_index, // must be < MAX_ACTORS_IN_WORLD
                    unsigned int description_template);         // must be < MAX_ACTOR_DESCRIPTION_TEMPLATES

// used by the client to extract the actor's basic description from its ID
void UnpackActorID(ActorID actor_id,
                   unsigned int* description_template);

}


#endif
