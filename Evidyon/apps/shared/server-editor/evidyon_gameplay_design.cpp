#include "evidyon_gameplay_design.h"

#include <math.h> // for "floor"


namespace Evidyon {
namespace GameplayDesign {

int avatarStatBonus(int stat_value) {
  static const int limits[] = { 15, 17, 19, 21, 25, 29, 34, 39, 45, 50, 61 };
  for (int i = 0; i < sizeof(limits) / sizeof(int); ++i) {
    if (stat_value < limits[i]) return i;
  }
  return 12 + (int)floor((stat_value - 61) / 10.0);
}

double avatarLevelPercent(int level) {
	if ((double)level/40.0 > 1.0) {
		return 1.0;
	} else {
		return (double)level/40.0;
	}
}

float avatarMovementSpeed(int agility) {
  const int limits[]   = {   10,   11,   13,   16,   20,   25,   31,   38,   46,   55,   65 };
  const float speeds[] = { 3.0f, 3.1f, 3.2f, 3.4f, 3.7f, 4.0f, 4.3f, 4.6f, 4.9f, 5.3f, 5.7f };
  const int num_limits = sizeof(limits)/sizeof(int);
  float speed = speeds[num_limits-1]; // assign the maximum speed as the default
  for (int i = 1; i < num_limits; ++i) {
    if (agility < limits[i]) {
      speed = speeds[i-1];
      break;
    }
  }
  speed += (agility-10) * 0.005f; // add a little bit of speed for each limiter
  return speed;
}

float avatarAttackDelay(int agility) {
  int bonus = avatarStatBonus(agility);
  if (bonus > 11) bonus = 11;
  const float ATTACK_DELAYS[12] = { 0.8f,  0.7f, 0.6f,  0.5f,
                                    0.45f, 0.4f, 0.35f, 0.3f,
                                    0.25f, 0.2f, 0.1f,  0.0f };
  return ATTACK_DELAYS[bonus];
}

float avatarCastingDelayPercent(int agility, int intelligence) {
  const double INTELLIGENCE_WEIGHT = 0.3;
  const double AGILITY_WEIGHT = 0.7;
  return 1.0f + AGILITY_WEIGHT/exp(agility/10.0) + INTELLIGENCE_WEIGHT/exp(intelligence/10.0);
}

float magicStatBonus(int level, int stat) {
  //const double BASE_POWER = 1.2;
  //const double SUBTRACTOR = 1.5216533333333;
  const double BASE_POWER = 1.105;
  const double SUBTRACTOR = 1.5;
  return pow(BASE_POWER, 5+avatarStatBonus(stat)) + level / 30.0 - SUBTRACTOR;
}

float avatarOffensiveMagicBonus(int level, int intelligence, int wisdom) {
  const float INTELLIGENCE_WEIGHT = 0.7f;
  const float WISDOM_WEIGHT = 0.3f;
  return magicStatBonus(level, intelligence) * INTELLIGENCE_WEIGHT +
         magicStatBonus(level, wisdom) * WISDOM_WEIGHT;
}

float avatarDefensiveMagicBonus(int level, int wisdom) {
  return magicStatBonus(level, wisdom);
}


int avatarHPRegenRate(int hp, int constitution) {
  double calc_hp_regen_rate = hp / 20.0;
  return ((calc_hp_regen_rate > 1.0 ? ((int)calc_hp_regen_rate) : 1));
}

int avatarMPRegenRate(int mp, int intelligence) {
  double calc_mp_regen_rate = mp / 14.0;
  return ((calc_mp_regen_rate > 1.0 ? ((int)calc_mp_regen_rate) : 1));
}

int avatarCoreHP(int level, int strength, int constitution) {
  static const double CONSTITUTION_WEIGHT = 1.0;
  static const double STRENGTH_WEIGHT = 0.25;
  const double stat_amount = (CONSTITUTION_WEIGHT*constitution + STRENGTH_WEIGHT*strength)*18.0;
  const double bonus_amount = (CONSTITUTION_WEIGHT*avatarStatBonus(constitution) + STRENGTH_WEIGHT*avatarStatBonus(strength))*8.0;
  return ((level*12.0) + stat_amount + bonus_amount) * avatarLevelPercent(level);
}

int avatarCoreMP(int level, int intelligence, int wisdom) {
  static const double WISDOM_WEIGHT = 1.0;
  static const double INTELLIGENCE_WEIGHT = 0.4;
  const double stat_amount = (WISDOM_WEIGHT*wisdom + INTELLIGENCE_WEIGHT*intelligence)*20.0;
  const double bonus_amount = (avatarStatBonus(wisdom)*WISDOM_WEIGHT + avatarStatBonus(intelligence)*INTELLIGENCE_WEIGHT)*20.0;
  return ((level*12.0) + stat_amount + bonus_amount)* avatarLevelPercent(level);
}

int avatarMeleeDamageBase(int strength, int level) {
  const int limits[]  = { 10,  11,  13,   16,   20,   25,   31,   38,   46,   55,   65 };
  const int damages[] = {  4,   6,   8,   12,   20,   42,   85,  130,  180,  240,  300 };
  const int num_limits = sizeof(limits)/sizeof(int);
  int min_damage = damages[num_limits-1]; // assign the maximum damage as the default
  for (int i = 1; i < num_limits; ++i) {
    if (strength < limits[i]) {
      min_damage = damages[i-1];
      break;
    }
  }
  return min_damage;
}

int avatarMeleeDamageAdditional(int strength, int level) {
  return (int)floor(strength / 4.0) + level;
}


float armorDamageFactor(int armor) {
  // this produces:
  //  90% damage - 70 armor
  //  75% damage - 109 armor
  //  50% damage - 162 armor
  //  30% damage - 265 armor
  //  25% damage - 350 armor
  //  17% damage - 1000 armor
  // The exponential threshold makes it so that the s-bend doesn't level
  // off the bonus from armor.  Normally, the exponent approaches zero so
  // really high armors would have no benefit.  This makes it so that
  // the curve declines indefinitely, approaching zero as 1/armor.
  // These parameters were derived to make the curve smooth in Excel.
  static const float e = 2.718281828f;
  const float sbend_armor = (1.0f - (1.0f/(1.0f+pow(e, armor/-30.0f + 4.0f)))) * 0.6f + 0.4f;
  static const int exp_threshold = 150;
  if (armor > exp_threshold) {
    return sbend_armor;
  } else {
    const float invcurve_armor = (1.0f / (armor/60.0f + 1.0f)) - 0.285f; // this < 0
    return invcurve_armor + sbend_armor;
  }
}



}
}