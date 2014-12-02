#ifndef __MELEECOMBATCONTROLLER_H__
#define __MELEECOMBATCONTROLLER_H__


#include "actorpointer.h"
#include "meleeattack.h"
#include "meleeblock.h"

namespace Evidyon {

class Actor;


// part of an Actor.  only controls hand-to-hand combat.
class MeleeCombatController {
  enum State {
    BLOCKING_ATTACK,      // currently blocking another melee attack
    ATTACKING_BLOCKED,    // goes back to combat state when expired
    ATTACKING_UNBLOCKED,  // calculates attack when expired
    ACTION_DELAY,         // after attacking or blocking.  not a busy state.
    IDLE,
  };

public:
  void reset(Actor* owner);

  // Changes the amount of time that this controller waits between the end
  // of one action (attack/block) and the beginning of the next attack.
  void setActionDelay(double seconds);

  // Advances the controller's state
  void update(double time);

  // Changes the target that will be attacked next.
  void attack(const ActorPointer* target);

  // Returns 'true' if the controller has a valid target and would enter the
  // ATTACK state, but it is out of range or the owning actor is busy.  This
  // should be checked so that the controller can move closer to the target.
  bool pursuingTarget() const;

  // All other actions (such as casting spells) should be prevented while busy
  bool busy() const;

  // If a controller is blocking, it is frozen: the actor must stay where it
  // is and at its current orientation.  Attacking actors can move.
  bool frozen() const;

  // Guaranteed to return a valid actor if pursuingTarget returned 'true'
  // after a recent call to update().
  Actor* getTarget();

  // Erases the state of this controller, but maintains the owner actor
  // and other settings.  Use this when changing maps.
  void stop();


private:
  // Checks the internal state and the actor controller to see if it can 
  // block an attack.  If the attack is blocked, this method returns 'true'.
  // This is called by the attacker's combat controller.
  bool block(Actor* attacker, const MeleeAttack* attack);


private:
  Actor* owner_;
  double action_delay_;

  // Handles combat progression
  State state_;
  double state_update_time_;

  // The next target is moved into the current target each time ACTION_DELAY
  // expires.  If the target exists, this change causes the controller to
  // try to attack the target.
  ActorPointer current_target_;
  ActorPointer next_target_;

  MeleeAttack current_attack_;
  MeleeBlock current_block_;
};


}




#endif