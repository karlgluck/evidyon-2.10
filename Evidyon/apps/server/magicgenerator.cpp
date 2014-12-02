#include "magicgenerator.h"
#include "actor.h"
#include "magic.h"
#include "instantiatedmagic.h"
#include "../common/null.h"
#include "globalmagicmanager.h"
#include "../shared/evidyon_magic.h"
#include "globalgamefilemanager.h"
#include "globalactormanager.h"

#include <math.h> // for atan2f
#include <dc/debug>

namespace Evidyon {

MagicGenerator::MagicGenerator() {
  owner_ = NULL;
  next_generation_time_ = 0.0;
  current_instance_ = NULL;
  times_to_generate_ = 0;
}

MagicGenerator::~MagicGenerator() {
  reset(NULL);
}

void MagicGenerator::reset(Actor* owner) {
  stop();
  owner_ = owner;
}

void MagicGenerator::update(GlobalMagicManager* manager, double time) {
  if (owner_ &&
      (times_to_generate_ > 0) &&
      current_instance_ &&
      (time>next_generation_time_)) {

  const Magic* magic = &current_instance_->magic;
  const MagicDescription* description = magic->description;
  const MagicDescriptionStage* stage = &description->stages[0];
  const MagicDescriptionStageParameters* magic_params = &current_instance_->stage_parameters[0];
  const MagicDescriptionStageTargeting::Parameters* targeting_params = &magic_params->targeting;

  switch (stage->targeting.type) {
      case MagicDescriptionStageTargeting::SELF:
      manager->spawnTimerTargeter(owner_,
                                  owner_,
                                  current_instance_,
                                  time + targeting_params->delay,
                                  0);
    break;

    case MagicDescriptionStageTargeting::PROJECTILE: {
      GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
      double duration = gamefile_manager->getProjectileDuration(stage->per_targeter_fx_type);
      manager->spawnProjectileTargeter(owner_,
                                       targeting_params->spread,
                                       time + duration,
                                       current_instance_);
    } break;

    case MagicDescriptionStageTargeting::ANY_TARGET_ACTOR: {
      Actor* target = const_cast<Actor*>(magic->target.dereference());
      if (target == NULL) break;
      manager->spawnTimerTargeter(target,
                                  target,
                                  current_instance_,
                                  time + targeting_params->delay,
                                  0);
    } break;

    case MagicDescriptionStageTargeting::NONCASTER_TARGET_ACTOR: {
      Actor* target = const_cast<Actor*>(magic->target.dereference());
      if (target == NULL || (owner_ == target)) break;
      manager->spawnTimerTargeter(target,
                                  target,
                                  current_instance_,
                                  time + targeting_params->delay,
                                  0);
    } break;

    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_CASTER: {
      Actor* actors[MagicTargeterTimer::MAX_TARGETS];
      actors[0] = owner_;
      int targets_found = 1 + 
        owner_->findNearbyActorsInGroup(targeting_params->spread,
                                        actors + 1,
                                        targeting_params->targets-1);
      owner_->broadcastMagicFX(stage->aoe_magic_fx_type,
                               owner_,
                               0.0f,
                               0.0f,
                               current_instance_->visualization_power_parameter,
                               actors,
                               targets_found);
      if (targets_found > 0) {
        manager->spawnTimerTargeter(owner_,
                                    actors,
                                    targets_found,
                                    current_instance_,
                                    time + targeting_params->delay,
                                    0);
      }
    } break;

    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_CASTER_OFFENSIVE: {
      Actor* actors[MagicTargeterTimer::MAX_TARGETS];
      int targets = targeting_params->targets < MagicTargeterTimer::MAX_TARGETS ?
            MagicTargeterTimer::MAX_TARGETS :
            targeting_params->targets;
      int targets_found = owner_->findNearbyEnemies(targeting_params->spread,
                                                    actors,
                                                    targets);
      owner_->broadcastMagicFX(stage->aoe_magic_fx_type,
                               owner_,
                               0.0f,
                               0.0f,
                               current_instance_->visualization_power_parameter,
                               actors,
                               targets_found);
      if (targets_found > 0) {
        manager->spawnTimerTargeter(owner_,
                                    actors,
                                    targets_found,
                                    current_instance_,
                                    time + targeting_params->delay,
                                    0);
      }
    } break;

    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_OTHER_OFFENSIVE: {
      Actor* actors[MagicTargeterTimer::MAX_TARGETS];
      actors[0] = const_cast<Actor*>(magic->target.dereference());
      int targets_found = 1 + 
        owner_->findEnemiesNear(actors[0],
                                targeting_params->spread,
                                actors + 1,
                                targeting_params->targets-1);
      if (targets_found > 0) {
        actors[0]->broadcastMagicFX(stage->aoe_magic_fx_type, actors[0],
          0.0f, 0.0f, current_instance_->visualization_power_parameter,
          actors, targets_found);
        manager->spawnTimerTargeter(actors[0], actors, targets_found,
          current_instance_, time + targeting_params->delay, 0);
      } else {
        owner_->broadcastMagicFX(stage->aoe_magic_fx_type, actors[0],
          0.0f, 0.0f, current_instance_->visualization_power_parameter,
          NULL, 0);
      }
    } break;

    case MagicDescriptionStageTargeting::AOE_LOCATIONBOUND_OFFENSIVE: {
      Actor* actors[MagicTargeterTimer::MAX_TARGETS];
      float target_x = magic->x;
      float target_y = magic->y;
      int targets_found =
        owner_->findEnemiesNear(target_x,
                                target_y,
                                targeting_params->spread,
                                actors,
                                targeting_params->targets);
      if (targets_found > 0) {
        actors[0]->broadcastMagicFX(stage->aoe_magic_fx_type,
                                    NULL,
                                    target_x,
                                    target_y,
                                    current_instance_->visualization_power_parameter,
                                    NULL,
                                    0);
        manager->spawnTimerTargeter(NULL,
                                    actors,
                                    targets_found,
                                    current_instance_,
                                    time + targeting_params->delay,
                                    0);
      } else {
        owner_->broadcastMagicFX(stage->aoe_magic_fx_type,
                                 NULL,
                                 target_x,
                                 target_y,
                                 current_instance_->visualization_power_parameter,
                                 NULL,
                                 0);
      }
    } break;

    case MagicDescriptionStageTargeting::PERIODIC_AOE_LOCATIONBOUND_OFFENSIVE: {
      Actor* actors[MagicTargeterTimer::MAX_TARGETS];
      float target_x = magic->x;
      float target_y = magic->y;
      int targets_found =
        owner_->findEnemiesNear(target_x, target_y,
          targeting_params->spread, actors,
          MagicTargeterTimer::MAX_TARGETS);
      if (targets_found > 0) {
        actors[0]->broadcastMagicFX(stage->aoe_magic_fx_type, NULL,
          target_x, target_y, current_instance_->visualization_power_parameter,
          actors, targets_found);
        manager->spawnTimerTargeter(NULL, actors, targets_found,
          current_instance_, time + targeting_params->delay, 0);
      } else {
        owner_->broadcastMagicFX(stage->aoe_magic_fx_type, NULL,
          target_x, target_y, current_instance_->visualization_power_parameter,
          NULL, 0);
      }
    } break;
  }

  // Get rid of the current instance if we are finished casting
  --times_to_generate_;
  if (0 >= times_to_generate_) {
      --current_instance_->references;
      current_instance_ = NULL;
    } else {
      next_generation_time_ = time + targeting_params->delay;
    }
  }
}


bool MagicGenerator::cast(double time,
                          int source,
                          Magic* magic,
                          bool* freeze,
                          double* motion_timeout,
                          double* repeat_timeout) {
  InstantiatedMagic* instance =
    GlobalMagicManager::singleton()->createInstance();
  CONFIRM(instance) else return false;

  int times_to_generate;
  double motion_delay, action_delay;
  int visualization_power_parameter;

  bool result = owner_->instantiateMagic(source,
                                         magic,
                                        &times_to_generate,
                                        &motion_delay,
                                        &action_delay,
                                        &visualization_power_parameter,
                                         instance->stage_parameters);

  if (result) {
    stop();

    // If this instance doesn't need to be generated (there is a possibility
    // that this will be useful in the future, and is useful for debugging)
    // then don't bother doing anything with it.
    if (times_to_generate > 0) {
      GlobalActorManager::singleton()->getPointer(owner_, &instance->caster);
      instance->magic.description = magic->description;
      instance->magic.target = magic->target;
      instance->magic.x = magic->x;
      instance->magic.y = magic->y;

      instance->references++;
      current_instance_ = instance;

      next_generation_time_ =
        time + magic->description->stages[0].targeting.parameters.delay;
      times_to_generate_ = times_to_generate;

      // Michele's suggestion: only make players turn to face the location
      // if they aren't moving or are going to be stopped by this spell
      if ((motion_delay > 0.0) || owner_->isMoving() == false) {
        switch(magic->description->stages[0].targeting.type) {
          case MagicDescriptionStageTargeting::ANY_TARGET_ACTOR:
            if (owner_ == magic->target.dereference()) break;
          case MagicDescriptionStageTargeting::AOE_LOCATIONBOUND_OFFENSIVE:
          case MagicDescriptionStageTargeting::NONCASTER_TARGET_ACTOR:
          case MagicDescriptionStageTargeting::PROJECTILE:
            owner_->faceLocation(magic->x, magic->y);
            break;
        }
      }
    }

    *freeze = magic->description->casting.freeze;
    *motion_timeout = time + motion_delay;
    *repeat_timeout = time + action_delay;
  }

  return result;
}


void MagicGenerator::stop() {
  if (current_instance_) {
    --current_instance_->references;
    current_instance_ = NULL;
  }
  times_to_generate_ = 0;
}

}