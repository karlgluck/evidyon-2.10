#ifndef __ITEMAVATARACTORTYPEMODIFIER_H__
#define __ITEMAVATARACTORTYPEMODIFIER_H__

#include <dc/list>
#include "actortype.h"
#include "../shared/evidyon_avatar.h"

// This class is mainly used so that armor can change the appearance of a character
class ItemAvatarActorTypeModifier : public dc::dcResource<ItemAvatarActorTypeModifier> {
public:
  ItemAvatarActorTypeModifier();
  void compileForServer(int actor_indices[Evidyon::AVATARGENDER_COUNT]
                                         [Evidyon::AVATARRACE_COUNT]
                                         [Evidyon::AVATARCLASS_COUNT]) const;

public:
  static std::string staticTypeName();

private:
  dc::dcList<ActorType>::Reference actor_types_[Evidyon::AVATARGENDER_COUNT]
                                               [Evidyon::AVATARRACE_COUNT]
                                               [Evidyon::AVATARCLASS_COUNT];
};


#endif