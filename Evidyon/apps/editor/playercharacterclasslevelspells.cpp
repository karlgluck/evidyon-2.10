//------------------------------------------------------------------------------------------------
// File:    playercharacterclasslevelspells.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "wavsound.h"

#include "avatarmodifier.h"
#include "playercharacterclasslevelspells.h"



//------------------------------------------------------------------------------------------------
// Name: fillClassSpellAvailabilityMask
// Desc: Fills out the mask with available effects
//------------------------------------------------------------------------------------------------
void PlayerCharacterClassLevelSpells::fillClassSpellAvailabilityMask(SpellAvailabilityMask mask[HIGHEST_DEFINED_LEVEL]) const
{
    // Reset the mask
    for (int i = 0; i < HIGHEST_DEFINED_LEVEL; ++i)
        mask[i].zero();

    // Fill in effects for their given level
    const dc::dcGenericResource::Array& members = getMembers();
    for (dcGenericResource::Array::const_iterator i = members.begin(); i != members.end(); ++i)
    {
        const Element* element = (Element*)(*i);
        int levelIndex = element->getLevelValue() - 1;
        levelIndex = max(levelIndex,0);
        if (APP_WARNING(levelIndex >= HIGHEST_DEFINED_LEVEL)("Invalid level for magic (%lu being capped at %lu)", levelIndex+1, HIGHEST_DEFINED_LEVEL ))
            levelIndex = HIGHEST_DEFINED_LEVEL - 1;
        const dc::dcList<Spell>::Element* spell = element->getSpell();
        if (APP_WARNING(!spell)("Invalid spell at level %lu being ignored", levelIndex+1)) continue;
        unsigned int spellIndex = spell->getIndex();
        if (APP_WARNING(spellIndex > MAX_SPELL_TYPES)("Magic %lu is out of the maximum range for number of effects"))
            continue;

        // Make this spell available
        mask[levelIndex].setBit(spellIndex);
    }

    // Go through each level and make sure that level has the previous level's abilities
    for (int i = 1; i < HIGHEST_DEFINED_LEVEL; ++i)
        mask[i].unionWith(&mask[i-1]);
}



//------------------------------------------------------------------------------------------------
// Name: allowDuplicates
// Desc: Determines whether or not this collection allows duplicate entries to
//       be held internally.
//       @return Flag value
//------------------------------------------------------------------------------------------------
bool PlayerCharacterClassLevelSpells::allowDuplicates() const
{
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: keepSorted
// Desc: Determines whether or not the derived collection type requires that its
//       entries be kept sorted.
//------------------------------------------------------------------------------------------------
bool PlayerCharacterClassLevelSpells::keepSorted() const
{
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: The name of this class type
//       @return dc::dcClass name string
//------------------------------------------------------------------------------------------------
std::string PlayerCharacterClassLevelSpells::staticTypeName()
{
    return std::string("Player Character Class Level Effects");
}


//------------------------------------------------------------------------------------------------
// Name: Element
// Desc: Initializes this resource
//------------------------------------------------------------------------------------------------
PlayerCharacterClassLevelSpells::Element::Element()
{
    member("Level", &myLevel);
    member("Magic", &mySpell);
}



//------------------------------------------------------------------------------------------------
// Name: compareTo
// Desc: Compares this element with another resource.  Only levels are compared.
//------------------------------------------------------------------------------------------------
int PlayerCharacterClassLevelSpells::Element::compareTo(const dc::dcGenericResource* other) const
{
    if (other)
    {
        if (getTypeID() == other->getTypeID())
            return myLevel.compareTo(&((Element*)other)->myLevel);
        else if (myLevel.getTypeID() == other->getTypeID())
            return myLevel.compareTo(other);
    }

    // Use the generic implementation
    return dc::dcGenericResource::compareTo(other);
}




//------------------------------------------------------------------------------------------------
// Name: getLevelValue
// Desc: Obtains the level at which this element becomes active
//------------------------------------------------------------------------------------------------
int PlayerCharacterClassLevelSpells::Element::getLevelValue() const
{
    return myLevel.getValue();
}



const dc::dcList<Evidyon::Spell>::Element* PlayerCharacterClassLevelSpells::Element::getSpell() const
{
    return mySpell.getReferencedResource();
}



//------------------------------------------------------------------------------------------------
// Name: set
// Desc: Sets this element's contents
//------------------------------------------------------------------------------------------------
void PlayerCharacterClassLevelSpells::Element::set(int level, dc::dcList<Evidyon::Spell>::Element* magic)
{
    myLevel.setValue(level);
    mySpell.setReferencedResource(magic);
}





//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: The name of this class type
//------------------------------------------------------------------------------------------------
std::string PlayerCharacterClassLevelSpells::Element::staticTypeName()
{
    return std::string("Element of PlayerCharacterClassLevelSpells");
}

