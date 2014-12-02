#ifndef __CLIENT_GAMEFILE_H__
#define __CLIENT_GAMEFILE_H__


#include <d3dx9.h> // TODO: remove this dependency
#include <mmsystem.h> // TODO: remove this dependency

#include "../evidyon_actor.h"
#include "../evidyon_avatar.h"


/**
 * 
 */
enum EquipmentSlot {
  EQUIPMENTSLOT_NONE,     // this item cannot be equipped
  EQUIPMENTSLOT_ACCESSORY, // amulet, bracelet, charm, earring, anklet
  EQUIPMENTSLOT_RING,
  EQUIPMENTSLOT_2HWEAPON,
  EQUIPMENTSLOT_1HWEAPON,
  EQUIPMENTSLOT_BELT,
  EQUIPMENTSLOT_BOOTS,
  EQUIPMENTSLOT_BODYARMOR,
  EQUIPMENTSLOT_GLOVES,
  EQUIPMENTSLOT_HELMET,
  EQUIPMENTSLOT_SHIELD,
};

static const int CLIENTGAMEFILE_RACE_STRING_LENGTH  = 16;
static const int CLIENTGAMEFILE_RACE_DESCRIPTION_STRING_LENGTH = 128;
static const int CLIENTGAMEFILE_CLASS_STRING_LENGTH  = 16;
static const int CLIENTGAMEFILE_CLASS_DESCRIPTION_STRING_LENGTH = 128;
static const int CLIENTGAMEFILE_ITEM_NAME_STRING_LENGTH = 32;
static const int CLIENTGAMEFILE_ITEM_DESC_STRING_LENGTH = 256;
static const int CLIENTGAMEFILE_ITEM_PROP_FIELD_DESC_LENGTH = 32;

/**
 * Defines how an item is represented to the client
 */
struct ClientItemDescription
{
  /// What this item is called
  char name[CLIENTGAMEFILE_ITEM_NAME_STRING_LENGTH];

  /// How this item is described
  char description[CLIENTGAMEFILE_ITEM_DESC_STRING_LENGTH];

  /// Which scenery element to use for this item (not equipped representation)
  size_t sceneryRepresentationIndex;

  // Can we use this item?  These fields are used by the client to allow a key
  // to be bound to this item
  bool consumable, consuming_requires_target;

  // If this is not a consumable, these fields are appended when the
  // corresponding bit is set in the props mask.
  char prop_field_descriptions[8][CLIENTGAMEFILE_ITEM_PROP_FIELD_DESC_LENGTH];
};


/**
 * Define different actions that actors can perform.  Shared between editor and client.
 */
enum ActorAnimation
{
  ACTORANIMATION_IDLE,
  ACTORANIMATION_COMBAT_IDLE,
  ACTORANIMATION_RUN,
  ACTORANIMATION_RUN_ATTACK,
  ACTORANIMATION_DIE1,
  ACTORANIMATION_DIE2,
  ACTORANIMATION_DIE3,
  ACTORANIMATION_DIE4,
  ACTORANIMATION_ATTACK1,
  ACTORANIMATION_ATTACK2,
  ACTORANIMATION_ATTACK3,
  ACTORANIMATION_ATTACK4,
  ACTORANIMATION_BLOCK1, // different types usually used for
  ACTORANIMATION_BLOCK2, // block w/ shield, block w/ weapon, ...
  ACTORANIMATION_BLOCK3,
  ACTORANIMATION_CASTSPELL1,
  ACTORANIMATION_CASTSPELL2,
  ACTORANIMATION_GOTHIT,
  ACTORANIMATION_MISC1,
  ACTORANIMATION_MISC2,
  ACTORANIMATION_MISC3,
  ACTORANIMATION_MISC4,

  ACTORANIMATION_COUNT,

  ACTORANIMATION_INVALID,
};


/**
 * Gives information about how actors look on the client
 */
struct ClientActorTypeDescription
{
  unsigned long skinnedMeshIndex;
  unsigned long actionAnimationIndices[ACTORANIMATION_COUNT];
  float actionAnimationSpeedFactors[ACTORANIMATION_COUNT];
  float actionAnimationSpeedOffsets[ACTORANIMATION_COUNT];
  unsigned long skinTexture;
  D3DXMATRIX rootTransform;
  float sizeRadiusSq, height;
  unsigned int soundIndices[Evidyon::ACTORSOUND_COUNT];
  int attachmentPointBoneMatrices[Evidyon::ACTORATTACHMENTPOINT_COUNT];
  D3DXMATRIX attachmentPointTransforms[Evidyon::ACTORATTACHMENTPOINT_COUNT];
};


/**
 * Stores sound information
 */
struct ClientSound
{
  /**
   * This header structure is stored exactly in the compiled game file
   */
  struct Header
  {
    /// Format information used to create the buffer for this sound
    WAVEFORMATEX waveFormat;

    /// How many bytes to load into the waveform data buffer
    unsigned int waveformSize;

  } header;

  /// Data to load into the sound buffer
  BYTE* waveformData;
};




#endif