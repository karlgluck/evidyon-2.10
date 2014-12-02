//------------------------------------------------------------------------------------------------
// File:    atlas.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __ATLAS_H__
#define __ATLAS_H__

#include <dc/integer>
#include "location.h"
class Atlas;
typedef dc::dcInteger WorldCoordinateType;
typedef Location WorldLocationType;

#include "stdafx.h"
#include "allgameresources.h"
#include <dcx/d3d>

#include <d3dx9.h>
#include <dc/enum>
#include <dc/list>
#include "maplocationspecialfunction.h"
#include "terrain.h"
#include "location.h"
#include "swarmspawner.h"
#include "rarityscale.h"

#include "wavsound.h"

class AtlasCompilationContext;

/**
 * Allows the user to link up a bunch of map layers into an actual world atlas
 */
class Atlas : public dc::dcResource<Atlas> {
public:

  /**
   * Initializes this class
   */
  Atlas();

  /**
   * Gets the name of this resource types
   */
  static std::string staticTypeName();

  /**
   * Loads map components and generates a preview window which lets the user visually edit
   * the world contents.
   */
  void edit();

  inline dc::dcList<Terrain>::Reference* getDefaultTerrain(size_t index0Or1) { return &myDefaultTerrainRef[index0Or1]; }
  inline float getDefaultTerrainElevationValue(size_t index0Or1) { return myDefaultTerrainElevations[index0Or1].getValue(); }
  inline dc::dcEnum<MapNavigability>* getDefaultNavigability() { return &myDefaultNavigability; }
  inline dc::dcList<MapLayer>* getMapLayers() { return &myMapLayers; }
  inline dc::dcMap<WorldLocationType,MapLocationInfo>* getDetailedLocations() { return &myDetailedLocations; }
  inline Location::BasicType getSpawnLocationValue() const { return mySpawnLocation.getValue(); }
  inline float getSpawnRadiusValue() const { return mySpawnRadius.getValue(); }
  inline unsigned int getBackgroundSoundLoopIndex() const { return background_sound_loop_.getReferencedResourceIndex(); }

  /**
   * This method fills the output context class with data.  If anything is currently in
   * the context, it is removed.
   */
  bool compile(LPDIRECT3DDEVICE9 d3dDevice, AtlasCompilationContext* context);

  // Compiles information for the server that is written after the compilation context
  // has been used.  This method writes out, for example, special functions of the map,
  // monster spawning information, and more.
  // The monster level rarity boost is created by the treasure generator.  It specifies
  // how much of an increase in treasure-level is given to a region's swarm spawner
  // depending on its average monster level.
  bool compileServerData(LPDIRECT3DDEVICE9 d3d_device,
                         dc::dcStreamOut* serverStream,
                         int map_width,
                         int map_height,
                         int monster_level_rarity_boost[RARITY_SCALE_SIZE]);

protected:

  /// The terrain type that is applied if no other type can be found.  Each terrain layer
  /// has its own default terrain.
  dc::dcList<Terrain>::Reference myDefaultTerrainRef[2];

  /// The default terrain heights for each of the layers
  dc::dcFloat myDefaultTerrainElevations[2];

  /// The default navigability type of areas outside of the map
  dc::dcEnum<MapNavigability> myDefaultNavigability;

  /// Unique things that map locations do; for example, transport between maps
  dc::dcTable<MapLocationSpecialFunction> mySpecialFunctions;

  /// Stores map template information
  dc::dcList<MapLayer> myMapLayers;

  /// A list of specific, hand-edited modifications to the world (these always override
  /// whatever is in the template).
  dc::dcMap<WorldLocationType,MapLocationInfo> myDetailedLocations;

  /// This is the range in which characters are spawned in the world
  Location mySpawnLocation;
  dc::dcFloat mySpawnRadius;

  /// Spawn points on this map
  SwarmSpawner swarm_spawner_;

  // The sound loop to play in the background
  dc::dcList<WAVSound>::Reference background_sound_loop_;

  // An inhabitant is spawned only at a certain location.  There is only ever a single
  // instance of the controller active at any given time.
  typedef dc::dcMap<WorldLocationType, dc::dcTable<LifeformAI>::Reference> Inhabitants;
  Inhabitants inhabitants_;
};




#endif

