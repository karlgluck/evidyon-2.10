#ifndef __SWARMSPAWNDESCRIPTION_H__
#define __SWARMSPAWNDESCRIPTION_H__



namespace Evidyon {

// Always <= 32 because there are only 32 bits in the spawn location mask
static const unsigned int MAX_SWARM_SPAWNS_PER_WORLD_REGION = 32;

struct CompiledRegionSwarmSpawn {
  int ai_lifeform_type;

  // This is based on the average level of monsters in this region.  It has
  // N paired bits (N <= 16) in the high and low WORDs set to 0.  The random
  // treasure generation value is ANDed with this mask before checking to
  // see how many bits match.
  // By setting N higher, treasure is more more valuable on average.
  unsigned int treasure_randomizer_mask;

  // count from 0->activate period, then:
  //  if spawn counter >= 0, decrement spawn counter by granularity
  //  if spawn counter < 0
  //      spawn a monster
  //      spawn counter++
  //      if spawn counter == 0, spawn counter = spawn_counter_reset_value
  unsigned int spawn_counter_activate_period;
  unsigned int spawn_counter_granularity;
  unsigned int spawn_counter_reset_value;

  // bit set to 1 if this swarm type can create an actor at
  // the corresponding location
  unsigned int spawn_location_mask;
};


struct CompiledSwarmSpawnLocationInfo {
  int x, y; // location at which to spawn
  int level;// level of monster to spawn here
};


extern const float SWARM_SPAWN_PERIOD;

}




#endif

