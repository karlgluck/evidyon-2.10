#ifndef __GLOBALMAPMANAGER_H__
#define __GLOBALMAPMANAGER_H__


#include <d3dx9.h>
#include "../shared/compiledatlasformat.h"


namespace dc {
class dcStreamIn;
}


struct _iobuf;
typedef _iobuf FILE;
class IsometricCamera;
struct GameFileData;

struct Geometry;

namespace Evidyon {

class GlobalRenderingManager;

struct MapChunkBatchRenderingParameters {
  UINT minimumVertexIndex;  /// The lowest vertex index used in this batch
  UINT numberOfVertices;  /// How many vertices are in this batch
  UINT startIndex;  /// The index in the IB at which to start reading triangles to render
  UINT triangles; /// How many triangles to render in this batch
  int texture_index;  /// Information on to render this batch
};


struct MapChunk {
  bool isValid; /// Whether or not this chunk is filled with valid data
  int chunk_x, chunk_y; /// This chunk's "tag" (used by the caching algorithm)

  /// The index of the last frame during which this chunk was marked as
  /// visible.  If this frame is not equal to the client's current frame
  /// counter, this chunk is skipped during rendering.
  DWORD lastVisibleFrame;

  /// The batches to render in this chunk
  MapChunkBatchRenderingParameters* batchArray;

  /// The number of entries in the batch array
  size_t batchArraySize;

  /// The vertices to use to render this chunk's terrain
  LPDIRECT3DVERTEXBUFFER9 myVB;

  /// The indices to use when rendering this chunk's terrain
  LPDIRECT3DINDEXBUFFER9 myIB;
};


//struct MapData {
//};

static const int MAP_CACHE_WIDTH_IN_CHUNKS  = 8;
static const int MAP_CACHE_HEIGHT_IN_CHUNKS   = 6;

class GlobalMapManager {
public:
  GlobalMapManager();

  // Sets up this manager and loads the map from the file
  bool createFromGameFile(LPDIRECT3DDEVICE9 d3d_device, GameFileData* game_file_data, dc::dcStreamIn* game_file,
                          GlobalRenderingManager* global_rendering_manager);
  void destroy();

  // Frees all of the data used by the map chunk cache
  void clearCache();

  bool render(IsometricCamera* camera,
        unsigned int texture_id);
  void update(float center_x, float center_y);

  // Switches the map that the manager is currently displaying
  void changeMap(int map_id, unsigned int* new_background_loop);


public: // debugging
  void printDebugStats(char* buffer, size_t buffer_size) const;

private:

  // Draws a map chunk's geometry using the given texture ID to the screen
  bool renderMapChunk(unsigned int textureID,
            IsometricCamera* camera,
            MapChunk* chunk);

  // Makes sure that a map is in the cache
  bool ensureMapChunkIsInCache(int chunk_x, int chunk_y);

  // Erases the memory used by this map chunk
  void deallocateMapChunkData(MapChunk* chunk);

// Source data from the game file
private:
  struct PerMapData {
    PerMapData();
    bool loadFromGameFile(dc::dcStreamIn* game_file,
                          LPDIRECT3DDEVICE9 d3d_device);
    void destroy();

    CompiledAtlasClientHeader header; /// The atlas header
    CompiledAtlasClientLocationDescription defaultLocation; /// Location type to display when there are no other types
    size_t chunksWide, chunksHigh;  /// The size of the map in chunks that contains data
    Geometry* terrainMeshArray; /// Source mesh data used to render terrain elements
    size_t terrainMeshArraySize;

    /// Table where the most common location descriptions are looked up.  The
    /// size of this array is always COMPILEDATLAS_MAP_LOOKUPTABLE_SIZE
    CompiledAtlasClientLocationDescription* descriptionTableArray;

    /// The map. All references should be valid since the nontable description
    /// array is being read into memory.
    CompiledAtlasClientLocationDescription** locationToDescriptionArray;
    size_t locationToDescriptionArraySize;

    /// This array holds all of the elements that are not indexed in the main table.
    /// It should hold entries that are unique enough to make increasing the size of
    /// the index in the description table array not worth it (ex. each entry should
    /// have an occurrance of less than 10 or so)
    size_t nontableDescriptionArraySize;
    CompiledAtlasClientLocationDescription* nontableDescriptionArray;
  };

  PerMapData* maps_;
  int number_of_maps_;

private:

  GlobalRenderingManager* global_rendering_manager_;
  GameFileData* game_file_data_;
  LPDIRECT3DDEVICE9 d3d_device_;

  // The map for which the chunk cache is valid
  int current_map_;

  /// The cache for map chunks.  This lookup is backward from most normal
  /// matrices; it's indexed by X then Y.
  MapChunk myMapChunkCache[MAP_CACHE_WIDTH_IN_CHUNKS][MAP_CACHE_HEIGHT_IN_CHUNKS];

  /// The current rendering frame
  DWORD myCurrentFrame;
};

}


#endif