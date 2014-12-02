//------------------------------------------------------------------------------------------------
// File:  servergamefile.h
//
// Desc:  Contains definitions that both the server and the game editor need to access when
//      accessing or creating the game file.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __SERVER_GAMEFILE_H__
#define __SERVER_GAMEFILE_H__

#include "serverspelldescription.h"

#define MAX_AICHARACTER_NAME_LENGTH 32

#include "../evidyon_actor.h"
#include "../evidyon_avatar.h"

namespace Evidyon {

struct AvatarRaceDescription {
  int level1AttributeMaximums[5];
  int male_actor_template_index,
      female_actor_template_index;
  unsigned int hair_styles[Evidyon::AVATARGENDER_COUNT][Evidyon::AVATAR_RACE_HAIR_STYLES_PER_GENDER];
};

struct AvatarClassDescription
{
  float hpModifier, mpModifier;

  // all of the spells that this class gets access to at the different levels
  SpellAvailabilityMask spellAvailabilityMask[HIGHEST_DEFINED_LEVEL];

  float abilityDelayFactor, abilityDelayOffset;
};



#pragma pack(push, 1)

/**
 * This bitfield structure is used to identify which slots are filled on
 * a character, and which slots are required to equip it.
 */
union ServerEquipmentSlotMask {
  struct {
    // Items that aren't visible on the character
    unsigned accessory0 : 1;
    unsigned accessory1 : 1;
    unsigned accessory2 : 1;
    unsigned ring0    : 1;
    unsigned ring1    : 1;
    unsigned ring2    : 1;
    unsigned ring3    : 1;
    unsigned ring4    : 1;
    unsigned ring5    : 1;
    unsigned belt     : 1;
    unsigned gloves   : 1;

    // Items that are visible on the character
    unsigned weaponHand : 1;
    unsigned shieldHand : 1;
    unsigned boots    : 1;
    unsigned bodyArmor  : 1;
    unsigned helmet   : 1;

    // Unused bits
    unsigned __unused : 16;

  } fields;

  unsigned long mask;

  /**
   * This mask is going to be used for an item, so its mask should reflect the slots into
   * which it can be placed.  The slots should be set to 1 in the 'fields' bitfield.
   */
  void item_initialize();

  /**
   * This mask is going to be used for equipment, so it should reflect the available slots
   * on a character.  Thus, all slots where no items are equipped are '1'.  This method
   * makes all slots available.
   */
  void equipment_initialize();

  /**
   * Returns 'false' if the item can't be equipped because other equipment blocks it
   */
  bool equipment_equipMultipleSlotItem(const ServerEquipmentSlotMask* itemFilledSlotsMask);

  /**
   *
   */
  void equipment_unequipMultipleSlotItem(const ServerEquipmentSlotMask* itemFilledSlotsMask);

  /**
   * Gets the index of a slot into which an item that is able to fill single, different slots can be placed
   * and makes it unavailable.  If this method returns 'false', the item can't be equipped.
   */
  bool equipment_equipSingleSlotItem(const ServerEquipmentSlotMask* itemCompatibilityMask);
  static int getLowestBitSet(unsigned long ul);
  void equipment_unequipSingleSlotItem(const ServerEquipmentSlotMask* itemCompatibilityMask);
};


#pragma pack(pop)




// to be added:
//  - reditus, maximus chances
//  - thorns (return damage)
//  - reflect projectiles
union CompiledItemAvatarModifierMask {
  enum FieldType {
    MIN_ATTACK_DAMAGE,
    MAX_ATTACK_DAMAGE,
    PHYSICAL_DEFENSE,
    MAGICAL_DEFENSE,
    HP_REGEN_RATE,
    MP_REGEN_RATE,
    MAX_HP_MULTIPLIER,
    MAX_MP_MULTIPLIER,
    MAX_HP_ADDED,
    MAX_MP_ADDED,
    LEVEL,
    MOVEMENT_SPEED,
    SPELL0,
    SPELL1,
    ATTACK_SPEED,
    ACTOR_TYPE,
    STEAL_HP,
    STEAL_MP,
    NUMBER_OF_FIELDS,
  };
  struct {
    unsigned min_attack_damage : 1;
    unsigned max_attack_damage : 1;
    unsigned physical_defense : 1;
    unsigned magical_defense : 1;
    unsigned hp_regen_rate : 1;
    unsigned mp_regen_rate : 1;
    unsigned max_hp_multiplier : 1;
    unsigned max_mp_multiplier : 1;
    unsigned max_hp_added : 1;
    unsigned max_mp_added : 1;
    unsigned level : 1;
    unsigned movement_speed : 1;
    unsigned spell0 : 1;
    unsigned spell1 : 1;
    unsigned attack_speed : 1;
    unsigned actor_type : 1;
    unsigned steal_hp : 1;
    unsigned steal_mp : 1;

    //unsigned __unused : 14;

  } fields;

  unsigned int value;
};

static const size_t s = sizeof(CompiledItemAvatarModifierMask);


struct CompiledItemAvatarModifier {

  // these masks define what attributes to apply and when they
  // should be processed.
  CompiledItemAvatarModifierMask always_applied;
  CompiledItemAvatarModifierMask props;

  int min_attack_damage, max_attack_damage;
  int hp_regen_rate;
  int mp_regen_rate;
  float max_hp_multiplier, max_mp_multiplier;
  int max_hp_added, max_mp_added;
  int level;
  float movement_speed_multiplier;

  int spells_to_make_available[2];

  float attack_speed_multiplier;

  // A table indexed by gender, race and class that defines an actor
  // index assigned when this item type is equipped.
  int actor_indices[AVATARGENDER_COUNT][AVATARRACE_COUNT][AVATARCLASS_COUNT];

  int physical_defense;
  int magical_defense;

  float steal_hp;
  float steal_mp;
};


/*
 NOTE: multiple genite levels are handled by having entirely
       different items!
*/


/**
 * Describes an item on the server
 */
struct ServerItemDescription {

  // apply this modifier when the item is equipped
  CompiledItemAvatarModifier avatar_modifier;

  // this is the index of this item in the item list
  unsigned int type_index;

  // This is a unique value that is deterministically generated for each item
  // based on its name as defined in the game file.  Since items can change
  // order between compilations of the game file, when an avatar is saved to
  // the server's database, all of its items are typed using this hash value.
  // When the avatar is retrieved, the item manager looks up this value to
  // find the item index.
  unsigned int permanent_storage_hash;

  // How many units of this item can be stored on top of one another
  int max_stack_size;

  // The slots that this item can be placed into
  ServerEquipmentSlotMask equipment_slot_mask;

  // If this is 'true', then the item is like a two-handed weapon:  it fills multiple equipment slots.  If
  // it is not a multiple-slot item, then the mask specifies a range of slots that this item can fill.
  bool fills_multiple_slots;

  // The scenery that represents this item when it is equipped
  ActorAttachmentPoint equipped_attachment_point;
  size_t equipped_scenery_index;

  // The magic type to activate when the user consumes this item.  If this is < 0,
  // this item cannot be consumed.
  int consumable_magic;

  // When showing this item on the ground, this is the value passed to the
  // client in the network message.  Maximum of 16 bits.
  unsigned int ground_type_index;

  // How much geonite this item is worth when sacrificed
  unsigned int geonite_value;
};



}


#endif