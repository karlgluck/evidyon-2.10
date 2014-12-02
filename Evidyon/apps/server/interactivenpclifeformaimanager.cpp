#include "interactivenpclifeformaimanager.h"
#include "globalactormanager.h"
#include <dc/debug>
#include "client.h"
#include "../shared/server-editor/lifeformaidescription.h"

namespace Evidyon {


InteractiveNPCLifeformAIManager::InteractiveNPCLifeformAIManager() {
}

bool InteractiveNPCLifeformAIManager::create() {
  return controllers_.create();
}


void InteractiveNPCLifeformAIManager::destroy() {
  // Release everything
  {
    GlobalActorManager* actor_manager = GlobalActorManager::singleton();
    for (ControllerPool::Iterator i(&controllers_); i.hasNext(); i.advance()) {
      InteractiveNPCLifeformAIController* controller = i.get();
      controller->release(actor_manager);
    }
  }
  controllers_.destroy();
}


bool InteractiveNPCLifeformAIManager::update(double time,
                                                 double time_since_last_update) {
  GlobalActorManager* actor_manager = GlobalActorManager::singleton();

  for (ControllerPool::Iterator i(&controllers_); i.hasNext(); i.advance()) {
    InteractiveNPCLifeformAIController* controller = i.get();

    controller->update(time, time_since_last_update);
  }

  return true;
}



bool InteractiveNPCLifeformAIManager::acquire(
    float x,
    float y,
    Map* map,
    GlobalActorManager* actor_manager,
    const LifeformAIDescription_InteractiveNPC* description,
    ActorPointer* acquired_actor) {
  InteractiveNPCLifeformAIController* controller = controllers_.acquire();
  CONFIRM(controller) else return false;
  ActorPointer actor;
  CONFIRM(actor_manager->acquireActor(&actor,
                                      controller,
                                      description->actor_template_index,
                                      map,
                                      x,
                                      y)) else {
    controllers_.release(controller);
    return false;
  }
  controller->acquire(&actor,
                      description);
  acquired_actor->copy(&actor);
  actor.dereference()->setCorporealState(NORMAL);
  return true;
}



void InteractiveNPCLifeformAIManager::releaseWorldRegion(WorldRegion* region) {
  GlobalActorManager* actor_manager = GlobalActorManager::singleton();
  for (ControllerPool::Iterator i(&controllers_); i.hasNext(); i.advance()) {
    InteractiveNPCLifeformAIController* controller = i.get();
    Actor* actor = controller->getActor();
    ASSERT(actor);
    if (actor->getRegion() != region) continue;
    controller->release(actor_manager);
    i.remove();
  }
}

}