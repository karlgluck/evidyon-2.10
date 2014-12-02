#ifndef __EVIDYON_AVATAR_H__
#define __EVIDYON_AVATAR_H__

#include "evidyon_actor.h"

namespace Evidyon {

static const int MAX_AVATAR_NAME_LENGTH = 32;
static const int HIGHEST_DEFINED_LEVEL = 25;
static const int MAX_SPOKEN_TEXT_CHARACTERS = 48;

// based on the availability mask size
static const int MAX_SPELL_TYPES = (32*4); // TODO: confirm this during compilation!

/**
 * Used to define which effects are available to clients
 */
struct SpellAvailabilityMask
{
  unsigned long mask[4];
  inline void zero() { mask[0] = 0; mask[1] = 0; mask[2] = 0; mask[3] = 0; }
  inline void setBit(unsigned int bit)   { mask[bit>>5] |= 1<<(bit%32); }
  inline void clearBit(unsigned int bit)   { mask[bit>>5] &= ~(1<<(bit%32)); }
  inline bool isBitSet(unsigned int bit) const { return 1&(mask[bit>>5] >> (bit%32)); }
  inline void unionWith(const SpellAvailabilityMask* other) { mask[0] |= other->mask[0]; mask[1] |= other->mask[1]; mask[2] |= other->mask[2]; mask[3] |= other->mask[3]; }
  inline bool equals(const SpellAvailabilityMask* other) { return mask[0] == other->mask[0] && mask[1] == other->mask[1] && mask[2] == other->mask[2] && mask[3] == other->mask[3]; }
};

static const int AVATAR_RACE_HAIR_STYLES_PER_GENDER = 3;

/**
 * Defines player character genders
 */
enum AvatarGender {
  AVATARGENDER_MALE,
  AVATARGENDER_FEMALE,
  AVATARGENDER_COUNT,
  AVATARGENDER_INVALID,
};

enum AvatarRace {
  AVATARRACE_HUMAN,
  AVATARRACE_COUNT,
  AVATARRACE_INVALID,
};

enum AvatarClass {
  AVATARCLASS_KNIGHT,
  AVATARCLASS_SORCERER,
  AVATARCLASS_COUNT,
  AVATARCLASS_INVALID,
};

const char* AvatarGenderString(AvatarGender avatar_gender);
const char* AvatarRaceString(AvatarRace avatar_race);
const char* AvatarClassString(AvatarClass avatar_class);

}



#endif