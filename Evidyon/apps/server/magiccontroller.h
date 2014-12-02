#ifndef __MAGICCONTROLLER_H__
#define __MAGICCONTROLLER_H__

#include "magic.h"

namespace Evidyon {

class MagicGenerator;

/*
Cast Spell:
IDLE:
  -> CAST_SPELL
CAST_SPELL:
  -> DELAY_REPEAT
  -> DELAY_FREEZE_REPEAT
  -> DELAY_REPEAT_FREEZE
  -> DELAY_ROOT_REPEAT
  -> DELAY_REPEAT_ROOT
DELAY_REPEAT:
  -> IDLE
DELAY_FREEZE_REPEAT:
  -> DELAY_REPEAT
DELAY_REPEAT_FREEZE:
  -> DELAY_FREEZE
DELAY_FREEZE:
  -> IDLE
  -> CAST_SPELL
DELAY_ROOT_REPEAT:
  -> DELAY_REPEAT
DELAY_REPEAT_ROOT:
  -> DELAY_ROOT
DELAY_ROOT:
  -> CAST_SPELL
  -> IDLE
*/
class MagicController {
  enum State {
    IDLE,         // not casting a spell
    CAST_SPELL,       // cast the spell with the current parameters
    DELAY_REPEAT,     // wait until the timer expires, then go idle
    DELAY_FREEZE_REPEAT,  // prevent any actor motion or ability use, then go into a repeat delay
    DELAY_REPEAT_FREEZE,  // prevent any ability use or actor motion, then go into a freeze
    DELAY_FREEZE,     // prevent any actor motion, but allow magic to be used (goto CAST_SPELL)
    DELAY_ROOT_REPEAT,  // allow the actor to turn but not move and prevent ability use, then go into repeat delay
    DELAY_REPEAT_ROOT,  // allow the actor to turn but not move and execute a repeat delay, then go into a rooted state
    DELAY_ROOT,       // allow the actor to turn but not move, but allow magic to be used (goto CAST_SPELL)
  };

public:
  MagicController();

  // Clears all members of this class.  Must be called with a valid generator
  // before the class can be used.
  void reset(MagicGenerator* generator);

  // Advances this controller.  Returns 'true' if the casting state changed.
  // The return value is a cue to the casting entity that it should check
  // to see whether it is now rooted or frozen.
  bool update(double time);

  // The controller is busy if it is not in the idle state.  All other actions
  // (such as physical attacks/blocking/etc) should be prevented while busy.
  bool busy() const;

  // How long until the controller will be able to cast a spell that is queued.
  // A return value less than or equal to 0 means that a queued spell will be
  // cast when the next update(time) is called.
  double timeToCast(double time) const;

  // Queues magic to be used on the next update that the controller is in a
  // valid state.  Passing NULL forces nothing to be cast, and can stop the
  // caster from completing some actions (ex: attack run)
  // MP use and effectiveness calculations will be done by the magic generator
  void cast(int source, const Magic* magic);

  // If this method returns true, the caster should not be allowed to change
  // its movement speed, but should be allowed to turn.  This method will
  // always return true if frozen() returns true.
  bool rooted() const;

  // If this method returns true, the caster should not be allowed to change
  // its movement speed or direction.
  bool frozen() const;

  // Ceases casting and forces this controller to become idle
  void stop();

public:

  // Makes the actor not able to do anything until the unfreeze time is hit.  This
  // is useful beyond just controlling magic--it was originally implemented to be
  // invoked during a map-change to prevent jumping back and forth between maps.
  void freeze(double time_to_unfreeze);


private:
  State state_;
  double motion_timeout_; // time at which to exit freeze/root state
  double repeat_timeout_; // time to leave repeat state
  bool cast_spell_;     // whether or not spell_to_cast_ is valid
  Magic spell_to_cast_;   // the magic to use
  int spell_source_;    // where the spell comes from:
                        // <= 0 indicates an MP cost
                        // >0 defines 1 + the index of a consumable item in inventory

  // Instantiates a cast spell.  For example, creates fireball projectiles,
  // finds targets for area effects and handles chained spells.
  MagicGenerator* generator_;
};


}

#endif