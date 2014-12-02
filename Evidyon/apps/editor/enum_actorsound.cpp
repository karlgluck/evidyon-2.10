#include "../shared/evidyon_actor.h"
#include <dc/enum>



template<> const char* dc::dcEnum<Evidyon::ActorSound>::getString(Evidyon::ActorSound value) {
  using namespace Evidyon;
  switch (value) {
  case ACTORSOUND_ATTACK1:  return "Attack 1";
  case ACTORSOUND_ATTACK2:  return "Attack 2";
  case ACTORSOUND_DIE1:     return "Die 1";
  case ACTORSOUND_DIE2:     return "Die 2";
  case ACTORSOUND_GOTHIT1:  return "Got Hit 1";
  case ACTORSOUND_GOTHIT2:  return "Got Hit 2";
  default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::ActorSound>::staticTypeName()
{
    return "Enum-ActorSound";
}
