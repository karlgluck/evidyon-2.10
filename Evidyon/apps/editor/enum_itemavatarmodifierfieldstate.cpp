#include "itemavatarmodifierfield.h"




template<> const char* dc::dcEnum<ItemAvatarModifierFieldState>::getString(
    ItemAvatarModifierFieldState value) {
  switch (value) {
    case FIELDSTATE_DISABLED:           return "Disabled";
    case FIELDSTATE_INNATE_ATTRIBUTE:   return "Innate Attribute";
    case FIELDSTATE_PROPPED_ATTRIBUTE:  return "Propped Attribute";
    default: return 0;
  }
}

std::string dc::dcEnum<ItemAvatarModifierFieldState>::staticTypeName()
{
    return "Enum-ItemAvatarModifierFieldState";
}
