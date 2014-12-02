#include "detailedmagic.h"



template<> const char* dc::dcEnum<Evidyon::MagicDescriptionStageEffect::Type>::getString(
    Evidyon::MagicDescriptionStageEffect::Type value) {
  using namespace Evidyon;
  switch (value) {
    case MagicDescriptionStageEffect::INVALID:  return "Invalid";
    case MagicDescriptionStageEffect::WEAPON:   return "Weapon";
    case MagicDescriptionStageEffect::HP:       return "HP";
    case MagicDescriptionStageEffect::HP_NOARMOR: return "HP (No Armor)";
    case MagicDescriptionStageEffect::MP:       return "MP";
    case MagicDescriptionStageEffect::TELEPORT: return "Teleport";
    case MagicDescriptionStageEffect::TELEPORT_SWAP:  return "Teleport - Swap";
    case MagicDescriptionStageEffect::DISENCHANT:     return "Disenchant";
    case MagicDescriptionStageEffect::ENCHANT_HP:     return "Enchant HP";
    case MagicDescriptionStageEffect::ENCHANT_MP:     return "Enchant MP";
    case MagicDescriptionStageEffect::ENCHANT_MUTATE: return "Mutate to Actor Type";
    case MagicDescriptionStageEffect::ENCHANT_MUTATE_GASFORM:   return "Mutate to Gasform";
    case MagicDescriptionStageEffect::ENCHANT_MUTATE_COPY:      return "Mutate to Copy of Target";
    case MagicDescriptionStageEffect::ENCHANT_ATTACK_RUN:       return "Attack Run";
    case MagicDescriptionStageEffect::ENCHANT_MAXHP:            return "Enchant Max HP";
    case MagicDescriptionStageEffect::ENCHANT_MAXMP:            return "Enchant Max MP";
    case MagicDescriptionStageEffect::ENCHANT_PHYSICAL_DAMAGE:  return "Enchant Physical Damage";
    case MagicDescriptionStageEffect::ENCHANT_PHYSICAL_DEFENSE: return "Enchant Physical Defense";
    case MagicDescriptionStageEffect::ENCHANT_MAGICAL_DEFENSE:  return "Enchant Magical Defense";
    case MagicDescriptionStageEffect::ENCHANT_MOVEMENT_SPEED:   return "Enchant Movement Speed";
    case MagicDescriptionStageEffect::ENCHANT_ATTACK_SPEED:     return "Enchant Attack Speed";
    case MagicDescriptionStageEffect::GEOSID_RECALL:            return "Geosid: Recall";
    case MagicDescriptionStageEffect::GEOSID_RESTOREHPMP:       return "Geosid: Restore HP/MP";
    default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::MagicDescriptionStageEffect::Type>::staticTypeName()
{
    return "Enum-MagicDescriptionStageEffectType";
}
