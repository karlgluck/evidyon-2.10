#include "regionswarmspawner.h"
#include "globallifeformaimanager.h"
#include "globalactormanager.h"
#include <memory.h>
#include <dc/debug>
#include "globalworldmanager.h"
#include "../common/null.h"
#include "client.h"
#include "encodepackets.h"
#include "globalgamefilemanager.h"

namespace Evidyon {

bool RegionSwarmSpawner::create(unsigned int treasure_randomizer_mask,
                                const CompiledRegionSwarmSpawn* swarms,
                                size_t number_of_swarms,
                                const CompiledSwarmSpawnLocationInfo* locations) {
  region_ = NULL;
  treasure_randomizer_mask_ = treasure_randomizer_mask;

  for (int i = 0; i < number_of_swarms; ++i) {
    swarms_[i].description = swarms[i];
    swarms_[i].spawn_counter = swarms[i].spawn_counter_reset_value;
    swarms_[i].activation_counter = 0;
  }
  number_of_swarms_ = number_of_swarms;

  memcpy(spawn_locations_,
         locations,
         sizeof(spawn_locations_));

  for (int i = 0; i < MAX_SWARM_SPAWNS_PER_WORLD_REGION; ++i) {
    spawned_actors_[i].reset();
  }

  just_activated_ = false;

  return true;
}

void RegionSwarmSpawner::destroy() {
  region_ = NULL;
  memset(swarms_,
         0,
         sizeof(swarms_));
  memset(spawn_locations_,
         0,
         sizeof(spawn_locations_));

  for (int i = 0; i < MAX_SWARM_SPAWNS_PER_WORLD_REGION; ++i) {
    spawned_actors_[i].reset();
  }
}

void RegionSwarmSpawner::activate(WorldRegion* local_region) {
  region_ = local_region;
  just_activated_ = true;
}

bool RegionSwarmSpawner::deactivateIfRegionIs(WorldRegion* local_region) {
  if (region_ == local_region) {
    region_ = NULL;
    return true;
  } else {
    return false;
  }
}


void RegionSwarmSpawner::update(GlobalLifeformAIManager* lifeform_ai_manager) {

  if (just_activated_) { // spawn a complete set of monsters
    just_activated_ = false;
    bool spawned_full_compliment = true; // initialize to 'true' for regions with no swarms
    do {
      for (int i = 0; i < number_of_swarms_; ++i) {
        RegionSwarmType* swarm = &swarms_[i];

        // pick a location at which to spawn
        int location_index;
        unsigned int mask = swarm->description.spawn_location_mask;
        for (location_index = 0; location_index < MAX_SWARM_SPAWNS_PER_WORLD_REGION; ++location_index) {
          if (((mask & 1) == 1) && spawned_actors_[location_index].invalid()) break;
          mask >>= 1;
        }

        spawned_full_compliment = location_index >= MAX_SWARM_SPAWNS_PER_WORLD_REGION;
        if (spawned_full_compliment) break;

        // spawn a monster!
        bool spawned;
        spawned = lifeform_ai_manager->acquire(
            spawn_locations_[location_index].x,
            spawn_locations_[location_index].y,
            region_->getMap(),
            GlobalActorManager::singleton(),
            swarm->description.ai_lifeform_type,
            spawn_locations_[location_index].level,
            treasure_randomizer_mask_,
            false,
           &spawned_actors_[location_index]);

        // if we didn't spawn a monster, something is wrong...but we still
        // should press on with the program
        CONFIRM(spawned) else {
          spawned_full_compliment = true;
          break;
        }
      }
    } while (false == spawned_full_compliment);


  } else { // spawn normally
    for (int i = 0; i < number_of_swarms_; ++i) {
      RegionSwarmType* swarm = &swarms_[i];

      unsigned int activation_counter = swarm->activation_counter + 1;
      const unsigned int spawn_counter_activate_period = 
          swarm->description.spawn_counter_activate_period;
      if (activation_counter < spawn_counter_activate_period) {
        swarm->activation_counter = activation_counter;
        continue;
      }

      int spawn_counter = swarm->spawn_counter;
      bool spawned = false;
      if (spawn_counter < 0) {

        // pick a location at which to spawn
        int location_index;
        unsigned int mask = swarm->description.spawn_location_mask;
        for (location_index = 0; location_index < MAX_SWARM_SPAWNS_PER_WORLD_REGION; ++location_index) {
          if (((mask & 1) == 1) && spawned_actors_[location_index].invalid()) break;
          mask >>= 1;
        }

        if (location_index < MAX_SWARM_SPAWNS_PER_WORLD_REGION) {

          // spawn a monster!
          CompiledSwarmSpawnLocationInfo* spawned_location = &spawn_locations_[location_index];
          float x = spawned_location->x,
                y = spawned_location->y;
          spawned = lifeform_ai_manager->acquire(
              x,
              y,
              region_->getMap(),
              GlobalActorManager::singleton(),
              swarm->description.ai_lifeform_type,
              spawned_location->level,
              treasure_randomizer_mask_,
              true,
             &spawned_actors_[location_index]);

          // if we didn't spawn a monster, something is wrong.
          CONFIRM(spawned) else {
            activation_counter = 0;
            swarm->spawn_counter = swarm->description.spawn_counter_reset_value;
          }

          if (spawned) {
            using namespace Evidyon::Network::Packet::Server::Encode;
            NetworkPacket* packet = createMagicFX(GlobalGamefileManager::singleton()->getSpawnMagicFX(),
                                                  0, // source actor id number
                                                  x + 0.5f,
                                                  y + 0.5f,
                                                  0,
                                                  NULL,
                                                  0);
            region_->broadcastOrphanedNetworkPacket(packet);
          }

        } else {
          break; // reached full spawn
        }
      }

      if (spawned) {
        ++spawn_counter;
        swarm->spawn_counter =
            spawn_counter == 0 ?
              swarm->description.spawn_counter_reset_value :
              spawn_counter;
      } else {
        swarm->spawn_counter =
            spawn_counter - swarm->description.spawn_counter_granularity;
      }

      // reset the activation counter
      swarm->activation_counter = 0;
    }
  }
}



}