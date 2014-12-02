#ifndef __SPECIALFXBINDING_ACTOR_H__
#define __SPECIALFXBINDING_ACTOR_H__

#include "specialfxbinding.h"

namespace Evidyon {

class Actor;

class SpecialFXBinding_Actor : public SpecialFXBinding {
public:
  SpecialFXBinding_Actor(Actor* actor);
  virtual void update(double time, double time_since_last_update);
  virtual void get(float* x, float* y, float* z);

private:
  Actor* actor_; // valid as long as the binding has not expired
};

}


#endif