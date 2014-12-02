#include "specialfxbinding_projectile.h"
#include "../common/null.h"


namespace Evidyon {

SpecialFXBinding_Projectile::SpecialFXBinding_Projectile(
    double expiration_time,
    float x,
    float y,
    float dx,
    float dy,
    int termination_fx,
    int termination_sound) {
  expiration_time_ = expiration_time;
  x_ = x;
  y_ = y;
  dx_ = dx;
  dy_ = dy;
  termination_fx_ = termination_fx;
  termination_sound_ = termination_sound;
}


void SpecialFXBinding_Projectile::update(double time, double time_since_last_update) {
  if (time > expiration_time_) setExpired();
  x_ += time_since_last_update * dx_;
  y_ += time_since_last_update * dy_;
}


void SpecialFXBinding_Projectile::get(float* x, float* y, float* z) {
  if (x != NULL) *x = x_;
  if (y != NULL) *y = y_;
}


bool SpecialFXBinding_Projectile::isTimedOut(double current_time) const {
  return current_time > expiration_time_;
}


int SpecialFXBinding_Projectile::getTerminationFX() const {
  return termination_fx_;
}


int SpecialFXBinding_Projectile::getTerminationSound() const {
  return termination_sound_;
}

}