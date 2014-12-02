#include "globallifeformaimanager.h"
#include <stdio.h>
#include "../common/safemacros.h"
#include "../shared/server-editor/lifeformaidescription.h"
#include <dc/debug>
#include "globaldatabasemanager.h"

namespace Evidyon {

GlobalLifeformAIManager* GlobalLifeformAIManager::singleton_ = NULL;

GlobalLifeformAIManager* GlobalLifeformAIManager::singleton() {
  return singleton_;
}

GlobalLifeformAIManager::GlobalLifeformAIManager() {
  descriptions_ = NULL;
  number_of_descriptions_ = 0;
}

bool GlobalLifeformAIManager::create(FILE* game_file) {
  CONFIRM(normal_melee_monsters_.create()) else return false;

  size_t number_of_descriptions = 0;
  fread(&number_of_descriptions,
        sizeof(number_of_descriptions), 
        1,
        game_file);
  number_of_descriptions_ = number_of_descriptions;
  if (number_of_descriptions > 0) {
    descriptions_ = new LifeformAIDescription[number_of_descriptions];
    ASSERT(descriptions_) else { destroy(); return false; }
    fread(descriptions_,
          sizeof(LifeformAIDescription),
          number_of_descriptions,
          game_file);
  } else {
    descriptions_ = NULL;
  }

  singleton_ = this;

  return true;
}


void GlobalLifeformAIManager::destroy() {
  SAFE_DELETE_ARRAY(descriptions_);
  number_of_descriptions_ = 0;
  normal_melee_monsters_.destroy();
  interactive_npcs_.destroy();
}


void GlobalLifeformAIManager::update(double time,
                                     double time_since_last_update) {
  normal_melee_monsters_.update(time, time_since_last_update);
  interactive_npcs_.update(time, time_since_last_update);
}


void GlobalLifeformAIManager::releaseWorldRegion(WorldRegion* region) {
  normal_melee_monsters_.releaseWorldRegion(region);
  interactive_npcs_.releaseWorldRegion(region);
}


bool GlobalLifeformAIManager::acquire(
   int spawn_x,
   int spawn_y,
   Map* map,
   GlobalActorManager* actor_manager,
   size_t lifeform_ai_type,
   int level,
   unsigned int treasure_randomizer_mask,
   bool delay_activity,
   ActorPointer* acquired_actor) {
#if defined(EXTENDED_DEBUGGING)
  ASSERT(acquired_actor != NULL) else return false;
#endif
  CONFIRM(lifeform_ai_type < number_of_descriptions_) else return false;

  bool succeeded = false;

  const LifeformAIDescription* lifeform_desc = &descriptions_[lifeform_ai_type];
  switch (lifeform_desc->type) {
    case LifeformAIDescription::NORMAL_MELEE_MONSTER: {
      const LifeformAIDescription_NormalMeleeMonster* desc =
          &lifeform_desc->normal_melee_monster;
      succeeded = normal_melee_monsters_.acquire(spawn_x * 1.0f + 0.5f,
                                                 spawn_y * 1.0f + 0.5f,
                                                 map,
                                                 actor_manager,
                                                 desc,
                                                 level,
                                                 treasure_randomizer_mask,
                                                 delay_activity,
                                                 acquired_actor);
    } break;

    case LifeformAIDescription::INTERACTIVE_NPC: {
      const LifeformAIDescription_InteractiveNPC* desc = 
          &lifeform_desc->interactive_npc;
      succeeded = interactive_npcs_.acquire(spawn_x * 1.0f + 0.5f,
                                            spawn_y * 1.0f + 0.5f,
                                            map,
                                            actor_manager,
                                            desc,
                                            acquired_actor);
    } break;
  }

  return succeeded;
}




}