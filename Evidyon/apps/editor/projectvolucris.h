//------------------------------------------------------------------------------------------------
// File:    projectvolucris.h
//
// Desc:    The resource which contains all of the other resources that are editable in the PV
//          resource editor.  This makes it very easy to save, load, compile, merge and edit
//          an entire project.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __PROJECTVOLUCRIS_H__
#define __PROJECTVOLUCRIS_H__


#include "spell.h"
#include "visualfximage.h"
#include "visualfx.h"
#include "magicfx.h"
#include "projectile.h"
#include "enchantmentclasslist.h"
#include "lifeformai.h"
#include "treasuregeneration.h"
#include "avatarracelist.h"
#include "avatarclasslist.h"


/**
 * Contains all of the resources that can be edited
 */
class ProjectVolucris : public dc::dcResource<ProjectVolucris>
{
public:

  /**
   * Initializes this class
   */
  ProjectVolucris();

  /**
   * The name of this resource type
   * @return Resource type name
   */
  static std::string staticTypeName();

  /**
   * Erases all of the information in this structure
   */
  void clear();

  void getReservedNames(std::list<std::string>* names);

public:

  inline dc::dcTable<ResourcePack>* getResourcePackTable() { return &myResourcePacks; }
  inline dc::dcTable<Image>::Reference* getFontImage() { return &myFontImage; }
  inline dc::dcTable<Image>::Reference* getTitleMainImage() { return &myTitleMainImage; }
  inline dc::dcTable<Image>::Reference* getTitleBackgroundImage() { return &myTitleBackgroundImage; }
  inline dc::dcTable<Image>::Reference* getGUIImage() { return &myGUIImage; }
  inline dc::dcTable<Image>::Reference* getEvidyonSymbolImage() { return &myEvidyonSymbolImage; }
  inline dc::dcTable<Image>::Reference* getDayNightLightParticleImageImage() { return &myDayNightLightParticleImage; }
  inline dc::dcTable<Image>* getImageTable() { return &myImages; }
  inline dc::dcTable<Texture>* getTextureTable() { return &myTextures; }
  inline dc::dcList<Skin>* getSkinList() { return &mySkins; }
  inline dc::dcTable<Mesh>* getMeshTable() { return &myMeshes; }
  inline dc::dcList<Scenery>* getSceneryList() { return &myScenery; }
  inline dc::dcList<Terrain>* getTerrainList() { return &myTerrain; }
  inline dc::dcList<TerrainRule>* getTerrainRulesList() { return &myTerrainRules; }
  inline dc::dcList<Elevation>* getElevations() { return &myElevations; }
  inline dc::dcTable<LifeformAI>* getLifeformAITable() { return &myLifeformAITable; }
  inline dc::dcList<Atlas>* getAtlasList() { return &myAtlases; }
  inline AvatarRaceList* getAvatarRaceList() { return &avatar_race_list_; }
  inline AvatarClassList* getAvatarClassList() { return &avatar_class_list_; }
  inline dc::dcTable<Magic>* getMagicList() { return &myMagic; }
  inline dc::dcList<Spell>* getSpellsList() { return &mySpells; }
  inline dc::dcList<Item>* getItemsList() { return &myItems; }
  inline dc::dcList<UnseenSkinnedMeshResource>* getSkinnedMeshesList() { return &mySkinnedMeshes; }
  inline dc::dcList<UnseenKeyframeAnimation>* getSkinnedMeshAnimationsList() { return &mySkinnedMeshAnimations; }
  inline dc::dcList<ActorType>* getActorTypesList() { return &myActorTypes; }
  inline LevelDefinitions* getLevelDefinitions() { return &myLevelDefinitions; }
  inline dc::dcList<WAVSound>* getWAVSoundList() { return &myWAVSoundList; }

