#ifndef __ITEMAVATARMODIFIER_H__
#define __ITEMAVATARMODIFIER_H__

#include <dc/integer>
#include <dc/float>
#include <dc/list>
#include "spell.h"
#include "itemavatarmodifierfield.h"
#include "itemavataractortypemodifier.h"


class ItemAvatarModifier : public dc::dcResource<ItemAvatarModifier> {
public:
  ItemAvatarModifier();

  // Builds text for what this item does based on its avatar modifier fields.
  // The first text is appended to the item's description.  The second text
  // is appended client-side when the corresponding bit is set in the item's
  // special properties.
  // This can fail if more than 8 fields are set to "prop" fields.
  bool compileForClient(bool is_weapon,
                        std::string* description_text,
                        std::string prop_field_descriptions[8]) const;

  // Builds the server's description of this item's modifier
  bool compileForServer(bool is_weapon,
                        Evidyon::CompiledItemAvatarModifier* modifier) const;


public:
  static std::string staticTypeName();


private:
  ItemAvatarModifierField<dc::dcInteger> min_attack_damage_;
  ItemAvatarModifierField<dc::dcInteger> max_attack_damage_;
  ItemAvatarModifierField<dc::dcInteger> physical_defense_;
  ItemAvatarModifierField<dc::dcInteger> magical_defense_;
  ItemAvatarModifierField<dc::dcInteger> hp_regen_rate_;
  ItemAvatarModifierField<dc::dcInteger> mp_regen_rate_;
  ItemAvatarModifierField<dc::dcFloat> hp_multiplier_;
  ItemAvatarModifierField<dc::dcFloat> mp_multiplier_;
  ItemAvatarModifierField<dc::dcInteger> hp_added_;
  ItemAvatarModifierField<dc::dcInteger> mp_added_;
  ItemAvatarModifierField<dc::dcInteger> level_added_;
  ItemAvatarModifierField<dc::dcFloat> movement_speed_multiplier_;
  ItemAvatarModifierField<dc::dcList<Spell>::Reference> spell_to_make_available_[2];
  ItemAvatarModifierField<dc::dcFloat> attack_speed_multiplier_;
  ItemAvatarModifierField<ItemAvatarActorTypeModifier> actor_type_;
  ItemAvatarModifierField<dc::dcFloat> steal_hp_percent_;
  ItemAvatarModifierField<dc::dcFloat> steal_mp_percent_;
};


#endif