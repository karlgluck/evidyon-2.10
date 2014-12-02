//------------------------------------------------------------------------------------------------
// File:  gamefiledata.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GAMEFILEDATA_H__
#define __GAMEFILEDATA_H__


#include "gamefiledata_texture.h"
#include "gamefiledata_skinnedmesh.h"
#include "../shared/client-editor/clientspelldescription.h"
#include "../shared/client-editor/clientenchantmentdescription.h"
#include <d3dx9.h>

struct ClientSound;
struct UnseenSkeletalAnimation;
struct ClientItemDescription;

typedef std::map<size_t,size_t> GameFileData_Skin;
typedef std::map<size_t,Geometry*> GameFileData_Mesh;

struct GameFileData_Scenery
{
  GameFileData_Mesh* mesh;
  GameFileData_Skin* skin;
  D3DXMATRIXA16 transform;
};

/**
 * This structure is instantiated inside of the main client class, and contains all of the
 * information loaded from the source file that runs the game.
 */
struct GameFileData
{
  // This client's game file version
  unsigned long fileVersion;

  /**
   * These are all of the images that are loaded from the game file.  These are all loaded as
   * full Direct3D textures that are ready to be used.  Failing to load a texture doesn't cause
   * the loading process to fail entirely; it will simply set the corresponding element in the
   * array to 'null'.
   */
  struct
  {
    // How many elements are in the texture array
    size_t d3dTextureArraySize;

    /// The array of textures
    LPDIRECT3DTEXTURE9* d3dTextureArray;

    /// Reference to the font texture
    LPDIRECT3DTEXTURE9 d3dTextureFont;

    /// The left side of the title texture
    LPDIRECT3DTEXTURE9 d3dTextureTitle;

    /// The right side of the title texture
    LPDIRECT3DTEXTURE9 d3dTextureTitleBackground;

    /// The GUI texture
    LPDIRECT3DTEXTURE9 d3dTextureGUI;

    /// The login screen background texture (Evidyon symbol)
    LPDIRECT3DTEXTURE9 d3dTextureEvidyonSymbol;

    LPDIRECT3DTEXTURE9 d3dTextureDayNightLightParticle;

  } images;



  /**
   * 
   */
  struct
  {

    GameFileData_Skin* skinArray;
    size_t skinArraySize;

  } skins;


  /**
   *
   */
  struct
  {
    GameFileData_Mesh** meshArray;

    size_t meshArraySize;

  } meshes;

  /**
   *
   */
  struct
  {
    GameFileData_Scenery* sceneryArray;

    size_t sceneryArraySize;

  } scenery;

  struct AvatarRace
  {
    std::string name;
    std::string description;
    int max_stat[5];
  };

  struct AvatarClass
  {
    std::string name;
    std::string description;
  };

  struct
  {
    /// Holds information about the different playable races
    AvatarRace* raceArray;
    size_t raceArraySize;

    /// Holds information about the different playable classes
    AvatarClass* classArray;
    size_t classArraySize;

  } avatars;

  struct
  {
    ClientSound* soundArray;
    size_t soundArraySize;

  } sounds;

  struct
  {
    Evidyon::ClientSpellDescription* spell_descriptions_;
    size_t number_of_spell_descriptions;
  } spells;

  struct
  {
    GameFileDataSkinnedMesh* skinnedMeshes;
    size_t numberOfSkinnedMeshes;

    UnseenSkeletalAnimation* animations;
    size_t numberOfAnimations;

  } animatedMeshes;

  struct
  {
    /// Holds information about item types
    ClientItemDescription* itemDescriptionArray;
    size_t itemDescriptionArraySize;
  } items;
};


#endif