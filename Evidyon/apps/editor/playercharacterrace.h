//------------------------------------------------------------------------------------------------
// File:    playercharacterrace.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __PLAYERCHARACTERRACE_H__
#define __PLAYERCHARACTERRACE_H__

namespace Evidyon {
struct ClientActorDescriptionTemplate;
}

class ActorDescriptionTemplateCompiler;

#include <dc/byte>
#include <dc/list>
#include "../shared/evidyon_avatar.h"
#include "actortype.h"
#include "scenery.h"


/**
 * Holds a character race (human, elf, gnome...) and its related attributes 
 */
class PlayerCharacterRace : public dc::dcResource<PlayerCharacterRace>
{
public:

  /**
   * Initializes this class
   */
  PlayerCharacterRace();

  /**
   * Gets the string representation of this resource
   */
  std::string toString();

  dc::dcByte::BasicType getMaxStrengthValue() const      { return myMaxStrength.getValue(); }
  dc::dcByte::BasicType getMaxAgilityValue() const       { return myMaxAgility.getValue(); }
  dc::dcByte::BasicType getMaxConstitutionValue() const  { return myMaxConstitution.getValue(); }
  dc::dcByte::BasicType getMaxIntelligenceValue() const  { return myMaxIntelligence.getValue(); }
  dc::dcByte::BasicType getMaxWisdomValue() const        { return myMaxWisdom.getValue(); }

  //const dc::dcList<ActorType>::Element* getActorType(Evidyon::AvatarGender gender) const { return myActorTypes[gender].getReferencedResource(); }
  int getActorTypeTemplate(Evidyon::AvatarGender gender) const { return actor_type_templates_[gender]; }

  // Writes both genders' actor types into the stream
  bool compileActorTemplates(ActorDescriptionTemplateCompiler* compiler);

  const std::string& getDescription() const;

  size_t getHairStyle(int gender, int style) const { return hair_styles_[gender][style].getReferencedResourceIndex(); }

public:

  /**
   * The name of this resource
   */
  static std::string staticTypeName();


protected:

  // Description of the race
  dc::dcString myDescription;

  /// The maximum values of the stats for level 1 players of this race
  dc::dcByte myMaxStrength;
  dc::dcByte myMaxAgility;
  dc::dcByte myMaxConstitution;
  dc::dcByte myMaxIntelligence;
  dc::dcByte myMaxWisdom;

  /// Which actor type to use for each gender of this race
  dc::dcList<ActorType>::Reference myActorTypes[Evidyon::AVATARGENDER_COUNT];

  // When the races are compiled for the client, this saves the template
  // index so that it can be copied into the server's description.
  int actor_type_templates_[Evidyon::AVATARGENDER_COUNT];

  // Hair styles for each gender
  dc::dcList<Scenery>::Reference hair_styles_[Evidyon::AVATARGENDER_COUNT][Evidyon::AVATAR_RACE_HAIR_STYLES_PER_GENDER];
};

#endif