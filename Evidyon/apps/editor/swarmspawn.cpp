#include "swarmspawn.h"
#include "../shared/server-editor/swarmspawndescription.h"



SwarmSpawn::SwarmSpawn() {
  member("Lifeform AI", &ai_lifeform_);
  member("Time Between Waves", &time_between_waves_);
  member("Time Between Individuals", &time_between_individuals_);
}

bool SwarmSpawn::compile(unsigned int granularity,
                         Evidyon::CompiledRegionSwarmSpawn* description) const {
  description->ai_lifeform_type =
      ai_lifeform_.getReferencedResourceIndex();
  description->spawn_counter_granularity = 
      granularity;
  description->spawn_counter_activate_period =
      time_between_individuals_.getValue() / Evidyon::SWARM_SPAWN_PERIOD;
  description->spawn_counter_reset_value =
     (time_between_waves_.getValue() * granularity) / time_between_individuals_.getValue();

  return true;
}


std::string SwarmSpawn::staticTypeName() {
  return "SwarmSpawn";
}
