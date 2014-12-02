#ifndef __INTERACTIVENPCLIFEFORMAICONTROLLER_H__
#define __INTERACTIVENPCLIFEFORMAICONTROLLER_H__

#include "actorcontroller.h"
#include "actorpointer.h"

namespace Evidyon {

struct LifeformAIDescription_InteractiveNPC;
class GlobalActorManager;
class Client;

class InteractiveNPCLifeformAIController : public ActorController {
public:
  void acquire(const ActorPointer* actor,
               const LifeformAIDescription_InteractiveNPC* description);
  void release(GlobalActorManager* actor_manager);
  void update(double time, double time_since_last_update);

public: // control methods
  inline void changeTarget(const ActorPointer* target);
  inline Actor* getActor();

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
  ActorPointer actor_;
  float root_x_, root_y_;
  double next_movement_time_;
  const LifeformAIDescription_InteractiveNPC* description_;
};

Actor* InteractiveNPCLifeformAIController::getActor() { return actor_.dereference(); }




}


#endif