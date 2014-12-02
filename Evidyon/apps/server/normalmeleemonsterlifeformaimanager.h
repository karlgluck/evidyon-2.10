#ifndef __NORMALMELEEMONSTERLIFEFORMAIMANAGER_H__
#define __NORMALMELEEMONSTERLIFEFORMAIMANAGER_H__


#include "normalmeleemonsterlifeformaicontroller.h"
#include <dcx/memorypool>

namespace Evidyon {

class Map;
class WorldRegion;
class GlobalActorManager;
struct LifeformAIDescription_NormalMeleeMonster;

// Controls all of the melee monsters in the world.  This class is responsible
// for updating the monsters, keeping track of their actors, performing
// targeting and handling combat.  Instead of introducing a separate lone
// structure for each monster, this controller handles all of them in an attemt
// to keep the complexity and memory requirements low.  This monster type is
// very common, so there are bound to be many of them active at a time.
class NormalMeleeMonsterLifeformAIManager {
  static const int MAX_MELEE_MONSTERS = 512;
  typedef dcx::dcxMemoryPool<NormalMeleeMonsterLifeformAIController, MAX_MELEE_MONSTERS> ControllerPool;

public:
  NormalMeleeMonsterLifeformAIManager();

  bool create();
  void destroy();
  bool update(double time,
              double time_since_last_update);

  // Generates a new melee monster
  bool acquire(float x,
               float y,
               Map* map,
               GlobalActorManager* actor_manager,
               const LifeformAIDescription_NormalMeleeMonster* description,
               int level,
               unsigned int treasure_randomizer_mask,
               bool delay_activity,
               ActorPointer* acquired_actor); // output

  //bool acquireDelayed(float x,
  //                    float y,
  //                    WorldRegion* region,
  //                    GlobalActorManager* actor_manager,
  //                    const LifeformAIDescription_NormalMeleeMonster* description,
  //                    int level,
  //                    unsigned int treasure_randomizer_mask,
  //                    ActorPointer* acquired_actor); // output


  // When a region is being removed, this method gets rid of all controllers
  // that are local to that region.
  void releaseWorldRegion(WorldRegion* region);


private:
  ControllerPool controllers_;
};



}



#endif