//------------------------------------------------------------------------------------------------
// File:    playercharacterclass.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "wavsound.h"

#include "avatarmodifier.h"
#include "playercharacterclasslevelspells.h"
#include "playercharacterclass.h"







//------------------------------------------------------------------------------------------------
// Name: PlayerCharacterClass
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
PlayerCharacterClass::PlayerCharacterClass() {
  member("Description", &myDescription);
  member("HP Modifier", &myHPModifier);
  member("MP Modifier", &myMPModifier);
  member("Spells", &spells_);
  member("Ability Delay Factor", &myAbilityDelayFactor);
  member("Ability Delay Offset", &myAbilityDelayOffset);
}



//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets the string representation of this resource
//------------------------------------------------------------------------------------------------
std::string PlayerCharacterClass::toString()
{
  return getName();
}




//------------------------------------------------------------------------------------------------
// Name: fillServerDescription
// Desc: Fills out this structure's description for the server
//------------------------------------------------------------------------------------------------
void PlayerCharacterClass::fillServerDescription(AvatarClassDescription* description) const
{
  description->hpModifier = myHPModifier.getValue();
  description->mpModifier = myMPModifier.getValue();
  spells_.fillClassSpellAvailabilityMask(description->spellAvailabilityMask);
  description->abilityDelayFactor = myAbilityDelayFactor.getValue();
  description->abilityDelayOffset = myAbilityDelayOffset.getValue();
}


const std::string& PlayerCharacterClass::getDescription() const {
  return myDescription.getValue();
}


//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: The name of this resource
//------------------------------------------------------------------------------------------------
std::string PlayerCharacterClass::staticTypeName()
{
  return "Player Character Class";
}


