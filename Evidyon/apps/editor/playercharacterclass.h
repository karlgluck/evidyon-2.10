//------------------------------------------------------------------------------------------------
// File:    playercharacterclass.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __PLAYERCHARACTERCLASS_H__
#define __PLAYERCHARACTERCLASS_H__


#include <dc/float>
#include "playercharacterclasslevelspells.h"
#include "../shared/evidyon_avatar.h"

namespace Evidyon {
struct AvatarClassDescription;
}

/**
 * Specifies a character class (fighter, paladin, wizard, etc...) and its related attributes
 */
class PlayerCharacterClass : public dc::dcResource<PlayerCharacterClass>
{
    public:

        /**
         * Initializes this class
         */
        PlayerCharacterClass();

        /**
         * Gets the string representation of this resource
         */
        std::string toString();

        float getHPModifierValue() const { return myHPModifier.getValue(); }
        float getMPModifierValue() const { return myMPModifier.getValue(); }

        /**
         * Fills out this structure's description for the server
         */
        void fillServerDescription(Evidyon::AvatarClassDescription* description) const;

        const std::string& getDescription() const;

    public:

        /**
         * The name of this resource
         */
        static std::string staticTypeName();


    protected:

        // Description of the race
        dc::dcString myDescription;

        /// The HP modifier that determines how quickly this class gains HP
        dc::dcFloat myHPModifier;

        /// The MP modifier that determines how quickly this class gains MP
        dc::dcFloat myMPModifier;

        /// The effects that players of this class are able to use at a particular level
        PlayerCharacterClassLevelSpells spells_;

        /// Magic delays
        dc::dcFloat myAbilityDelayFactor, myAbilityDelayOffset;
};


#endif