#include "magictargetertimer.h"
#include <memory.h>
#include "actor.h"
#include "globalactormanager.h"

#include <dc/debug>

namespace Evidyon {

void MagicTargeterTimer::initializeTimer(Actor** targets,
  int number_of_targets, double time_to_apply) {
  if (number_of_targets > MAX_TARGETS) number_of_targets = MAX_TARGETS;
  GlobalActorManager* manager = GlobalActorManager::singleton();
  for (int i = 0; i < number_of_targets; ++i) {
    manager->getPointer(targets[i], &targets_[i]);
  }
  time_to_apply_ = time_to_apply;
  number_of_targets_ = number_of_targets;
}

bool MagicTargeterTimer::update(GlobalMagicManager* magic_manager, double time) {
  if (time_to_apply_ > time) return false;

  MagicDescriptionStageEffect::Type type;
  const MagicDescriptionStageEffect::Parameters* params;
  const MagicDescriptionStageEffect* effect =
      MagicTargeter::getStageEffect(&params);

  Actor* actor_targets[MAX_TARGETS];
  int number_of_targets = number_of_targets_;
  for (int i = 0; i < number_of_targets; ++i) {
  if (targets_[i].invalid()) continue;
  Actor* target = targets_[i].dereferenceAssumingValid();
  actor_targets[i] = target;
  target->applyMagicEffect(getCaster(), time, getMagic(), effect, params);
  }

  spawnNextStageTargeters(time, magic_manager, actor_targets, number_of_targets);

  resetTimerTargets();

  return true;
}

void MagicTargeterTimer::resetTimerTargets() {
  for (int i = 0; i < MAX_TARGETS; ++i) {
  targets_[i].reset();
  }
}

}