#include "normalmeleemonsterlifeformaiactortype.h"
#include "../shared/server-editor/lifeformaidescription.h"
#include "../shared/actordescriptiontemplate.h"



NormalMeleeMonsterLifeformAIActorType::NormalMeleeMonsterLifeformAIActorType() {
  template_index_ = -1;

  member("Actor Type", &actor_type_);
  member("Enchantments", &enchantments_);
}


bool NormalMeleeMonsterLifeformAIActorType::compileForServer(Evidyon::AIMonsterActorType* description) {
  CONFIRM(template_index_ >= 0) else return false; // ensure that the client-side was compiled first
  description->template_index = template_index_;

  const dc::dcList<ActorType>::Element* actor_type = actor_type_.getReferencedResource();
  CONFIRM(actor_type) else return false;
  description->size = actor_type->getRadialSizeValue();

  return true;
}


bool NormalMeleeMonsterLifeformAIActorType::compileActorTemplate(int template_index,
                                                                 Evidyon::ClientActorDescriptionTemplate* client_description,
                                                                 Evidyon::ServerActorDescriptionTemplate* server_description) {
  template_index_ = template_index;

  const dc::dcList<ActorType>::Element* actor_type = actor_type_.getReferencedResource();
  CONFIRM(actor_type) else return false;
  client_description->actor_type = actor_type->getIndex();
  server_description->actor_type = actor_type->getIndex();
  server_description->radial_size = actor_type->getRadialSizeValue();

  client_description->template_type = Evidyon::ACTORTEMPLATETYPE_MONSTER;

  client_description->enchantments = 0;
  const dc::dcGenericResource::Array& enchantments = enchantments_.getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = enchantments.begin();
       i != enchantments.end(); ++i) {
    dc::dcReference<Enchantment>* enchantment_reference = 
      (dcList<dcReference<Enchantment>>::Element*)(*i);
    Enchantment* enchantment = enchantment_reference->getReferencedResource();

    client_description->enchantments &= enchantment->getInvClassMask();
    client_description->enchantments |= enchantment->getTypeBits();
  }

  for (int i = 0; i < Evidyon::ACTORATTACHMENTPOINT_COUNT; ++i) {
    client_description->equipped_item_scenery[i] = 0xFFFFFFFF;
  }

  return true;
}


std::string NormalMeleeMonsterLifeformAIActorType::staticTypeName() {
  return "NormalMeleeMonsterLifeformAIActorType";
}
