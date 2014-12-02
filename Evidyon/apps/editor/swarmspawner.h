#ifndef __SWARMSPAWNER_H__
#define __SWARMSPAWNER_H__


#include "mapcolormask.h"
#include "swarmspawn.h"
#include "spawndensity.h"
#include "spawnlevel.h"
#include <d3dx9.h>

namespace Evidyon {

struct CompiledRegionSwarmSpawn;
struct CompiledSwarmSpawnLocationInfo;
extern const float SWARM_SPAWN_PERIOD;

}

class SwarmSpawner : public dc::dcResource<SwarmSpawner> {
public:
  SwarmSpawner();
  bool compileForServer(LPDIRECT3DDEVICE9 d3d_device,
                        int map_regions_wide,
                        int map_regions_high,
                        int* monster_level_rarity_boost,
                        dc::dcStreamOut* stream);

public:
  static std::string staticTypeName();

private:
  dc::dcList<MapColorMask<SwarmSpawn>> spawn_;
  MapColorMask<SpawnLevel> levels_;
  MapColorMask<SpawnDensity> density_;
};


#endif