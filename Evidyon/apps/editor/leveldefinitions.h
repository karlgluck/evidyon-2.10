//------------------------------------------------------------------------------------------------
// File:    leveldefinitions.h
//
// Desc:    Defines many attributes of characters at given levels
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __LEVELDEFINITIONS_H__
#define __LEVELDEFINITIONS_H__

#include <dc/integer>
#include <dc/float>
#include "../shared/evidyon_avatar.h"


class LevelAttributes : public dc::dcResource<LevelAttributes> {
public:
  LevelAttributes();
  int getExperienceToNextLevel() const;
  float getMeleeDPS() const;
  float getMagicDPS() const;
  int getAverageHP() const;
  int getAverageMP() const;
  void setAverageHP(int value);
  void setAverageMP(int value);
  float getHoursToGainLevel() const;
  float getSecondsToGainLevel() const;
  float getExpectedExpPerSecond() const;

public:
  static std::string staticTypeName();

private:
  dc::dcInteger experience_to_next_level_;
  dc::dcInteger average_hp_;
  dc::dcInteger average_mp_;
  dc::dcFloat melee_damage_per_second_;
  dc::dcFloat magic_damage_per_second_;
  dc::dcFloat hours_to_gain_level_;
};


/**
 * Defines the amount of experience necessary to achieve a certain level
 */
class LevelDefinitions : public dc::dcResource<LevelDefinitions>
{
public:

  /**
   * Initializes this class
   */
  LevelDefinitions();

  /**
   * Compiles this structure into the output stream for the server
   */
  bool compileForServer(dc::dcStreamOut* stream) const;

  const LevelAttributes* getLevelAttributes(int level) const;
  LevelAttributes* getLevelAttributes(int level);

public:
  static std::string staticTypeName();

protected:
  LevelAttributes level_attributes_[Evidyon::HIGHEST_DEFINED_LEVEL];

};

#endif