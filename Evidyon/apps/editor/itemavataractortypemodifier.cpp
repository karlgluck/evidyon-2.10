#include "itemavataractortypemodifier.h"

// needed so we can get default actor types for invalid references
// NOTE: this is TODO because i haven't implemented changing actor
// based on class type yet--only race, and i don't want to implement
// something that requires a lot of code to be written that i'm just
// going to have to rewrite when that change is made.
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"


ItemAvatarActorTypeModifier::ItemAvatarActorTypeModifier() {
  using namespace Evidyon;
  for (int gender_index = 0; gender_index < AVATARGENDER_COUNT; ++gender_index) {
    for (int race_index = 0; race_index < AVATARRACE_COUNT; ++race_index) {
      for (int class_index = 0; class_index < AVATARCLASS_COUNT; ++class_index) {
        std::string gender = AvatarGenderString((AvatarGender)gender_index);
        std::string race = AvatarRaceString((AvatarRace)race_index);
        std::string class_name = AvatarClassString((AvatarClass)class_index);
        member(gender + " " + race + " " + class_name,
               &actor_types_[gender_index][race_index][class_index]);
      }
    }
  }
}


void ItemAvatarActorTypeModifier::compileForServer(int actor_indices[Evidyon::AVATARGENDER_COUNT]
                                                                    [Evidyon::AVATARRACE_COUNT]
                                                                    [Evidyon::AVATARCLASS_COUNT]) const {
  using namespace Evidyon;
  ProjectVolucris* pv = globalEditor()->getRootResource();
  for (int gender_index = 0; gender_index < AVATARGENDER_COUNT; ++gender_index) {
    for (int race_index = 0; race_index < AVATARRACE_COUNT; ++race_index) {
      for (int class_index = 0; class_index < AVATARCLASS_COUNT; ++class_index) {
        const dc::dcList<ActorType>::Element* actor_element =
            actor_types_[gender_index][race_index][class_index].getReferencedResource();

        if (actor_element == NULL) {
          // TODO!
          actor_indices[gender_index][race_index][class_index] = 0;
        } else {
          actor_indices[gender_index][race_index][class_index] =
              actor_element->getIndex();
        }
      }
    }
  }
}




std::string ItemAvatarActorTypeModifier::staticTypeName() {
  return "ItemAvatarActorTypeModifier";
}