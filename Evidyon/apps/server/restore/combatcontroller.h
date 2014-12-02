//------------------------------------------------------------------------------------------------
// File:    combatcontroller.h
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __COMBATCONTROLLER_H__
#define __COMBATCONTROLLER_H__

class Actor;

struct AttackData {
  bool blocked;
  int damage;
};

/**
 * Handles physical combat.  This class handles combat timing and flow only;
 * it has no concept of spatial separation, offensive/defensive capability,
 * special abilities, etc.  All of that should be handled by the Actor.
 */
class CombatController
{
public:
  CombatController();

  void setOwner(Actor* owner);
  void update(double time);

  // Changes the target that this controller is attacking.  This method should
  // be used with care--always find out if the controller is busy before
  // using this method, otherwise the results may be nonsensical.
  void attack(Actor* target);

  // Returns true if the controller is currently executing an attack or is
  // blocking an attack.  The controller is not busy when idle or it has
  // completed an attack and has not yet been assigned its next target.
  bool busy(double time);

  // This will wake up the controller if it is idle.  This method is invoked
  // by another CombatController when this controller's owner is being
  // attacked.
  bool block(Actor* attacker, double time);

private:

  void enterBlockingAttackState(double time);

  void update_ActionDelay(double time);
  void update_BlockingAttackDelay(double time);
  void update_BlockedSwingDelay(double time);
  void update_UnblockedSwingDelay(double time);
  void update_UnblockedSwingDelayDamageGenerated(double time);
  void update_Recovering(double time);

  // If the controller is IDLE, move into ACTION_DELAY
  void wakeUp();


private:

  enum CurrentState {
    BLOCKING_ATTACK_DELAY,
    BLOCKED_SWING_DELAY,
    UNBLOCKED_SWING_DELAY, // waiting to send a message to the target's client
    UNBLOCKED_SWING_DELAY_DAMAGE_GENERATED,
    RECOVERING,
    ACTION_DELAY,
    IDLE,
  };

  // The actor that contains this controller
  Actor* owner_;

  // The actor that this controller is currently attacking, and the one to
  // start attacking on the next attack round.
  Actor* current_target_;

  // This flag is set if the actor should block an incoming attack from
  // the actor in attack_source_
  bool blocking_attack_;
  Actor* attack_source_;
  
  CurrentState current_state_;

  double time_;
  AttackData current_attack_;
};

#endif