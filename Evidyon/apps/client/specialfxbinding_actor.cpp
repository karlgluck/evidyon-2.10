#include "specialfxbinding_actor.h"
#include "../common/null.h"
#include "actor.h"


namespace Evidyon {


SpecialFXBinding_Actor::SpecialFXBinding_Actor(Actor* actor) {
  actor_ = actor;
}

void SpecialFXBinding_Actor::update(double time, double time_since_last_update) {
  if (getExpired()) {
    actor_ = NULL;
  }
}

void SpecialFXBinding_Actor::get(float* x, float* y, float* z) {
  if (actor_ == NULL) return;
  if (x != NULL) *x = actor_->getX();
  if (y != NULL) *y = actor_->getY();
}


}