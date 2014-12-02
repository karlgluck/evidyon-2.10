#ifndef __AVATARCLASSLIST_H__
#define __AVATARCLASSLIST_H__


#include "../shared/evidyon_avatar.h"
#include "playercharacterclass.h"


class AvatarClassList : public dc::dcResource<AvatarClassList> {
public:
  AvatarClassList();
  bool compileForServer(dc::dcStreamOut* stream) const;
  bool compileForClient(dc::dcStreamOut* stream) const;

public:
  static std::string staticTypeName();

private:
  PlayerCharacterClass classes_[Evidyon::AVATARCLASS_COUNT];
};


#endif