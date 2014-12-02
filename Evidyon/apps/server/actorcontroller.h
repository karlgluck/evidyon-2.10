#ifndef __ACTORCONTROLLER_H__
#define __ACTORCONTROLLER_H__

#include "../shared/server-editor/magicdescription.h"

namespace Evidyon {

struct Magic;
struct ActorStateDescriptionData;
struct MeleeAttack;
struct MeleeBlock;
class Actor;
class Client;

class ActorController {

public:
  // This method only returns a non-NULL pointer if the controller is a client.  The
  // default implementation returns 'NULL'
  virtual Client* getClient() const;

  inline bool isClient() const { return 0 != getClient(); }


  virtual void applyMagicEffect(
      Actor* source, double time, const Magic* magic,
      const MagicDescriptionStageEffect* effect,
      const MagicDescriptionStageEffect::Parameters* effect_parameters) = 0;

  virtual bool instantiateMagic(int source,
                                const Magic* magic,
                                int* times_to_generate,
                                double* motion_delay,
                                double* action_delay,
                                int* visualization_power_parameter,
                                MagicDescriptionStageParameters stage_parameters[MagicDescription::NUM_STAGES]);

  virtual void fillStateDescription(ActorStateDescriptionData* state) = 0;
  virtual const char* getName() = 0;

  // Fills the structure with attack data.  This method is allowed to turn
  // the actor to face its target.
  // Return values:
  // -1: the actor is busy with something else (spell, etc)
  //  0: successfully filled attack; proceed with calculations
  //  1: target is currently out of range
  virtual int instantiateMeleeAttack(Actor* target,
                                     MeleeAttack* attack) = 0;

  // Returns 'true' if the actor has blocked the given attack.
  // For standard usage, this method should see if the attack could possibly
  // be blocked, then check its internal state and use the melee controller
  // to execute the action.
  virtual bool block(Actor* attacker,
                     const MeleeAttack* attack) = 0;

  // Hits this actor with the specified attack.  This method should check
  // to see if the attack is still valid (it is in range, the source is
  // facing the correct direction, etc.)
  virtual void applyMeleeAttack(Actor* attacker,
                                MeleeAttack* attack) = 0;
};


}

#endif