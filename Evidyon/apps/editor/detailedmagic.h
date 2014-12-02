#ifndef __DETAILED_MAGIC_H__
#define __DETAILED_MAGIC_H__

#include "magic.h"
#include "../shared/server-editor/magicdescription.h"
#include <dc/enum>
#include <dc/float>
#include <dc/boolean>
#include <dc/list>
#include "magicfx.h"
#include "projectile.h"
#include "enchantment.h"

namespace Evidyon {

class DetailedMagicStage : public dc::dcResource<DetailedMagicStage> {
public:
  DetailedMagicStage();
  void compileForServer(MagicDescriptionStage* description);

public:
  static std::string staticTypeName();

private:
  dc::dcEnum<MagicDescriptionStageTargeting::Type> targeting_type_;
  dc::dcInteger targets_;
  dc::dcFloat delay_;
  dc::dcFloat spread_;
  dc::dcEnum<MagicDescriptionStageEffect::Type> effect_type_;
  dc::dcFloat duration_;
  dc::dcFloat float_param_;
  dc::dcInteger int_param_;

  // If the effect is an enchantment, the referenced object is used
  // to fill out enchantment parameters
  dc::dcReference<Enchantment> enchantment_;

  dc::dcList<MagicFX>::Reference aoe_fx_;

  // Depending on the targeting type, one of these is used
  dc::dcList<MagicFX>::Reference per_targeter_magic_fx_;
  dc::dcList<Projectile>::Reference per_targeter_projectile_fx_;
};


// Detailed magic lets the editor specify every part of a
// magic instance's definition by hand.
class DetailedMagic : public dc::dcResource<DetailedMagic, Magic> {
public:
  DetailedMagic();
  virtual void compileForServer(MagicDescription* description);

public:
  static std::string staticTypeName();

private:
  dc::dcList<MagicFX>::Reference magic_fx_;
  dc::dcEnum<ActorAction> casting_action_;
  dc::dcEnum<MagicDescription::PowerType> power_type_;
  dc::dcBoolean freeze_;
  dc::dcFloat movement_delay_;
  dc::dcFloat essential_action_delay_;
  dc::dcFloat balancing_action_delay_;
  DetailedMagicStage stages_[Evidyon::MagicDescription::NUM_STAGES];

private:
  /// This class implements the Magic interface
  static dc::dcClass<DetailedMagic>::Implements<Magic> implements_magic_;
};


}

#endif