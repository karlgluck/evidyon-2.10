#ifndef __GLOBALLIFEFORMAIMANAGER_H__
#define __GLOBALLIFEFORMAIMANAGER_H__

#include "actorpointer.h"
#include "normalmeleemonsterlifeformaimanager.h"
#include "interactivenpclifeformaimanager.h"

struct _iobuf;
typedef struct _iobuf FILE;

namespace Evidyon {

struct LifeformAIDescription;

class GlobalLifeformAIManager {
public:
  static GlobalLifeformAIManager* singleton();

public:
  GlobalLifeformAIManager();
  bool create(FILE* game_file);
  void destroy();
  void update(double time,
              double time_since_last_update);

  void releaseWorldRegion(WorldRegion* region);

  bool acquire(int spawn_x,
               int spawn_y, // world coordinates of spawn point
               Map* map, // map in which to place the lifeform
               GlobalActorManager* actor_manager,
               size_t lifeform_ai_type,
               int level,
               unsigned int treasure_randomizer_mask,
               bool delay_activity,
               ActorPointer* acquired_actor); // output if returns 'true'

private:
  LifeformAIDescription* descriptions_;
  size_t number_of_descriptions_;
  NormalMeleeMonsterLifeformAIManager normal_melee_monsters_;
  InteractiveNPCLifeformAIManager interactive_npcs_;

private:
  static GlobalLifeformAIManager* singleton_;
};


/*


editor has two spawn type definitions:
  - color maps to define the swarming spawn types in regions across the world
      color -> AI lifeform type
  - map to define spawn density
      color -> density
  - maps to define monster levels:  colors define a different level
      color -> level

editor sticks these together to generate a very big list of locations mapped
to spawn point types.  it then collects these by world region, and builds
region-specific spawning lists.  this is a big vector of:
{
  int x, y;
  unsigned int region_spawn_type_mask; // sets bits of the local types that can be spawned here
}

the region spawn type index is local to a given region. since a region may not
have very many different kinds of spawn, it will (hopefully) be sufficient to
make a vector of 32:
{
  int lifeform_ai_type;
  int level;
  unsigned int spawn_counter_reset;  // add ~((spawn_counter)>>31) each update--i.e. add 1 when >= 0
  unsigned int reset_period; // when reset == reset_period, spawn_counter -= granularity
  int spawn_counter;  // increment by 1 when spawning a lifeform of this type (only when <= 0)
                      // decrement by spawn_counter_granularity each update,
  unsigned int spawn_counter_granularity;
}






- client enters region that previously had no clients in it
  - client.cpp invokes check method
- is this region's spawner activatable?
  - map has a std::multimap of <world region id, spawn_point> defining which
    spawn points are active in a region
  - for each entry in multimap, let the point know that it just became active
      bool force_spawn - this region became active for the very first time
                         (it had expired in the past and been removed, or
                         simply nobody had visited it yet)
      only  spawn if the internal counter <= 0
  - spawner has a vector-queue of spawn points that is iterated through every
    5-10 seconds and decerements their counters. this is a ring list.  the
    the points are erased when the counters are <= 0.


spawnable locations mask for the whole map: bit = 1 if a monster can be
spawned on a location. grouped by world region.

each spawner with a density




*/





}



#endif