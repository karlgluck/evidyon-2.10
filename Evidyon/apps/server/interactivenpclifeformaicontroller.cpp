#include "interactivenpclifeformaicontroller.h"
#include "time.h"
#include "globalactormanager.h"
#include "../shared/client-server/actordescription.h"
#include "../shared/server-editor/lifeformaidescription.h"
#include "meleeattack.h"
#include "client.h"
#include "worldregion.h"
#include "globalitemmanager.h"
#include "randf.h"


#include <dc/debug>
#include <math.h> // atan2f, sqrtf, ...
#include "globalworldmanager.h" // for Map

namespace Evidyon {



void InteractiveNPCLifeformAIController::acquire(
    const ActorPointer* actor,
    const LifeformAIDescription_InteractiveNPC* description) {


  actor_.copy(actor);
  description_ = description;

  Actor* actor_pointer = actor_.dereferenceAssumingValid();
  actor_pointer->setMemberOfFaction(Actor::NEUTRAL_NPC);

  actor_pointer->setDirection(randfAngleRadians());
  actor_pointer->getPosition(&root_x_, &root_y_);
  next_movement_time_ = 0.0;
}


void InteractiveNPCLifeformAIController::release(GlobalActorManager* actor_manager) {
  actor_manager->releaseActor(&actor_);
  description_ = NULL;
}



void InteractiveNPCLifeformAIController::update(double time,
                                                double time_since_last_update) {

  float x, y;
  Actor* actor = actor_.dereference();
  CONFIRM(actor) else return;
  actor->getPosition(&x, &y);
  WorldRegion* current_region = actor->getRegion();

  if (time > next_movement_time_) {
    float distance_home_sq = actor->distanceToSq(root_x_, root_y_);
    if (actor->isMoving()) { // stop moving
      actor->stopMoving();
      next_movement_time_ = time + (rand()/float(RAND_MAX)) * 30.0 + 15.0; // 15-45 seconds here
    } else {
      const float SPEED = 1.0f;
      if (distance_home_sq < 1.0f) { // at "home base"?
        float tgt_x = (rand()/float(RAND_MAX)) * 3.0f,
              tgt_y = (rand()/float(RAND_MAX)) * 3.0f;
        float dist = sqrtf(tgt_x*tgt_x + tgt_y*tgt_y);
        actor->faceLocation(root_x_ + tgt_x, root_y_ + tgt_y);
        actor->setSpeed(SPEED);
        next_movement_time_ = time + SPEED / dist;
      } else {
        // walk home
        actor->faceLocation(root_x_, root_y_);
        actor->setSpeed(SPEED);
        next_movement_time_ = time + SPEED / sqrtf(distance_home_sq);
      }
    }
  }

  float next_x = x, next_y = y;

  if (actor->isMoving() && current_region) {
    float dx, dy;
    actor->getPositionDelta(time_since_last_update, &dx, &dy);
    next_x = x + dx;
    next_y = y + dy;

    bool stop_moving = false;
    Actor* collision = current_region->collideActorWithActor(actor,
                                                             x,
                                                             y,
                                                             next_x,
                                                             next_y);
    if (collision != NULL) {
      stop_moving = !actor->isGlancingCollision(collision);
    }

    if (stop_moving) {
      actor->stopMoving();
      next_movement_time_ = time + (rand()/float(RAND_MAX)) * 5.0f + 5.0f; // 5-10 sec wait
    }
  }

  {
    WorldRegion* regions_left[9];
    WorldRegion* regions_entered[9];
    int number_of_regions_left=0, number_of_regions_entered=0;
    if (actor->update(next_x,
                      next_y,
                      regions_left,
                     &number_of_regions_left,
                      regions_entered,
                     &number_of_regions_entered)) {
    }
  }
}




void InteractiveNPCLifeformAIController::applyMagicEffect(Actor* source,
                                                              double time,
                                                              const Magic* magic,
                                                              const MagicDescriptionStageEffect* effect,
                                                              const MagicDescriptionStageEffect::Parameters* effect_parameters) {

}


void InteractiveNPCLifeformAIController::fillStateDescription(ActorStateDescriptionData* state) {
  state->actor_type = 0;//description_->actor_type;
  state->enchantments = 0; // TODO :D
  state->corporeal_state = NORMAL;
  state->equipped_items[ACTORATTACHMENTPOINT_HELMET] = (unsigned short)~0;
  state->equipped_items[ACTORATTACHMENTPOINT_WEAPON] = (unsigned short)~0;
  state->equipped_items[ACTORATTACHMENTPOINT_SHIELD] = (unsigned short)~0;
}


const char* InteractiveNPCLifeformAIController::getName() {
  return "";
}

int InteractiveNPCLifeformAIController::instantiateMeleeAttack(
      Actor* target,
      MeleeAttack* attack) {
  return -1;
}

bool InteractiveNPCLifeformAIController::block(
      Actor* attacker,
      const MeleeAttack* attack) {
  return false;
}

void InteractiveNPCLifeformAIController::applyMeleeAttack(
      Actor* attacker,
      MeleeAttack* attack) {
}




}