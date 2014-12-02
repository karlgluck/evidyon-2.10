#ifndef __NORMALMELEEMONSTERLIFEFORMAICONTROLLER_H__
#define __NORMALMELEEMONSTERLIFEFORMAICONTROLLER_H__

#include "actorcontroller.h"
#include "actorpointer.h"
#include "meleecombatcontroller.h"

namespace Evidyon {

struct LifeformAIDescription_NormalMeleeMonster;
class GlobalActorManager;
class Client;

class NormalMeleeMonsterLifeformAIController : public ActorController {
  enum State {
    WAITING,
    PERSUING,
    NAVIGATING,
  };

public:
  void acquire(const ActorPointer* actor,
               int level,
               unsigned int treasure_randomizer_mask,
               const LifeformAIDescription_NormalMeleeMonster* description,
               bool delay_activity);
  void release(GlobalActorManager* actor_manager);

  void update(double time, double time_since_last_update);

public: // control methods
  inline void changeTarget(const ActorPointer* target);
  inline Actor* getActor();
  inline Actor* getTarget();
  inline bool canPerformAction() const;
  inline bool isDead() const;

  // If this method puts the actor's HP below zero, it sets the update counter
  // to a value.  When the counter reaches zero, the monster should be erased.
  void changeHP(Actor* source, int amount);


public: // overrides
  virtual void applyMagicEffect(Actor* source,
                                double time,
                                const Magic* magic,
                                const MagicDescriptionStageEffect* effect,
                                const MagicDescriptionStageEffect::Parameters* effect_parameters);
  virtual void fillStateDescription(ActorStateDescriptionData* state);
  virtual const char* getName();
  virtual int instantiateMeleeAttack(Actor* target,
                                     MeleeAttack* attack);
  virtual bool block(Actor* attacker,
                     const MeleeAttack* attack);
  virtual void applyMeleeAttack(Actor* attacker,
                                MeleeAttack* attack);

protected:
  int hp_, level_;
  //int actor_type_;  // selected from the description at creation
  ActorPointer actor_;
  const LifeformAIDescription_NormalMeleeMonster* description_;

  // used to drop treasure on the ground
  unsigned int treasure_randomizer_mask_;

  // since each update takes a known amount of time, movement and attack delays
  // are defined by setting these counters.  each update, the counters are
  // decremented; if the values are less than or equal to 0, the actor can
  // move/attack/cast a spell/etc.
  int updates_until_next_allowed_action_;
  State current_state_;
  bool navigate_clockwise_;

  // Handles combat!
  MeleeCombatController melee_combat_controller_;
};


Actor* NormalMeleeMonsterLifeformAIController::getActor() { return actor_.dereference(); }
bool NormalMeleeMonsterLifeformAIController::canPerformAction() const { return updates_until_next_allowed_action_ <= 0; }
bool NormalMeleeMonsterLifeformAIController::isDead() const { return hp_ < 0; }



}


#endif