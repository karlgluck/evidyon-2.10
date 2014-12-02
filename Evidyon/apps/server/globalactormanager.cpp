#include "globalactormanager.h"
#include "worldregion.h"
#include "globalworldmanager.h"
#include "../shared/actordescriptiontemplate.h"
#include "../common/safemacros.h"

#include <dc/debug>

namespace Evidyon {

GlobalActorManager* GlobalActorManager::singleton_ = NULL;

GlobalActorManager* GlobalActorManager::singleton() { return singleton_; }

GlobalActorManager::GlobalActorManager() {
  actor_description_templates_ = NULL;
}

bool GlobalActorManager::create(FILE* game_file) {
  if (!actor_memory_pool_.create()) return false;
  next_available_actor_id_ = 0;
  singleton_ = this;

  { // load the actor description templates
    size_t templates;
    CONFIRM(1 == fread(&templates, sizeof(templates), 1, game_file)) else return false;

    actor_description_templates_ = new ServerActorDescriptionTemplate[templates];
    ASSERT(actor_description_templates_ != NULL) else return false;
    number_of_actor_description_templates_ = templates;

    CONFIRM(templates == fread(actor_description_templates_,
                               sizeof(ServerActorDescriptionTemplate),
                               templates,
                               game_file)) else return false;
  }

  return true;
}


void GlobalActorManager::destroy() {
  actor_memory_pool_.destroy();
  singleton_ = NULL;
  SAFE_DELETE_ARRAY(actor_description_templates_);
  number_of_actor_description_templates_ = 0;
}


bool GlobalActorManager::acquireActor(ActorPointer* pointer,
                                      ActorController* controller,
                                      unsigned int description_template,
                                      Map* map,
                                      float x,
                                      float y) {
  if (!actor_memory_pool_.acquire(pointer)) return false;
  ActorID id = PackActorID(++next_available_actor_id_,
                           actor_memory_pool_.indexOf(pointer),
                           description_template);
  Actor* actor = pointer->dereferenceAssumingValid();
  actor->acquire(getDescriptionTemplate(description_template),
                 controller,
                 id,
                 map,
                 x,
                 y);
  active_actors_.insert(ActiveActors::value_type(id, actor));
  return true;
}


void GlobalActorManager::releaseActor(ActorPointer* pointer) {
  Actor* actor = pointer->dereference();
  if (actor) {
    active_actors_.erase(actor->getIDNumber());
    actor->release();
  }
  actor_memory_pool_.release(pointer);
}


void GlobalActorManager::getPointer(Actor* actor, ActorPointer* pointer) {
  actor_memory_pool_.convert(actor, pointer);
}


void GlobalActorManager::getPointer(int actor_id, ActorPointer* pointer) {
  ActiveActors::iterator i = active_actors_.find(actor_id);
  if (i == active_actors_.end()) {
    pointer->reset();
  } else {
    actor_memory_pool_.convert(i->second, pointer);
  }
}

Actor* GlobalActorManager::getActor(ActorID id) {
  ActiveActors::iterator i = active_actors_.find(id);
  return i == active_actors_.end() ? NULL : i->second;
}

void GlobalActorManager::setSyncGroup(int sync_group) {
  for (ActorMemoryPool::Iterator i(&actor_memory_pool_);
     i.hasNext();
     i.advance()) {
    Actor* actor = i.get();
    //CONFIRM(actor->getRegion()) else return;
    actor->setSyncGroup(sync_group);
  }
}

const ServerActorDescriptionTemplate*
    GlobalActorManager::getDescriptionTemplate(size_t template_index) const {
  CONFIRM(template_index < number_of_actor_description_templates_) else return NULL;
  return &actor_description_templates_[template_index];
}


}