#ifndef __SPECIALFXBINDING_PROJECTILE_H__
#define __SPECIALFXBINDING_PROJECTILE_H__

#include "specialfxbinding.h"

namespace Evidyon {


class SpecialFXBinding_Projectile : public SpecialFXBinding {
public:
  SpecialFXBinding_Projectile(double expiration_time,
                              float x,
                              float y,
                              float dx,
                              float dy,
                              int termination_fx,
                              int termination_sound);
  virtual void update(double time, double time_since_last_update);
  virtual void get(float* x, float* y, float* z);

  bool isTimedOut(double current_time) const;
  int getTerminationFX() const;
  int getTerminationSound() const;


private:
  double expiration_time_;
  float x_, y_, dx_, dy_;
  int termination_fx_, termination_sound_;
};


}


#endif