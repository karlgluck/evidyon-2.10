#ifndef __AIMELEEATTACK_H__
#define __AIMELEEATTACK_H__


#include <dc/float>
#include <dc/enum>
#include <dc/integer>
#include "../shared/evidyon_actor.h"
#include "leveldefinitions.h"


namespace Evidyon {
struct AIMeleeAttackDescription;
}


class AIMeleeAttack : public dc::dcResource<AIMeleeAttack> {
public:
  AIMeleeAttack();
  void compile(const LevelAttributes* intended_level_attributes,
               int intended_level, // the level for which this attack is configured
               float time_between_attacks, // specified by the AI lifeform
               const float attack_durations[Evidyon::ACTORATTACK_COUNT],
               Evidyon::AIMeleeAttackDescription* description) const;

public:
  static std::string staticTypeName();

private:
  dc::dcFloat range_;
  dc::dcEnum<Evidyon::ActorAttack> attack_;

  // Percent of target's HP dealt in damage per second (min & max) for
  // this attack.  The actual min/max damage values are calculated based
  // on the time between attacks and the average HP of a character
  // at the intended level.
  dc::dcInteger percent_dps_min_, percent_dps_max_;
};



#endif

