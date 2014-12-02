//------------------------------------------------------------------------------------------------
// File:    item.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __ITEM_H__
#define __ITEM_H__

namespace Evidyon {
struct ServerItemDescription;
}

struct ClientItemDescription;
#include <dc/list>
#include "scenery.h"
#include <dc/enum>
#include <dc/string>
#include "spell.h"



#include "itemavatarmodifier.h"

/**
 * 
 */
class Item : public dc::dcResource<Item>
{
public:

  /**
   * Initializes this class
   */
  Item();

  /**
   * Enters data into the server effect structure
   */
  bool fillServerDescription(ServerItemDescription* description) const;

  /**
   * Enters data into the client effect structure
   */
  bool compileForClient(ClientItemDescription* description) const;

  /**
   * Sets the scenery visualization of this item
   */
  void setSceneryRepresentation(dcList<Scenery>::Element* scenery);

  dc::dcList<Scenery>::Reference* getSceneryReference() { return &mySceneryRepresentation; }

  /**
   * Used to sort this list.  If the static "compiling" flag is set, the items are
   * sorted by type ID.  Otherwise, they are sorted by their equipment slot.
   */
  int compareTo(const dc::dcGenericResource* other) const;

  inline int getTreasureLevelValue() const { return treasure_level_.getValue(); }

  inline dc::dcEnum<EquipmentSlot>& getSlot() { return myEquipmentSlot; }

  inline ItemAvatarModifier& getItemAvatarModifier() { return avatar_modifier_; }

public:

  /**
   * Returns the type name of this class
   */
  static std::string staticTypeName();


protected:

  // this is the name that the item is given in the client's game file.
  dc::dcString displayed_name_;

  dc::dcEnum<EquipmentSlot> myEquipmentSlot;  /// Which slot in the "equipped items" region to place this item in
  dc::dcString myDescription; /// The textual description of this item
  dc::dcList<Scenery>::Reference mySceneryRepresentation; /// What this item looks like when it is on the ground (or item preview in inventory)

  dc::dcByte max_stack_size_;
  //dc::dcFloat weight_;

  dc::dcInteger treasure_level_;  // how rare this item is
  dc::dcInteger geonite_value_;    // amount of geonite gained when sacrificed

  dc::dcTable<Magic>::Reference consumable_magic_;
  dc::dcBoolean consumable_needs_target_;
  ItemAvatarModifier avatar_modifier_; // what this item does
};





#endif