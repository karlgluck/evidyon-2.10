#include "meleecombatcontroller.h"
#include "meleeattack.h"
#include "actor.h"
#include "actorcontroller.h"
#include "time.h"
#include "../common/null.h"

#include <dc/debug>


namespace Evidyon {


void MeleeCombatController::reset(Actor* owner) {
  owner_ = owner;
  action_delay_ = 0.0;
  state_ = IDLE;
  state_update_time_ = 0.0;
  current_target_.reset();
  next_target_.reset();
}


void MeleeCombatController::setActionDelay(double seconds) {
  action_delay_ = seconds;
}


void MeleeCombatController::update(double time) {
  switch (state_) {
    default:
    case IDLE: break;


    case ACTION_DELAY: {
      if (time < state_update_time_) break;

      // copy in the next attack target
      current_target_.copy(&next_target_);
      Actor* target = current_target_.dereference();
      if (target == NULL || target->isDead()) {
        state_ = IDLE;
        break;
      }
      owner_->setCombatState(true);

      // try to hit this target
      ActorController* owner_controller = owner_->getController();
      int result = owner_controller->instantiateMeleeAttack(target,
                                                           &current_attack_);
      if (result == -1) { // the actor is busy with something else
        break;
      } else if (result == 1) {
        // the actor is out of range; enter attack mode!  this pose
        // should raise the actor's weapon, and indicate that they are
        // going to be attacking something.
        // it is intentional that this is set each loop--it needs to
        // constantly be set in the actor syncs.
        break;
      }

      if (target->block(owner_,
                       &current_attack_)) {
        state_ = ATTACKING_BLOCKED;
        owner_->setAction(current_attack_.blocked_action);        
      } else {
        state_ = ATTACKING_UNBLOCKED;
        owner_->setAction(current_attack_.swing_action);
      }
      state_update_time_ = time + current_attack_.duration;

    } break;


    case ATTACKING_BLOCKED: {
      if (time > state_update_time_) {
        state_ = ACTION_DELAY;
        state_update_time_ = time + action_delay_;
      } else {
        //owner_->setAction(current_attack_.swing_action);
        owner_->setCombatState(true);
      }
    } break;


    case ATTACKING_UNBLOCKED: {
      if (time > state_update_time_) {
        state_ = ACTION_DELAY;
        state_update_time_ = time + action_delay_;

        // apply damage
        Actor* target = current_target_.dereference();
        if (!target) break;
        ActorController* target_controller = target->getController();
        target_controller->applyMeleeAttack(owner_,
                                           &current_attack_);

      } else {
        //owner_->setAction(current_attack_.swing_action);
        owner_->setCombatState(true);
      }
    } break;


    case BLOCKING_ATTACK: {
      if (time > state_update_time_) {
        state_ = ACTION_DELAY;
        state_update_time_ = time + action_delay_;
      } else {
        //owner_->setAction(current_block_.block_action);
        owner_->setCombatState(true);
      }
    } break;
  }
}

void MeleeCombatController::attack(const ActorPointer* target) {
  next_target_.copy(target);

  if (NULL == current_target_.dereference()) {
    current_target_.copy(target);
  }

  if (state_ == IDLE && target != NULL) {
    state_ = ACTION_DELAY;
    state_update_time_ = 0.0;
  }
}

bool MeleeCombatController::pursuingTarget() const {
  const Actor* next_target = next_target_.dereference();
  return state_ == ACTION_DELAY &&
         (next_target != NULL) &&
         (next_target->isAlive());
}

bool MeleeCombatController::busy() const {
  return !(state_ == IDLE || state_ == ACTION_DELAY);
}


bool MeleeCombatController::frozen() const {
  return state_ == BLOCKING_ATTACK ||
         state_ == ATTACKING_BLOCKED ||
         state_ == ATTACKING_UNBLOCKED;
}

Actor* MeleeCombatController::getTarget() {
  return current_target_.dereference();
}

void MeleeCombatController::stop() {
  state_ = IDLE;
  owner_->setCombatState(false);
  state_update_time_ = 0.0;
  current_target_.reset();
  next_target_.reset();
}

bool MeleeCombatController::block(Actor* attacker,
                                  const MeleeAttack* attack) {
  switch (state_) {
    case IDLE:
    case ACTION_DELAY:
    case BLOCKING_ATTACK:
      owner_->setCombatState(true);
      //state_ = BLOCKING_ATTACK;
      //state_update_time_ = Time::current_frame + attack->duration;
      return false;

    default: // if attacking, can't also block
      return false;
  }
}





}