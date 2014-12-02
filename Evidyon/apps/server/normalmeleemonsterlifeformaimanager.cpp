#include "normalmeleemonsterlifeformaimanager.h"
#include "globalactormanager.h"
#include <dc/debug>
#include "client.h"
#include "../shared/server-editor/lifeformaidescription.h"


namespace Evidyon {


NormalMeleeMonsterLifeformAIManager::NormalMeleeMonsterLifeformAIManager() {
}

bool NormalMeleeMonsterLifeformAIManager::create() {
  return controllers_.create();
}


void NormalMeleeMonsterLifeformAIManager::destroy() {
  // Release everything
  {
    GlobalActorManager* actor_manager = GlobalActorManager::singleton();
    for (ControllerPool::Iterator i(&controllers_); i.hasNext(); i.advance()) {
      NormalMeleeMonsterLifeformAIController* controller = i.get();
      controller->release(actor_manager);
    }
  }
  controllers_.destroy();
}


bool NormalMeleeMonsterLifeformAIManager::update(double time,
                                                 double time_since_last_update) {
  GlobalActorManager* actor_manager = GlobalActorManager::singleton();

  for (ControllerPool::Iterator i(&controllers_); i.hasNext(); i.advance()) {
    NormalMeleeMonsterLifeformAIController* controller = i.get();

    controller->update(time, time_since_last_update);

    // erase the actor if it is dead & gone
    if (controller->isDead() && controller->canPerformAction()) {
      controller->release(actor_manager);
      i.remove();
    }
  }

  return true;
}



bool NormalMeleeMonsterLifeformAIManager::acquire(
    float x,
    float y,
    Map* map,
    GlobalActorManager* actor_manager,
    const LifeformAIDescription_NormalMeleeMonster* description,
    int level,
    unsigned int treasure_randomizer_mask,
    bool delay_activity,
    ActorPointer* acquired_actor) {
  NormalMeleeMonsterLifeformAIController* controller = controllers_.acquire();
  CONFIRM(controller) else return false;
  ActorPointer actor;
  const AIMonsterActorType* actor_type =
      &description->actor_types[rand()%LIFEFORMAI_NORMALMELEE_ACTORTYPES];
  CONFIRM(actor_manager->acquireActor(&actor,
                                      controller,
                                      actor_type->template_index,
                                      map,
                                      x,
                                      y)) else {
    controllers_.release(controller);
    return false;
  }
  controller->acquire(&actor,
                      level,
                      treasure_randomizer_mask,
                      description,
                      delay_activity);
  acquired_actor->copy(&actor);

  return true;
}





void NormalMeleeMonsterLifeformAIManager::releaseWorldRegion(WorldRegion* region) {
  GlobalActorManager* actor_manager = GlobalActorManager::singleton();
  for (ControllerPool::Iterator i(&controllers_); i.hasNext(); i.advance()) {
    NormalMeleeMonsterLifeformAIController* controller = i.get();
    Actor* actor = controller->getActor();
    ASSERT(actor);
    if (actor->getRegion() != region) continue;
    controller->release(actor_manager);
    i.remove();
  }
}

}