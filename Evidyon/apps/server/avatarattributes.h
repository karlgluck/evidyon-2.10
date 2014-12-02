#ifndef __AVATARATTRIBUTES_H__
#define __AVATARATTRIBUTES_H__

#include "../shared/evidyon_avatar.h"

namespace Evidyon {

enum Gender {
  MALE,
  FEMALE
};

struct AvatarAttributes {
  char name[MAX_AVATAR_NAME_LENGTH];
  int genderIndex, raceIndex, classIndex;
  int currentEXP;
  int next_level_EXP;
  int stats[5]; // str agi con int wis
  int availableStatPoints;
  int money, geonite;
  int hp, mp;

  // calculated from the physical/magical defense
  float physical_damage_factor, magical_damage_factor;

  // casting bonuses
  float offensive_magic_bonus, defensive_magic_bonus;

  // these values can be changed by enchantments or items
  SpellAvailabilityMask spell_availability;
  struct Enchantable {
    int hp_regen_rate,
        mp_regen_rate;
    int max_hp, max_mp;
    int level;
    ActorTypeIndex actor_type;
    float speed, attack_delay_time;
    float casting_delay_percent;
    int min_attack_damage, random_attack_damage;
    int physical_defense, magical_defense;
  } base, current;
};



}

#endif