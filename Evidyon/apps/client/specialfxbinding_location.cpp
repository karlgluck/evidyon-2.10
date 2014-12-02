#include "specialfxbinding_location.h"
#include "../common/safemacros.h"
#include "../common/null.h"


namespace Evidyon {

SpecialFXBinding_Location::SpecialFXBinding_Location(float x, float y) {
  x_ = x;
  y_ = y;
}


void SpecialFXBinding_Location::update(double time, double time_since_last_update) {

}


void SpecialFXBinding_Location::get(float* x, float* y, float* z) {
  if (x != NULL) *x = x_;
  if (y != NULL) *y = y_;
}


}