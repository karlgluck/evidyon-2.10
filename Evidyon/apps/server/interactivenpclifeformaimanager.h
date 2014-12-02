#ifndef __INTERACTIVENPCLIFEFORMAIMANAGER_H__
#define __INTERACTIVENPCLIFEFORMAIMANAGER_H__


#include "interactivenpclifeformaicontroller.h"
#include <dcx/memorypool>

namespace Evidyon {

class Map;
class WorldRegion;
class GlobalActorManager;
struct LifeformAIDescription_InteractiveNPC;


class InteractiveNPCLifeformAIManager {
  static const int MAX_INTERACTIVE_NPCS = 32;
  typedef dcx::dcxMemoryPool<InteractiveNPCLifeformAIController, MAX_INTERACTIVE_NPCS> ControllerPool;

public:
  InteractiveNPCLifeformAIManager();

  bool create();
  void destroy();
  bool update(double time,
              double time_since_last_update);

  // Generates a new interactive NPC
  bool acquire(float x,
               float y,
               Map* map,
               GlobalActorManager* actor_manager,
               const LifeformAIDescription_InteractiveNPC* description,
               ActorPointer* acquired_actor); // output

  // When a region is being removed, this method gets rid of all controllers
  // that are local to that region.
  void releaseWorldRegion(WorldRegion* region);


private:
  ControllerPool controllers_;
};



}



#endif