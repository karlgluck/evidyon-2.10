#include "normalmeleemonsterlifeformaicontroller.h"
#include "time.h"
#include "globalactormanager.h"
#include "../shared/client-server/actordescription.h"
#include "../shared/server-editor/lifeformaidescription.h"
#include "meleeattack.h"
#include "client.h"
#include "worldregion.h"
#include "globalitemmanager.h"



#include <dc/debug>
#include <math.h> // atan2f, sqrtf, ...
#include "globalworldmanager.h" // for Map

namespace Evidyon {


static const double TIME_DEAD_BEFORE_ERASED = 6.0;
static const double TIME_BETWEEN_TARGET_SEARCHES = 1.0;
static const double DELAY_BEFORE_SPAWN = 0.50;

void NormalMeleeMonsterLifeformAIController::acquire(
    const ActorPointer* actor,
    int level,
    unsigned int treasure_randomizer_mask,
    const LifeformAIDescription_NormalMeleeMonster* description,
    bool delay_activity) {

  actor_.copy(actor);
  hp_ = description->hp + description->added_hp_per_level * level;
  level_ = level;
  description_ = description;
  updates_until_next_allowed_action_ = 0;
  current_state_ = WAITING;
  treasure_randomizer_mask_ = treasure_randomizer_mask;
  navigate_clockwise_ = rand()%2 == 0;

  Actor* actor_pointer = actor_.dereferenceAssumingValid();
  actor_pointer->setMemberOfFaction(Actor::CHAOTIC);
  melee_combat_controller_.reset(actor_pointer);
  melee_combat_controller_.setActionDelay(description->time_between_attacks);

  // this sets how long before the monster instantiates itself in the world
  if (delay_activity) {
    updates_until_next_allowed_action_ = DELAY_BEFORE_SPAWN / Time::MAIN_LOOP_PERIOD;
  } else {
    updates_until_next_allowed_action_ = 0;
    actor_pointer->setCorporealState(NORMAL);
    actor_pointer->enterWorld();
  }
}

void NormalMeleeMonsterLifeformAIController::release(GlobalActorManager* actor_manager) {
  actor_manager->releaseActor(&actor_);
  description_ = NULL;
}



void NormalMeleeMonsterLifeformAIController::update(double time,
                                                    double time_since_last_update) {
  --updates_until_next_allowed_action_;

  // nothing needs to be updated if we're dead...
  if (isDead()) return;

  float x, y;
  Actor* actor = actor_.dereference();
  CONFIRM(actor) else return;
  actor->getPosition(&x, &y);
  WorldRegion* current_region = actor->getRegion();

  // if the actor is invisible, this actor is waiting to be spawned
  if (actor->isInvisible()) {
    if (updates_until_next_allowed_action_ <= 0) {
      actor->setCorporealState(NORMAL);
    } else {
      return;
    }
  }

  if (updates_until_next_allowed_action_ <= 0) {
    switch (current_state_) {
      case WAITING:
        break;

      case NAVIGATING:
        current_state_ = PERSUING;
        break;
    }
  }

  if (current_state_ != NAVIGATING) {

    melee_combat_controller_.update(time);
    if (melee_combat_controller_.pursuingTarget()) {
      if (!melee_combat_controller_.frozen()) {
        Actor* target = melee_combat_controller_.getTarget();
        if (target) {

          float target_x, target_y;
          target->getPosition(&target_x, &target_y);
          float dx = target_x - x, dy = target_y - y;
          float distance_sq = dx*dx+dy*dy;

          static const float ATAN2_THREASHOLD = 0.01f;
          if (distance_sq > ATAN2_THREASHOLD) {
            actor->setDirection(atan2f(dy,dx));
          }
          actor->setSpeed(description_->movement_speed);

          WorldRegion* target_region = target->getRegion();
          Map* map = NULL;
          Map* current_map = NULL;
          if (target_region) {
            map = target_region->getMap();
          }

          if (current_region) {
            current_map = current_region->getMap();
          }

          if (target->isDead() ||
              !actor->withinViewOf(target) ||
              (map != current_map)) {
            melee_combat_controller_.attack(NULL);
            actor->stopMoving();
            current_state_ = WAITING;
          } else {
            current_state_ = PERSUING;
          }
        }
      }

    } else if (current_region != NULL) { // look for a target
      WorldRegion::ClientSet::iterator end = current_region->nearbyClientsEnd();
      WorldRegion::ClientSet::iterator i = current_region->nearbyClientsBegin();
      for (; i != end; ++i) {
        Client* client = *i;
        if (client->validTargetOfEnemyAt(x, y)) { // target this actor
          melee_combat_controller_.attack(client->getActor());
          current_state_ = PERSUING;
          break;
        }
      }

      if (i == end) {
        actor->stopMoving();
        current_state_ = WAITING;
      }
    }
  }

  float next_x, next_y;

  if (actor->isMoving()) {
    float dx, dy;
    actor->getPositionDelta(time_since_last_update, &dx, &dy);
    next_x = x + dx;
    next_y = y + dy;

    const MapLocationInfo* next_location_info = NULL;
    {
      int next_map_x = (int)(next_x+0.5f),
          next_map_y = (int)(next_y+0.5f);
      Map* map = current_region->getMap();
      if (map) {
        next_location_info = map->getMapLocationInfo(next_map_x, next_map_y);
      }
    }

    bool stop_moving = false;
    Actor* collision = current_region->collideActorWithActor(actor,
                                                             x,
                                                             y,
                                                             next_x,
                                                             next_y);
    if (collision != NULL) {
      if (collision->isEnemyOf(actor)) { // attack what we collided with
        ActorPointer ptr;
        GlobalActorManager::singleton()->getPointer(actor, &ptr);
        melee_combat_controller_.attack(&ptr);
        stop_moving = true;
      } else {
        stop_moving = !actor->isGlancingCollision(collision);
      }
    }
    stop_moving = stop_moving || !next_location_info || !next_location_info->pass_by_moving;

    if (stop_moving) {
      Actor* target = melee_combat_controller_.getTarget();
      if (target != NULL && (collision != target)) {
        float entrance_distance,
             entrance_angle;
        float my_distance_to_target = sqrtf(actor->distanceToSq(target));
        float my_angle_to_target = actor->angleTo(target);
        if (collision != NULL) {
          // this is really messy, i should write a minAngleDifference method since this is used so often
          float angle_diff = collision->angleTo(target) - my_angle_to_target;
          if (angle_diff > 3.1415) {
            angle_diff -= 2.0f * 3.1415;
          }
          if (angle_diff < -3.1415) {
            angle_diff += 2.0f * 3.1415;
          }

          //entrance_angle = my_angle_to_target + 1.0f;//- ((rand()/(float)RAND_MAX) * 1.5f + 0.2f) * angle_diff;
          float sign;
          sign = navigate_clockwise_ ? +1.0f : -1.0f;

          entrance_angle = my_angle_to_target + sign * 3.14159265 / 3.0f / 3.0f; // can only fit 6 around it anyway
          entrance_distance = my_distance_to_target + 1.0f;

        } else { // no collision with another actor
          navigate_clockwise_ = !navigate_clockwise_;
          entrance_distance = my_distance_to_target + 1.0f;
          entrance_angle = my_angle_to_target + 3.14159265 / 3.0f / 3.0f * (navigate_clockwise_ ? 1.0f : -1.0f);
        }

        float tgtx, tgty;
        target->getPosition(&tgtx, &tgty);
        if (rand()%3 != 0) {
          entrance_angle += 3.14159265f;
          tgtx += cosf(entrance_angle) * entrance_distance;
          tgty += sinf(entrance_angle) * entrance_distance;
          actor->faceLocation(tgtx, tgty);
          updates_until_next_allowed_action_ =
            (sqrtf(actor->distanceToSq(tgtx, tgty)) / description_->movement_speed) / Time::MAIN_LOOP_PERIOD;
          actor->setSpeed(description_->movement_speed);
          current_state_ = NAVIGATING;
        } else {
          actor->stopMoving(); // 1/3 times just stop and face the target
          actor->faceLocation(tgtx, tgty);
          updates_until_next_allowed_action_ = 0.5f / Time::MAIN_LOOP_PERIOD;
        }

      } else {
        actor->stopMoving();
      }

      next_x = x;
      next_y = y;
    }
  } else {
    next_x = x;
    next_y = y;
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


void NormalMeleeMonsterLifeformAIController::changeHP(Actor* source,
                                                      int amount) {
  if (isDead()) return;

  hp_ += amount;

  // TODO: change target if attacker does lots of damage

  if (hp_ < 0) {
    Actor* actor = actor_.dereferenceAssumingValid();
    actor->setCorporealState(DEAD);
    updates_until_next_allowed_action_ =
      TIME_DEAD_BEFORE_ERASED / Time::MAIN_LOOP_PERIOD;

    if (source != NULL) { // give EXP
      ActorController* source_controller = 
        source->getController();
      if (source_controller->isClient()) {
        Client* client = (Client*)source_controller;
        client->changeEXP(description_->experience + level_ * description_->added_experience_per_level);
      }
    }

    // spawn items
    float x, y;
    actor->getPosition(&x, &y);
    GlobalItemManager* global_item_manager = GlobalItemManager::singleton();
    WorldRegion* region = actor->getRegion();
    CONFIRM(region) else return;
    global_item_manager->dropMonsterTreasure(region->getMap(),
                                             x,
                                             y,
                                             description_->treasure_class_index,
                                             treasure_randomizer_mask_);
  }
}


void NormalMeleeMonsterLifeformAIController::applyMagicEffect(Actor* source,
                                                              double time,
                                                              const Magic* magic,
                                                              const MagicDescriptionStageEffect* effect,
                                                              const MagicDescriptionStageEffect::Parameters* effect_parameters) {
  switch (effect->type) {
    case MagicDescriptionStageEffect::WEAPON: {
      changeHP(source, description_->magic_damage_factor *
                       effect_parameters->int_param *
                       effect_parameters->float_param);
      Actor* actor = actor_.dereference();
      actor->fleshHit();
    } break;

    case MagicDescriptionStageEffect::HP_NOARMOR:
    case MagicDescriptionStageEffect::HP: {
      changeHP(source, description_->magic_damage_factor *
                       effect_parameters->int_param);
      break;
    }

    case MagicDescriptionStageEffect::TELEPORT_SWAP: {  // swap locations with the target actor--a client!
      Actor* actor = actor_.dereference();
      static const float MAX_TELEPORT_SWAP_DISTANCE_SQ = 12.0f*12.0f*1.5f;
      if ((source != NULL) && //target_actor->getController()->isClient() &&
        (source->distanceToSq(actor) < MAX_TELEPORT_SWAP_DISTANCE_SQ)) {
        float my_x, my_y, target_x, target_y;
        actor->getPosition(&my_x, &my_y);
        source->getPosition(&target_x, &target_y);
        source->faceLocation(my_x, my_y);
        source->teleportTo(my_x, my_y);
        actor->faceLocation(target_x, target_y);
        actor->teleportTo(target_x, target_y);
      }
    } break;

  }
}


void NormalMeleeMonsterLifeformAIController::fillStateDescription(ActorStateDescriptionData* state) {
  state->actor_type = 0;//actor_type_;
  state->enchantments = 0; // TODO :D
  state->corporeal_state = hp_ > 0 ? NORMAL : DEAD;
  state->equipped_items[ACTORATTACHMENTPOINT_HELMET] = (unsigned short)~0;
  state->equipped_items[ACTORATTACHMENTPOINT_WEAPON] = (unsigned short)~0;
  state->equipped_items[ACTORATTACHMENTPOINT_SHIELD] = (unsigned short)~0;
}


const char* NormalMeleeMonsterLifeformAIController::getName() {
  return "Hobbit";
}

int NormalMeleeMonsterLifeformAIController::instantiateMeleeAttack(
      Actor* target,
      MeleeAttack* attack) {

  Actor* actor = actor_.dereferenceAssumingValid();
  //float distance_sq = actor->distanceToSq(target);
  float range = actor->separationDistanceTo(target);

  // look through the attacks to see if we can perform one from this range
  int possible_attacks_mask = 0;
  possible_attacks_mask =
    (description_->melee_attack_types[0].range > range ? 1 : 0) << 0;
  possible_attacks_mask |=
    (description_->melee_attack_types[1].range > range ? 1 : 0) << 1;
  possible_attacks_mask |=
    (description_->melee_attack_types[2].range > range ? 1 : 0) << 2;


  // from the possible attacks, pick one randomly
  // TODO
  int attack_index = rand()%3;
  bool attack_target = (1 & (possible_attacks_mask >> (attack_index))) == 1;
  if (attack_target) {
    const AIMeleeAttackDescription* attack_desc =
        &description_->melee_attack_types[attack_index];
    attack->swing_action = attack_desc->action;
    attack->blocked_action = attack_desc->action;
    attack->damage = attack_desc->base_damage +
                     attack_desc->added_damage_per_level * level_;
    attack->duration = attack_desc->duration;
    attack->attack_roll = level_; // ???
    attack->range = attack_desc->range;

    float x, y;
    target->getPosition(&x, &y);
    actor->faceLocation(x, y);

    return 0; // attack the target
  } else {
    return 1; // move toward target
  }
}

bool NormalMeleeMonsterLifeformAIController::block(
      Actor* attacker,
      const MeleeAttack* attack) {
  return false; // todo
}

void NormalMeleeMonsterLifeformAIController::applyMeleeAttack(
      Actor* attacker,
      MeleeAttack* attack) {
  changeHP(attacker, -attack->damage);
  Actor* actor = actor_.dereference();
  //actor->setAction(ACTORACTION_GOTHIT);
  actor->fleshHit();
}




}