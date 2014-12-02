#include "avatarclasslist.h"
#include "../shared/server-editor/server_gamefile.h"
#include "../shared/client-editor/client_gamefile.h"

AvatarClassList::AvatarClassList() {
  for (int i = 0; i < (int)Evidyon::AVATARCLASS_COUNT; ++i) {
    member(Evidyon::AvatarClassString((Evidyon::AvatarClass)i),
          &classes_[i]);
  }
}

bool AvatarClassList::compileForServer(dc::dcStreamOut* stream) const {
  { // todo: not needed because classes are specified
    size_t classes = Evidyon::AVATARCLASS_COUNT;
    stream->write(&classes, sizeof(classes));
  }
  {
    for (int i = 0; i < (int)Evidyon::AVATARCLASS_COUNT; ++i) {
      Evidyon::AvatarClassDescription description;
      classes_[i].fillServerDescription(&description);
      CONFIRM(stream->write(&description,
                            sizeof(description))) else return false;

    }
  }

  return true;
}



bool AvatarClassList::compileForClient(dc::dcStreamOut* stream) const {
  { // todo: not needed because classes are specified
    size_t classes = Evidyon::AVATARCLASS_COUNT;
    stream->write(&classes, sizeof(classes));
  }
  {
    for (int i = 0; i < (int)Evidyon::AVATARCLASS_COUNT; ++i) {
    char name[CLIENTGAMEFILE_CLASS_STRING_LENGTH];
    strcpy_s(name,
             CLIENTGAMEFILE_CLASS_STRING_LENGTH,
             classes_[i].getName().c_str());
      CONFIRM(stream->write(name,
                            CLIENTGAMEFILE_CLASS_STRING_LENGTH * sizeof(char))) else return false;
      char desc[CLIENTGAMEFILE_CLASS_DESCRIPTION_STRING_LENGTH];
      strcpy_s(desc,
               CLIENTGAMEFILE_CLASS_DESCRIPTION_STRING_LENGTH,
               classes_[i].getDescription().c_str());
      CONFIRM(stream->write(desc,
                            CLIENTGAMEFILE_CLASS_DESCRIPTION_STRING_LENGTH * sizeof(char))) else return false;
    }
  }

  return true;
}

std::string AvatarClassList::staticTypeName() {
  return "AvatarClassList";
}