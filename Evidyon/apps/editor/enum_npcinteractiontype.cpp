#include "../shared/server-editor/lifeformaidescription.h"
#include <dc/enum>



template<>
const char* dc::dcEnum<Evidyon::NPCInteractionType>::getString(Evidyon::NPCInteractionType value) {
  using namespace Evidyon;
  switch (value) {
    case NPCINTERACTION_STORAGE:    return "Storage";
    case NPCINTERACTION_BAZAAR:     return "Bazaar";
    case NPCINTERACTION_GEOSID:     return "Geosid";
    case NPCINTERACTION_QUEST:      return "Quest";
    case NPCINTERACTION_MERCHANT:   return "Merchant";
    default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::NPCInteractionType>::staticTypeName()
{
    return "Enum-NPCInteractionType";
}
