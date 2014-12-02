#include "actorcontroller.h"
#include "../common/null.h"

namespace Evidyon {

Client* ActorController::getClient() const {
  return NULL;
}


bool ActorController::instantiateMagic(int source, const Magic* magic,
  int* times_to_generate, double* motion_delay, double* action_delay,
  int* visualization_power_parameter,
  MagicDescriptionStageParameters stage_parameters[MagicDescription::NUM_STAGES]) {
  return false;
}

}