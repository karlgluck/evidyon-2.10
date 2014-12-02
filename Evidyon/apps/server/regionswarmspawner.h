#ifndef __REGIONSWARMSPAWNER_H__
#define __REGIONSWARMSPAWNER_H__


#include "../shared/server-editor/swarmspawndescription.h"
#include "actorpointer.h"

namespace Evidyon {

class WorldRegion;
class GlobalLifeformAIManager;
  
struct RegionSwarmType {
  CompiledRegionSwarmSpawn description;
  unsigned int activation_counter;
  int spawn_counter;
};

class RegionSwarmSpawner {
public:

  // a spawner is initialized by the game file manager
  bool create(unsigned int treasure_randomizer_mask,
              const CompiledRegionSwarmSpawn* swarms,
              size_t number_of_swarms,
              const CompiledSwarmSpawnLocationInfo* locations);
  void destroy();

  // When a spawner's associated world region is acquired, this method sets
  // up the spawner.
  void activate(WorldRegion* local_region);

  // If the parameter matches the internal region, this method clears the
  // internal reference and returns 'true'.
  bool deactivateIfRegionIs(WorldRegion* local_region);

  // spawns monsters.  should only be called when this region is active.
  void update(GlobalLifeformAIManager* lifeform_ai_manager);


private:
  WorldRegion* region_; // the part of the world into which this spawner spawns

  RegionSwarmType swarms_[MAX_SWARM_SPAWNS_PER_WORLD_REGION];
  int number_of_swarms_;
  CompiledSwarmSpawnLocationInfo spawn_locations_[MAX_SWARM_SPAWNS_PER_WORLD_REGION];
  ActorPointer spawned_actors_[MAX_SWARM_SPAWNS_PER_WORLD_REGION];

  bool just_activated_; // 'true' between activate() and the first update()
  unsigned int treasure_randomizer_mask_;
};

}


#endif