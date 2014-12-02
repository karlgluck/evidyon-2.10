#include "../shared/evidyon_actor.h"
#include <dc/enum>


template<> const char* dc::dcEnum<Evidyon::ActorAttack>::getString(
    Evidyon::ActorAttack value) {
  using namespace Evidyon;
  switch (value) {
    case Evidyon::ACTORATTACK_1: return "Attack 1";
    case Evidyon::ACTORATTACK_2: return "Attack 2";
    case Evidyon::ACTORATTACK_3: return "Attack 3";
    case Evidyon::ACTORATTACK_4: return "Attack 4";
    default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::ActorAttack>::staticTypeName()
{
    return "Enum-ActorAttack";
}
