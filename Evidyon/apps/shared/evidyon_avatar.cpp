#include "evidyon_avatar.h"

namespace Evidyon {

// client item description: display as % of max, or qty.

//class : Knight
// bash in heads
//class : Sorcerer
// illusions, summoning spells, magey stuff
//class : Jackal
// has "grab", backstab skills, fast fast fast!, tracking skills
// the theif class
//class : Druid
// healing, buffing, shapeshifting, naturalist stuff, convert enemies, etc.
//class : Artemist (artisan, alchemist)
// mining, metalworking, item creation, etc.
//  genesis - turn genite into an item
//  synthesis - combine items using genite to create a new item
//  extract - draw minerals from the earth (create item of mineral type below you)
//  

const char* AvatarGenderString(AvatarGender avatar_gender) {
  switch (avatar_gender) {
    default: return "invalid";
    case AVATARGENDER_MALE: return "Male";
    case AVATARGENDER_FEMALE: return "Female";
  }
}

const char* AvatarRaceString(AvatarRace avatar_race) {
  switch (avatar_race) {
    default: return "invalid";
    case AVATARRACE_HUMAN: return "Human";
  }
}

const char* AvatarClassString(AvatarClass avatar_class) {
  switch (avatar_class) {
    default: return "invalid";
    case AVATARCLASS_KNIGHT: return "Knight";
    case AVATARCLASS_SORCERER: return "Sorcerer";
  }
}




}