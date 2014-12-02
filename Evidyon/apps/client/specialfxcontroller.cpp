#include "specialfxcontroller.h"

namespace Evidyon {

void SpecialFXController::destroy(GlobalSpecialFXManager* manager) {
}

bool SpecialFXController::update(double time,
          double time_since_last_update,
          GlobalSpecialFXManager* manager) {
  return true;
}


bool SpecialFXController::render(int texture_id,
          IsometricCamera* camera,
          GlobalSpecialFXManager* manager) {
  return true;
}


bool SpecialFXController::renderFX(int image_id,
          IsometricCamera* camera,
          GlobalSpecialFXManager* manager) {
  return true;            
}



}