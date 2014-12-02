#ifndef __MAGICTARGETERTIMER_H__
#define __MAGICTARGETERTIMER_H__

#include "magictargeter.h"

#include "actorpointer.h"

namespace Evidyon {

class MagicTargeterTimer : public MagicTargeter {
public:
  static const int MAX_TARGETS = 8;

public:

  // Sets up the targeter for use
  void initializeTimer(Actor** targets, int number_of_targets, double time_to_apply);

  // This method is responsible for applying the effect of the targeter via
  // the manager, and for creating subsequent targeters if necessary.
  // Returns:
  //  'true' if this targeter has expired
  bool update(GlobalMagicManager* magic_manager, double time);

public:
  void resetTimerTargets();

private:
  ActorPointer targets_[MAX_TARGETS];
  int number_of_targets_;
  double time_to_apply_;
};



}

#endif