#ifndef __EVIDYON_GAMEPLAY_DESIGN_H__
#define __EVIDYON_GAMEPLAY_DESIGN_H__


// contains static game rule definitions
namespace Evidyon {
namespace GameplayDesign {

int avatarStatBonus(int stat_value);
double avatarLevelPercent(int level);
float avatarMovementSpeed(int agility);
float avatarAttackDelay(int agility);
float avatarCastingDelayPercent(int agility, int intelligence);
float avatarOffensiveMagicBonus(int level, int intelligence, int wisdom);
float avatarDefensiveMagicBonus(int level, int wisdom);
int avatarHPRegenRate(int hp, int constitution);
int avatarMPRegenRate(int mp, int intelligence);
int avatarMeleeDamageBase(int strength, int level);
int avatarMeleeDamageAdditional(int strength, int level);

// HP/MP before multiplying by class-specific HP/MP modifiers
int avatarCoreHP(int level, int strength, int constitution);
int avatarCoreMP(int level, int intelligence, int wisdom);


// Returns the modification of damage based on armor.  This is
// the same for both magic and physical armor values.
float armorDamageFactor(int armor);

}
}



#endif