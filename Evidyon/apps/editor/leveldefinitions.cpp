//------------------------------------------------------------------------------------------------
// File:    levelupexperience.cpp
//
// Desc:    Defines the amount of experience necessary to achieve a certain level
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "leveldefinitions.h"



LevelAttributes::LevelAttributes() {
  member("Exp. to Next Level", &experience_to_next_level_);
  member("Average HP", &average_hp_);
  member("Average MP", &average_mp_);
  member("Average Melee DPS", &melee_damage_per_second_);
  member("Average Magic DPS", &magic_damage_per_second_);
  member("Hours To Level", &hours_to_gain_level_);
}

int LevelAttributes::getExperienceToNextLevel() const {
  return experience_to_next_level_.getValue();
}

float LevelAttributes::getMagicDPS() const {
  return magic_damage_per_second_.getValue();
}

float LevelAttributes::getMeleeDPS() const {
  return melee_damage_per_second_.getValue();
}

int LevelAttributes::getAverageHP() const {
  return average_hp_.getValue();
}

int LevelAttributes::getAverageMP() const {
  return average_mp_.getValue();
}

void LevelAttributes::setAverageHP(int value) {
  average_hp_.setValue(value);
}

void LevelAttributes::setAverageMP(int value) {
  average_mp_.setValue(value);
}

float LevelAttributes::getHoursToGainLevel() const {
  return hours_to_gain_level_.getValue();
}

float LevelAttributes::getSecondsToGainLevel() const {
  return hours_to_gain_level_.getValue() * 60.0f * 60.0f;
}

float LevelAttributes::getExpectedExpPerSecond() const {
  return getExperienceToNextLevel() / getSecondsToGainLevel();
}

std::string LevelAttributes::staticTypeName() {
  return "LevelAttributes";
}






//------------------------------------------------------------------------------------------------
// Name: LevelDefinitions
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
LevelDefinitions::LevelDefinitions()
{
    for (int i = 0; i < HIGHEST_DEFINED_LEVEL; ++i)
    {
        char name[8];
        sprintf_s(name, sizeof(name)/sizeof(char), "%i", i+1);
        member(name, &level_attributes_[i]);
    }
}



//------------------------------------------------------------------------------------------------
// Name: compileForServer
// Desc: Compiles this structure into the output stream for the server
//------------------------------------------------------------------------------------------------
bool LevelDefinitions::compileForServer(dcStreamOut* stream) const
{
    if (APP_ERROR(!stream)("Invalid stream")) return false;

    for (int i = 0; i < HIGHEST_DEFINED_LEVEL; ++i)
    {
        int experience = level_attributes_[i].getExperienceToNextLevel();
        if (!stream->write(&experience, sizeof(experience))) return false;
    }

    // Success
    return true;
}


const LevelAttributes* LevelDefinitions::getLevelAttributes(int level) const {
  if (level <= 0) return &level_attributes_[0];
  if (level >= HIGHEST_DEFINED_LEVEL) return &level_attributes_[HIGHEST_DEFINED_LEVEL - 1];
  return &level_attributes_[level - 1];
}

LevelAttributes* LevelDefinitions::getLevelAttributes(int level) {
  if (level <= 0) return &level_attributes_[0];
  if (level >= HIGHEST_DEFINED_LEVEL) return &level_attributes_[HIGHEST_DEFINED_LEVEL - 1];
  return &level_attributes_[level - 1];
}


//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string LevelDefinitions::staticTypeName()
{
    return "LevelDefinitions";
}

