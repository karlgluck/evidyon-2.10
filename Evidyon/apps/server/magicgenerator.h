#ifndef __MAGICGENERATOR_H__
#define __MAGICGENERATOR_H__

namespace Evidyon {

struct Magic;
class Actor;
struct InstantiatedMagic;
class GlobalMagicManager;

// ex. progressions:
// fireball:
//  IDLE->TARGETING(create ProjectileTargeter)IDLE
// multi-fireball:
//  IDLE->TARGETING(create ProjectileTargeter)->REPEAT_DELAY->
//  TARGETING->...->IDLE
// single target beam:
//  IDLE->TARGETING(create visualization, create SingleTargetDelayedTargeter)->IDLE
// multitarget beam:
//  IDLE->TARGETING(create visualizations, create MultiTargetDelayedTargeter)->IDLE
class MagicGenerator {
public:
  MagicGenerator();
  ~MagicGenerator();

  // Initializes this class with its managing Actor
  void reset(Actor* owner);

  // Advances this generator.  Should be called by the owning Actor.
  void update(GlobalMagicManager* manager, double time);

  // Invoked by the MagicController when it wants to cast a spell.  This should
  // not be used anywhere else.  It is not a protected method:  it will
  // interrupt any spell currently being executed.
  // Input:
  //  time - current time
  //  source - if <=0, defines the MP cost to cast this spell; 
  //           if >0, defines 1+[consumable item index] that generates this spell
  //  magic - spell to cast
  // Output:
  //  *freeze == true:   freeze the actor
  //      == false:  root the actor
  //  *motion_timeout:   time at which to leave frozen or rooted state
  //  *repeat_timeout:   time at which to leave repeat delay state
  // Returns:
  //  'true' if the spell was cast
  //  'false' ignore all output parameters
  bool cast(double time,
            int source,
            Magic* magic,
            bool* freeze,
            double* motion_timeout,
            double* repeat_timeout);

  // Cancels any spell that this generator is in the process of creating
  void stop();


private:
  Actor* owner_;

  // The next time at which to cast the spell's first stage.  Used for
  // spells with repeated generation, such as a multiple-fireball.
  double next_generation_time_;

  // The number of times left to instantiate the current spell's first stage
  int times_to_generate_;

  // The instance being advanced
  InstantiatedMagic* current_instance_;
};



}

#endif