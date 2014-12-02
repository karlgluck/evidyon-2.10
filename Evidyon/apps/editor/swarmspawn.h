#ifndef __SWARMSPAWN_H__
#define __SWARMSPAWN_H__

#include <dc/table>
#include <dc/float>
#include "lifeformai.h"

namespace Evidyon {
struct CompiledRegionSwarmSpawn;
}

// used by the SwarmSpawner
class SwarmSpawn : public dc::dcResource<SwarmSpawn> {
public:
  SwarmSpawn();
  bool compile(unsigned int granularity,
               Evidyon::CompiledRegionSwarmSpawn* description) const;

public:
  static std::string staticTypeName();

private:
  dc::dcTable<LifeformAI>::Reference ai_lifeform_;
  dc::dcFloat time_between_waves_;
  dc::dcFloat time_between_individuals_;
};




#endif