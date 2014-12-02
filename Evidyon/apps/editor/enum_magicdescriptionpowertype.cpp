#include "detailedmagic.h"



template<> const char* dc::dcEnum<Evidyon::MagicDescription::PowerType>::getString(
    Evidyon::MagicDescription::PowerType value) {
  using namespace Evidyon;
  switch (value) {
    case MagicDescription::NO_BONUSES: return "No Bonuses";
    default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::MagicDescription::PowerType>::staticTypeName()
{
    return "Enum-MagicDescriptionPowerType";
}
