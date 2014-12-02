//------------------------------------------------------------------------------------------------
// File:    playercharacterclassleveleffects.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __PLAYERCHARACTERCLASSLEVELSPELLS_H__
#define __PLAYERCHARACTERCLASSLEVELSPELLS_H__

#include <dc/integer>
#include <dc/list>
#include "spell.h"


class PlayerCharacterClassLevelSpells : public dc::dcCollection<PlayerCharacterClassLevelSpells>
{
    public:

  /**
   * Associates two values with each other
   */
  class Element : public dc::dcResource<Element> {
  public:
    /**
     * Initializes this resource
     */
    Element();

    /**
     * Compares this element with another resource.  Only levels are compared.
     */
    int compareTo(const dc::dcGenericResource* other) const;

    /**
     * Obtains the level at which this element becomes active
     */
    int getLevelValue() const;

    /**
     * Gets the spell that this element references
     */
    const dc::dcList<Evidyon::Spell>::Element* getSpell() const;

    /**
     * Sets this element's contents
     */
    void set(int level, dc::dcList<Evidyon::Spell>::Element* effect);


  public:
    /**
     * The name of this class type
     * @return dc::dcClass name string
     */
    static std::string staticTypeName();


  protected:

    /// The level of this spell
    dc::dcInteger myLevel;

    /// The spell to make available
    dc::dcList<Evidyon::Spell>::Reference mySpell;
  };

  /**
   * Fills out the mask with available spells
   */
  void fillClassSpellAvailabilityMask(Evidyon::SpellAvailabilityMask mask[Evidyon::HIGHEST_DEFINED_LEVEL]) const;

  /**
   * Determines whether or not this collection allows duplicate entries to
   * be held internally.
   * @return Flag value
   */
  bool allowDuplicates() const;

  /**
   * Determines whether or not the derived collection type requires that its
   * entries be kept sorted.
   */
  bool keepSorted() const;


public:

  /**
   * The name of this class type
   * @return dc::dcClass name string
   */
  static std::string staticTypeName();

};


#endif