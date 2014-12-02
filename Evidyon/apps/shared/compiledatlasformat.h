//------------------------------------------------------------------------------------------------
// File:  compiledatlasformat.h
//
// Desc:  This file contains all of the declarations that are used in the compiled atlas format
//      for the client and server.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __COMPILEDATLASFORMAT_H__
#define __COMPILEDATLASFORMAT_H__

#include "../shared/evidyon_map.h"

/**
 * This specifies the navigability of a location
 */
/*
enum MapNavigability
{
  MAPNAV_WALK,
  MAPNAV_WADE,
  MAPNAV_SWIM,
  MAPNAV_PIT,
  MAPNAV_LOWWALL,
  MAPNAV_HIGHWALL,
  MAPNAV_IMPASSABLE,
  MAPNAV_SAFEZONE,

  MAPNAV_COUNT,
};*/

/// TODO: mapchunk.cpp in the client needs a reference to the subset for a wall; it should be
/// always subset zero, but this needs to be checked somehow
#define TERRAINMESHSUBSET_WALL_SIDES  0


/*
Client Data Structure:

[ Header Information ]
[ Terrain Meshes ]
[ Description Table ]
[ Map ] - this is an image that specifies either an offset into the description table, or a
      default value which indicates that the description should be pulled from the
      list of irregular descriptions
[ Chunk To Irregular Offset List ]
[ Irregular Descriptions ]

*/

/**
 * This structure contains the data that is used in the client map to describe
 * a given location.
 */
struct CompiledAtlasClientLocationDescription
{
  Evidyon::MapNavigability navigability;
  size_t sceneryIndex;

  struct TerrainData
  {
    // The index of the terrain mesh to render here.  These are separate from the
    // main list of meshes.
    size_t terrainMeshIndex;

    // The main texture for rendering
    size_t textureIndex;

    // If this is valid, a wall is rendered
    size_t wallTextureIndex;

    /// How much to translate the mesh used to render this terrain type along Y 
    float elevation;

    /// How much to scale this element vertically
    float height;
  };

  /// Information for each layer of the terrain
  TerrainData terrain[2];
};

#ifdef DIRECT3D_VERSION

/**
 * This structure contains the header that comes before the map that is used by the client
 */
struct CompiledAtlasClientHeader
{
  /// How many bytes-per-pixel are in the image->description map
  size_t mapBytesPerPixel;

  /// What format the atlas's map is in
  D3DFORMAT mapImageFormat;

  /// The size of the map being loaded
  size_t width, height;

  /// This is information about the location that should be displayed when no
  /// location is found in the map
  Evidyon::MapNavigability defaultNavigability;
  size_t defaultTextureIndex;

  unsigned int background_sound_loop;

  /// Lists the byte locations in the file of the different segments
  struct
  {
    size_t start;
    size_t terrainMeshes;
    size_t descriptionTable;
    size_t map;
    size_t chunkToNontableDescriptionIndicesTable;
    size_t nontableDescriptions;
    size_t end;

  } fileLocations;
};


#define COMPILEDATLAS_MAP_PIXEL_DATATYPE     WORD
#define COMPILEDATLAS_MAP_FORMAT         D3DFMT_R5G6B5
#define COMPILEDATLAS_MAP_BYTES_PER_PIXEL    sizeof(COMPILEDATLAS_MAP_PIXEL_DATATYPE)
#define COMPILEDATLAS_MAP_INDEX_NOTINTABLE   ((1<<(COMPILEDATLAS_MAP_BYTES_PER_PIXEL*8))-1)
#define COMPILEDATLAS_MAP_LOOKUPTABLE_SIZE   (COMPILEDATLAS_MAP_INDEX_NOTINTABLE)
#define COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS    16
#define COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS   16

#endif

/*
Server Data Structure:

[ Header Information ]
[ Navigability Table ]

*/
#include "./server-editor/serveratlas.h"





#endif