  inline dc::dcList<VisualFXImage>* getVisualFXImagesList() { return &myVisualFXImages; }
  inline dc::dcList<MagicFX>* getMagicFXList() { return &myMagicFX; }
  inline dc::dcTable<VisualFX>* getVisualFXTable() { return &myVisualFX; }
  inline EnchantmentClassList* getEnchantmentClassList() { return &myEnchantments; }

  inline dc::dcList<Projectile>* getProjectiles() { return &myProjectiles; }

  inline TreasureGeneration* getTreasureGeneration() { return &treasure_generation_; }

  size_t getFootstepSoundResourceIndex() { return myFootstepSound.getReferencedResourceIndex(); }
  size_t getButtonSoundResourceIndex() { return myButtonSound.getReferencedResourceIndex(); }
  size_t getPhysicalHitMagicFXResourceIndex() { return myPhysicalHitMagicFX.getReferencedResourceIndex(); }
  size_t getSpawnMagicFXResourceIndex() { return mySpawnMagicFX.getReferencedResourceIndex(); }


protected:

  /// Resource packs used by this project
  dc::dcTable<ResourcePack> myResourcePacks;

  dc::dcTable<Image>::Reference myFontImage;
  dc::dcTable<Image>::Reference myTitleMainImage;
  dc::dcTable<Image>::Reference myTitleBackgroundImage;
  dc::dcTable<Image>::Reference myGUIImage;
  dc::dcTable<Image>::Reference myEvidyonSymbolImage;
  dc::dcTable<Image>::Reference myDayNightLightParticleImage;

  dc::dcTable<Image> myImages;      /// The images in the editor
  dc::dcTable<Texture> myTextures;  /// Textures
  dc::dcList<Skin> mySkins;       /// Skins
  dc::dcTable<Mesh> myMeshes;     /// Meshes
  dc::dcList<Scenery> myScenery;  /// Scenery
  dc::dcList<Terrain> myTerrain;  /// Terrain types
  dc::dcList<TerrainRule> myTerrainRules; /// Terrain texturing rules
  dc::dcList<Elevation> myElevations; /// Heights used in the map

  /// Lists source skinned meshes
  dc::dcList<UnseenSkinnedMeshResource> mySkinnedMeshes;

  /// Holds mesh animations for the skinned meshes
  dc::dcList<UnseenKeyframeAnimation> mySkinnedMeshAnimations;

  /// Defines actor types that bring together skinned meshes
  dc::dcList<ActorType> myActorTypes;


  dc::dcList<Atlas> myAtlases;          /// The atlas controls the manifistation of the game world
  dc::dcList<WAVSound> myWAVSoundList;  /// The list of sound effects
  dc::dcList<WAVSound>::Reference myFootstepSound;
  dc::dcList<WAVSound>::Reference myButtonSound;

  /// Special effects
  dc::dcList<VisualFXImage> myVisualFXImages;
  dc::dcTable<VisualFX> myVisualFX;
  dc::dcList<MagicFX> myMagicFX;
  dc::dcList<MagicFX>::Reference myPhysicalHitMagicFX;
  dc::dcList<MagicFX>::Reference mySpawnMagicFX;

  dc::dcList<Projectile> myProjectiles;   /// Projectiles are used by magic and by physical attacks
  EnchantmentClassList myEnchantments;

  dc::dcTable<Magic> myMagic; /// Defines effects that can be employed by characters
  dc::dcList<Spell> mySpells;

  AvatarRaceList avatar_race_list_;
  AvatarClassList avatar_class_list_;

  LevelDefinitions myLevelDefinitions;  /// How much EXP it takes to advance at a certain level
  dc::dcList<Item> myItems;                   /// Items
  TreasureGeneration treasure_generation_; /// How treasure is generated
  dc::dcTable<LifeformAI> myLifeformAITable;
  dc::dcList<Contributor> myCredits;          /// Credits for the game
  //dcList<AICharacterType> myAICharacterTypes; /// Kinds of AI characters

  //dcList<MonsterClass> myMonsterClasses; /// Monster types
  //dcList<MonsterSpawn> myMonsterSpawn;   /// Spawn area
};


#endif
