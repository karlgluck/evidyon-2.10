#include "spell.h"
#include "../shared/server-editor/serverspelldescription.h"
#include "../shared/client-editor/clientspelldescription.h"

namespace Evidyon {


Spell::Spell() {
  member("Description", &description_);
  member("MP Cost", &mp_cost_);
  member("Magic", &magic_);
  member("Casting Needs Target", &casting_needs_target_);
}


void Spell::compileForClient(ClientSpellDescription* description) const {
  sprintf_s(description->name, ClientSpellDescription::MAX_NAME_LENGTH,
            "%s", getName().c_str());
  sprintf_s(description->description, ClientSpellDescription::MAX_DESCRIPTION_LENGTH,
            "%s", description_.getValue().c_str());
  description->mp_cost = mp_cost_.getValue();
  description->needs_target = casting_needs_target_.getValue();
}


void Spell::compileForServer(ServerSpellDescription* description) const {
  description->mp_cost = mp_cost_.getValue();
  description->magic_index = magic_.getReferencedResourceIndex();
}


bool Spell::castingNeedsTarget() const {
  return casting_needs_target_.getValue();
}


std::string Spell::staticTypeName() {
  return "Spell";
}



}