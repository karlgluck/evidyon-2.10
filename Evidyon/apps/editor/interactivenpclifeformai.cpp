#include "interactivenpclifeformai.h"
#include "../shared/actordescriptiontemplate.h"
#include "actordescriptiontemplatecompiler.h"

// need access to the global editor
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"


dc::dcClass<InteractiveNPCLifeformAI>::Implements<LifeformAI> InteractiveNPCLifeformAI::implementsLifeformAI;



InteractiveNPCLifeformAI::InteractiveNPCLifeformAI() {
  member("Name", &name_);
  member("Actor Type", &actor_type_);
  member("Interaction Type", &interaction_type_);
  member("Wander?", &wander_);

  for (int i = 0; i < Evidyon::ACTORATTACHMENTPOINT_COUNT; ++i) {
    const char* point = dc::dcEnum<Evidyon::ActorAttachmentPoint>::getString((Evidyon::ActorAttachmentPoint)i);
    std::string name = "";
    name.append(point);
    member(name, &equipped_items_[i]);
  }

  interaction_type_.setValue(Evidyon::NPCINTERACTION_STORAGE);
}

const std::string& InteractiveNPCLifeformAI::getLifeformName() const {
  return name_.getValue();
}


bool InteractiveNPCLifeformAI::compileForServer(Evidyon::LifeformAIDescription* description) const {
  description->type = Evidyon::LifeformAIDescription::INTERACTIVE_NPC;
  description->interactive_npc.interaction = interaction_type_.getValue();
  description->interactive_npc.actor_template_index = compiled_actor_template_index_;

  return true;
}


bool InteractiveNPCLifeformAI::compileActorTemplates(ActorDescriptionTemplateCompiler* compiler) {
  compiled_actor_template_index_ = compiler->getNextTemplateIndex();
  Evidyon::ClientActorDescriptionTemplate client_description;
  Evidyon::ServerActorDescriptionTemplate server_description;

  for (int i = 0; i < Evidyon::ACTORACTION_VARIABLE_SPEED_COUNT; ++i) {
    client_description.action_speeds[i] = 1.0f;
  }

  client_description.enchantments = 0;
  client_description.description_can_change = false;
  client_description.actor_type = actor_type_.getReferencedResourceIndex();
  strncpy_s(client_description.name, MAX_ACTOR_NAME_LENGTH, name_.getValue().c_str(), MAX_ACTOR_NAME_LENGTH);
  switch (interaction_type_.getValue()) {
    case Evidyon::NPCINTERACTION_STORAGE:  client_description.template_type = ACTORTEMPLATETYPE_NPC_STORAGE; break;
    case Evidyon::NPCINTERACTION_BAZAAR:   client_description.template_type = ACTORTEMPLATETYPE_NPC_BAZAAR; break;
    case Evidyon::NPCINTERACTION_GEOSID:   client_description.template_type = ACTORTEMPLATETYPE_NPC_GEOSID; break;
    case Evidyon::NPCINTERACTION_QUEST:    client_description.template_type = ACTORTEMPLATETYPE_NPC_QUESTGIVER; break;
    case Evidyon::NPCINTERACTION_MERCHANT: client_description.template_type = ACTORTEMPLATETYPE_NPC_MERCHANT; break;
  }

  for (int i = 0; i < Evidyon::ACTORATTACHMENTPOINT_COUNT; ++i) {
    dcList<Item>::Element* item_element = equipped_items_[i].getReferencedResource();
    size_t scenery_index = 0xFFFFFFFF;
    if (item_element) {
      scenery_index = item_element->getSceneryReference()->getReferencedResourceIndex();
    }
    client_description.equipped_item_scenery[i] = scenery_index;
  }

  server_description.actor_type = client_description.actor_type;

  compiler->addActorDescriptionTemplate(&client_description, &server_description);

  return true;
}



std::string InteractiveNPCLifeformAI::staticTypeName() {
  return "InteractiveNPCLifeformAI";
}
