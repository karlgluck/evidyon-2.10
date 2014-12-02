//------------------------------------------------------------------------------------------------
// File:    playercharacterrace.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"
#include "../shared/actordescriptiontemplate.h"
#include "actordescriptiontemplatecompiler.h"





//------------------------------------------------------------------------------------------------
// Name: PlayerCharacterRace
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
PlayerCharacterRace::PlayerCharacterRace() {
  member("Description", &myDescription);
  member("Level 1 Max Strength", &myMaxStrength);
  member("Level 1 Max Agility", &myMaxAgility);
  member("Level 1 Max Constitution", &myMaxConstitution);
  member("Level 1 Max Intelligence", &myMaxIntelligence);
  member("Level 1 Max Wisdom", &myMaxWisdom);

  member("Male Actor Type", &myActorTypes[Evidyon::AVATARGENDER_MALE]);
  member("Female Actor Type", &myActorTypes[Evidyon::AVATARGENDER_FEMALE]);

  for (int i = 0; i < AVATAR_RACE_HAIR_STYLES_PER_GENDER; ++i) {
    char number[16];
    sprintf_s(number, 16, "%i", i+1);
    member(std::string("Male Hair Style #") + number, &hair_styles_[Evidyon::AVATARGENDER_MALE][i]);
  }
  for (int i = 0; i < AVATAR_RACE_HAIR_STYLES_PER_GENDER; ++i) {
    char number[16];
    sprintf_s(number, 16, "%i", i+1);
    member(std::string("Female Hair Style #") + number, &hair_styles_[Evidyon::AVATARGENDER_FEMALE][i]);
  }
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets the string representation of this resource
//------------------------------------------------------------------------------------------------
std::string PlayerCharacterRace::toString()
{
    return getName();
}


//------------------------------------------------------------------------------------------------
bool PlayerCharacterRace::compileActorTemplates(ActorDescriptionTemplateCompiler* compiler) {
  for (int i = 0; i < (int)(Evidyon::AVATARGENDER_COUNT); ++i) {
    actor_type_templates_[i] = compiler->getNextTemplateIndex();

    dc::dcList<ActorType>::Element* actor_type = myActorTypes[i].getReferencedResource();
    CONFIRM(actor_type) else return false;

    Evidyon::ClientActorDescriptionTemplate client_description;
    client_description.actor_type = actor_type->getIndex();
    client_description.name[0] = '\0';
    client_description.enchantments = 0;
    client_description.description_can_change = true;
    client_description.template_type = Evidyon::ACTORTEMPLATETYPE_PLAYER;

    client_description.equipped_item_scenery[ACTORATTACHMENTPOINT_HELMET] = hair_styles_[i][0].getReferencedResourceIndex();

    // just set all to the normalized default
    for (int j = 0; j < (int)ACTORACTION_VARIABLE_SPEED_COUNT; ++j)
      client_description.action_speeds[j] = 1.0f;

    // reset attachment points
    for (int i = 0; i < Evidyon::ACTORATTACHMENTPOINT_COUNT; ++i) {
      client_description.equipped_item_scenery[i] = 0xFFFFFFFF;
    }

    Evidyon::ServerActorDescriptionTemplate server_description;
    server_description.actor_type = actor_type->getIndex();
    server_description.radial_size = actor_type->getRadialSizeValue();

    compiler->addActorDescriptionTemplate(&client_description,
                                          &server_description);
  }

  return true;
}


const std::string& PlayerCharacterRace::getDescription() const {
  return myDescription.getValue();
}





//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: The name of this resource
//------------------------------------------------------------------------------------------------
std::string PlayerCharacterRace::staticTypeName()
{
    return "Player Character Race";
}