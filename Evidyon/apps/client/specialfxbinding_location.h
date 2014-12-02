#ifndef __SPECIALFXBINDING_LOCATION_H__
#define __SPECIALFXBINDING_LOCATION_H__

#include "specialfxbinding.h"

namespace Evidyon {


class SpecialFXBinding_Location : public SpecialFXBinding {
public:
  SpecialFXBinding_Location(float x, float y);
  virtual void update(double time, double time_since_last_update);
  virtual void get(float* x, float* y, float* z);

private:
  float x_, y_;
};

}


#endif