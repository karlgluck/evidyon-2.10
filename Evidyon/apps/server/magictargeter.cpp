#include "magictargeter.h"
#include "instantiatedmagic.h"
#include "../common/null.h"
#include "globalmagicmanager.h"

namespace Evidyon {

MagicTargeter::MagicTargeter() {
  magic_instance_ = NULL;
  stage_ = MagicDescription::NUM_STAGES;
}

void MagicTargeter::initializeMagic(InstantiatedMagic* magic_instance, int stage) {
  magic_instance_ = magic_instance;
  stage_ = stage;
  ++magic_instance_->references;
}


void MagicTargeter::reset() {
  if (magic_instance_) {
  --magic_instance_->references;
  magic_instance_ = NULL;
  }
  stage_ = MagicDescription::NUM_STAGES;
}


void MagicTargeter::spawnNextStageTargeters(double time,
  GlobalMagicManager* manager, Actor** sources, int num_sources) {
  int stage = stage_ + 1;
  if (magic_instance_ == NULL ||
    stage >= MagicDescription::NUM_STAGES ||
    num_sources < 1) return;
  //switch (magic_instance_->magic.description->stages[stage].effect.type) {
  //  case MagicDescriptionStageTargeting::SELF: {
  //  } break;
  //  case MagicDescriptionStageTargeting::PROJECTILE: {
  //  } break;
  //  case MagicDescriptionStageTargeting::ANY_TARGET_ACTOR: {
  //  } break;
  //  case MagicDescriptionStageTargeting::NONCLIENT_TARGET_ACTOR: {
  //  } break;
  //  case MagicDescriptionStageTargeting::AOE_ACTORBOUND_CASTER: {
  //  } break;
  //  case MagicDescriptionStageTargeting::AOE_ACTORBOUND_CASTER_OFFENSIVE: {
  //  } break;
  //  case MagicDescriptionStageTargeting::AOE_ACTORBOUND_OTHER_OFFENSIVE: {
  //  } break;
  //  case MagicDescriptionStageTargeting::AOE_LOCATIONBOUND_OFFENSIVE: {
  //  } break;
  //  case MagicDescriptionStageTargeting::PERIODIC_AOE_LOCATIONBOUND_OFFENSIVE: {
  //  } break;
  //}
}


void MagicTargeter::spawnNextStageTargeters(double time,
  GlobalMagicManager* manager, float source_x, float source_y) {
  int stage = stage_ + 1;
  if (magic_instance_ == NULL || stage >= MagicDescription::NUM_STAGES) return;
}


const MagicDescriptionStageEffect* MagicTargeter::getStageEffect(
  const MagicDescriptionStageEffect::Parameters** parameters) {
  int stage = stage_;
  *parameters = &magic_instance_->stage_parameters[stage].effect;
  return &magic_instance_->magic.description->stages[stage].effect;
}


const Magic* MagicTargeter::getMagic() {
  return magic_instance_ == NULL ? NULL : &magic_instance_->magic;
}

Actor* MagicTargeter::getCaster() {
  return magic_instance_ == NULL ? NULL : magic_instance_->caster.dereference();
}


}