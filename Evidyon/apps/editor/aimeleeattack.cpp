#include "aimeleeattack.h"
#include "../shared/server-editor/lifeformaidescription.h"
#include "../shared/evidyon_actor.h"


AIMeleeAttack::AIMeleeAttack() {
  member("Range", &range_);
  member("Attack", &attack_);
  member("Min % Target HP Damage Per Second", &percent_dps_min_);
  member("Max % Target HP Damage Per Second", &percent_dps_max_);
}

void AIMeleeAttack::compile(const LevelAttributes* intended_level_attributes,
                            int intended_level,
                            float time_between_attacks,
                            const float attack_durations[Evidyon::ACTORATTACK_COUNT],
                            Evidyon::AIMeleeAttackDescription* description) const {
  description->range = range_.getValue();
  description->action = static_cast<Evidyon::ActorAction>(static_cast<int>(Evidyon::ACTORACTION_ATTACK1) + attack_.getValue());

  float duration = attack_durations[attack_.getValue()];
  float repeat_time = time_between_attacks + duration;

  int dps_min = percent_dps_min_.getValue() / 100.0f * intended_level_attributes->getAverageHP();
  int dps_max = percent_dps_max_.getValue() / 100.0f * intended_level_attributes->getAverageHP();

  float min_damage = dps_min * repeat_time;
  float max_damage = dps_max * repeat_time;
  int random_damage = (int)(max_damage - min_damage);

  // additional damage per second has been disabled
  float additional_damage_per_level = 0.0f; // repeat_time * additional_dps_per_level_.getValue();

  float base_damage = min_damage - intended_level * additional_damage_per_level;
  if (base_damage < 1.0f) {
    base_damage = 1.0f;
    additional_damage_per_level = (min_damage - 1) / intended_level;

    // if the slope is too shallow, prevent integer roundoff error
    if (additional_damage_per_level < 1.0f) {
      additional_damage_per_level = 0.0f;
      base_damage = min_damage;
    }
  }

  description->duration = duration;
  description->base_damage = base_damage > 1.0f ? ((int)base_damage) : 1;
  description->random_additional_damage = random_damage > 0 ? random_damage : 0;
  description->added_damage_per_level = (int)additional_damage_per_level;
}

std::string AIMeleeAttack::staticTypeName() {
  return "AIMeleeAttack";
}

