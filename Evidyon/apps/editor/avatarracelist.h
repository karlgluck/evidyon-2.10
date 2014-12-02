#ifndef __AVATARRACELIST_H__
#define __AVATARRACELIST_H__


#include "../shared/evidyon_avatar.h"
#include "playercharacterrace.h"


class AvatarRaceList : public dc::dcResource<AvatarRaceList> {
public:
  AvatarRaceList();
  bool compileForServer(dc::dcStreamOut* stream) const;
  bool compileForClient(dc::dcStreamOut* stream) const;

public:
  static std::string staticTypeName();

private:
  PlayerCharacterRace races_[Evidyon::AVATARRACE_COUNT];
};


#endif