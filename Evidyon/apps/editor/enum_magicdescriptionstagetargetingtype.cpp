#include "detailedmagic.h"


template<> const char* dc::dcEnum<Evidyon::MagicDescriptionStageTargeting::Type>::getString(
  Evidyon::MagicDescriptionStageTargeting::Type value) {
  using namespace Evidyon;
  switch (value) {
    case MagicDescriptionStageTargeting::SELF: return "Self";
    case MagicDescriptionStageTargeting::PROJECTILE: return "Projectile";
    case MagicDescriptionStageTargeting::ANY_TARGET_ACTOR: return "Any Target Actor";
    case MagicDescriptionStageTargeting::NONCASTER_TARGET_ACTOR: return "Non-Caster Target Actor";
    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_CASTER: return "AOE - Caster-Bound";
    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_CASTER_OFFENSIVE: return "Offensive AOE - Caster-Bound";
    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_OTHER_OFFENSIVE:  return "Offensive AOE - Actor-Bound";
    case MagicDescriptionStageTargeting::AOE_LOCATIONBOUND_OFFENSIVE: return "Offensive AOE - Location-Bound";
    case MagicDescriptionStageTargeting::PERIODIC_AOE_LOCATIONBOUND_OFFENSIVE: return "Periodic Offensive AOE - Location-Bound";
    default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::MagicDescriptionStageTargeting::Type>::staticTypeName()
{
    return "Enum-MagicDescriptionStageTargetingType";
}

