#include "globalmapmanager.h"
#include "../common/null.h"
#include "../common/safemacros.h"
#include "../shared/geometry.h"
#include "../common/isometriccamera.h"
#include "globalrenderingmanager.h"

#include "gamefiledata.h"

#include <dc/debug>
#include <dc/dcstream.h>

// TODO: get rid of this
#include "cleanup.h"



namespace Evidyon {

static const int MAP_DISPLAY_WIDTH   = 32;
static const int MAP_DISPLAY_HEIGHT  = 32;
static const int MAX_NUMBER_OF_MAPS  = 8;


GlobalMapManager::GlobalMapManager() {
  global_rendering_manager_ = NULL;
  game_file_data_ = NULL;
  current_map_ = 0;

  d3d_device_ = NULL;
  memset(myMapChunkCache, 0, sizeof(myMapChunkCache));
  myCurrentFrame = 0;
}




bool GlobalMapManager::createFromGameFile(
  LPDIRECT3DDEVICE9 d3d_device, GameFileData* game_file_data, dc::dcStreamIn* game_file,
  GlobalRenderingManager* global_rendering_manager) {
  global_rendering_manager_ = global_rendering_manager;
  (d3d_device_ = d3d_device)->AddRef();
  game_file_data_ = game_file_data;

  size_t number_of_maps;
  CONFIRM(game_file->read(&number_of_maps, sizeof(number_of_maps))) else return false;
  ASSERT(number_of_maps <= MAX_NUMBER_OF_MAPS) else return false;
  number_of_maps_ = number_of_maps;
  maps_ = new PerMapData[number_of_maps];
  ASSERT(maps_) else return false;

  for (size_t i = 0; i < number_of_maps; ++i) {
    maps_[i].loadFromGameFile(game_file, d3d_device);
  }

  return true;
}



void GlobalMapManager::destroy() {
  if (maps_ != NULL) {
    for (int i = 0; i < number_of_maps_; ++i) {
      maps_[i].destroy();
    }
    SAFE_DELETE_ARRAY(maps_);
    number_of_maps_ = 0;
  }

  SAFE_RELEASE(d3d_device_);
}




void GlobalMapManager::clearCache() {
  for (int x = 0; x < MAP_CACHE_WIDTH_IN_CHUNKS; ++x) {
    for (int y = 0; y < MAP_CACHE_HEIGHT_IN_CHUNKS; ++y) {
      deallocateMapChunkData(&myMapChunkCache[x][y]);
    }
  }
}


bool GlobalMapManager::render(
  IsometricCamera* camera,
  unsigned int texture_id) {
  CONFIRM(d3d_device_) else return false;

  // Reset the location matrices
  D3DXMATRIXA16 mat;
  D3DXMatrixIdentity(&mat);
  camera->setWorldMatrix(d3d_device_, &mat);

  // Initialize rendering
  //camera->initialize3DRendering(d3d_device_);

  // Repeat for all of the chunks
  for (size_t x = 0; x < MAP_CACHE_WIDTH_IN_CHUNKS; ++x) {
    for (size_t y = 0; y < MAP_CACHE_HEIGHT_IN_CHUNKS; ++y) {
      // Get the chunk cached at this index
      MapChunk* chunk = &myMapChunkCache[x][y];

      // If this chunk isn't visible, skip it
      if (chunk->lastVisibleFrame != myCurrentFrame) continue;

      // Render this chunk
      renderMapChunk(texture_id, camera, chunk);
    }
  }

  // Success
  return true;
}


void GlobalMapManager::update(float center_x, float center_y) {
  ++myCurrentFrame;

  // Get the coordinates
  int maxX = (int)(center_x) + MAP_DISPLAY_WIDTH/2,
    maxY = (int)(center_y) + MAP_DISPLAY_HEIGHT/2,
    minX = (int)(center_x) - MAP_DISPLAY_WIDTH/2,
    minY = (int)(center_y) - MAP_DISPLAY_HEIGHT/2;

  // If the minima are negative, decrease them by another chunk width
  if (minX < 0) minX -= COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS;
  if (minY < 0) minY -= COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS;

  for (int x = minX; x <= maxX; x += COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS) {
    for (int y = minY; y <= maxY; y += COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS) {
      int chunkX = x / COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS,
        chunkY = y / COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS;
      ensureMapChunkIsInCache(chunkX, chunkY);
    }
  }
}


void GlobalMapManager::changeMap(int map_id, unsigned int* new_background_loop) {
  clearCache();
  CONFIRM(map_id < number_of_maps_) else return;
  current_map_ = map_id;
  *new_background_loop = maps_[map_id].header.background_sound_loop;
}



bool GlobalMapManager::renderMapChunk(
  unsigned int textureID, IsometricCamera* camera,
  MapChunk* chunk) {

  bool chunkIsInitialized = false;

  // Render each batch
  for (size_t i = 0; i < chunk->batchArraySize; ++i) {
    // Get the batch to render
    MapChunkBatchRenderingParameters* batch = &chunk->batchArray[i];

    // Make sure this batch is using the provided texture
    if (textureID != batch->texture_index) continue;

    // Put this chunk in the renderer
    if (!chunkIsInitialized) {

      // Set up this chunk to be rendered
      d3d_device_->SetFVF(D3DFVF_GEOMETRYVERTEX);
      d3d_device_->SetStreamSource(0, chunk->myVB, 0, sizeof(GeometryVertex));
      d3d_device_->SetIndices(chunk->myIB);

      // Make sure this texture is set
      global_rendering_manager_->changeCurrentTexture(batch->texture_index);

      chunkIsInitialized = true;
    }


    // Draw the polygons
    HRESULT hr = d3d_device_->DrawIndexedPrimitive(
      D3DPT_TRIANGLELIST, 0, batch->minimumVertexIndex,
      batch->numberOfVertices, batch->startIndex, batch->triangles);

    // Check to see whether or not rendering
    if (APP_ERROR(FAILED(hr))("Error rendering terrain chunk")) return false;
  }

  // Success
  return true;
}

void GlobalMapManager::printDebugStats(char* buffer, size_t buffer_size) const {
  sprintf_s(buffer,
            buffer_size,
            "MapMgr:  on map %i/%i",
            current_map_,
            number_of_maps_);
}



void GeometryDuplicateTransform(const Geometry* sourceGeometry, const D3DXMATRIX* transformation, Geometry** output)
{
  // Check parameters
  if (APP_ERROR(!sourceGeometry || !transformation || !output)("Invalid parameter to GeometryDuplicateTransform")) return;
  
  // Reset output
  *output = 0;

  // Storage for the geometry we're going to allocate
  Geometry* geometry;

  // Create geometry
  AllocateGeometry(sourceGeometry->NumVertices, sourceGeometry->NumIndices, &geometry);

  // Copy over the data
  memcpy(geometry->pVertices, sourceGeometry->pVertices, sizeof(GeometryVertex) * sourceGeometry->NumVertices);
  memcpy(geometry->pIndices, sourceGeometry->pIndices, sizeof(GeometryIndex) * sourceGeometry->NumIndices);

  // Transform the vertices in place
  D3DXVec3TransformCoordArray((D3DXVECTOR3*)geometry->pVertices, sizeof(GeometryVertex),
                 (D3DXVECTOR3*)geometry->pVertices, sizeof(GeometryVertex), transformation, geometry->NumVertices);

  // The normal transform shouldn't change magnitudes
  //D3DXMATRIX normal_transform;
  //memcpy(&normal_transform, transformation, sizeof(normal_transform));
  // TODO: either renormalize the normals or provide a matrix with only rotation/translation and not scaling
  //D3DXVec3TransformCoordArray((D3DXVECTOR3*)&geometry->pVertices->nx, sizeof(GeometryVertex),
  //               (D3DXVECTOR3*)&geometry->pVertices->nx, sizeof(GeometryVertex), transformation, geometry->NumVertices);

  // Save the output
  *output = geometry;
}


bool GlobalMapManager::ensureMapChunkIsInCache(int chunk_x, int chunk_y) {

  if (current_map_ < 0 || current_map_ > number_of_maps_) return true;
  PerMapData* map = &maps_[current_map_];

  // If this chunk is out of range, fill it with default data
  bool isDefaultChunk = chunk_x < 0 || chunk_x >= map->chunksWide ||
                        chunk_y < 0 || chunk_y >= map->chunksHigh;

  // Get this map chunk
  int chunkIndexX = chunk_x, chunkIndexY = chunk_y;
  while(chunkIndexX < 0) chunkIndexX += MAP_CACHE_WIDTH_IN_CHUNKS;
  while(chunkIndexY < 0) chunkIndexY += MAP_CACHE_HEIGHT_IN_CHUNKS;
  MapChunk* chunk = &myMapChunkCache[chunkIndexX%MAP_CACHE_WIDTH_IN_CHUNKS][chunkIndexY%MAP_CACHE_HEIGHT_IN_CHUNKS];

  // If this element is already in the cache, we don't have to remake it
  if (chunk->isValid && chunk->chunk_x == chunk_x && chunk->chunk_y == chunk_y)
  {
    // Touch the chunk to make it visible
    chunk->lastVisibleFrame = myCurrentFrame;

    // Success
    return true;
  }

  if (isDefaultChunk)
  {
    // TODO: handle this
  }

  //DEBUG_INFO("Loading chunk");

  // Free this chunk
  deallocateMapChunkData(chunk);

  // The chunk will now be validated
  chunk->isValid = true;

  // Initialize this cached chunk with new data
  chunk->lastVisibleFrame = myCurrentFrame;
  chunk->chunk_x = chunk_x;
  chunk->chunk_y = chunk_y;

  // The map coordinate of the left/top corner
  int chunkLeft = chunk_x * COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS;
  int chunkTop = chunk_y * COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS;

  // Holds all of the geometry being created for this chunk
  SubsetGeometryCollection geometryCollection;

  // Repeat for each element in this chunk
  for (size_t y = 0; y < COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS; ++y)
  {
    // The current location on the map
    int mapY = y + chunkTop;

    for (size_t x = 0; x < COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS; ++x)
    {
      int mapX = x + chunkLeft;

      // ?????
      size_t d = map->defaultLocation.terrain[0].textureIndex;
      memcpy(&map->defaultLocation, map->locationToDescriptionArray[0], sizeof(CompiledAtlasClientLocationDescription));
      map->defaultLocation.terrain[0].textureIndex = d;
      map->defaultLocation.terrain[1].terrainMeshIndex = 0xFFFFFFFF;

      // Get the location data here
      CompiledAtlasClientLocationDescription* locationDescription =
          isDefaultChunk ?
            &map->defaultLocation :
             map->locationToDescriptionArray[mapY * map->header.width + mapX];

      // If this location data is invalid, read from the file and assign the pointer
             if (!locationDescription) {
        DEBUG_WARNING("Location %i, %i not in memory!", mapX, mapY);
        continue;
      }

      // Create geometry for each terrain layer
      for (int layer = 0; layer < 2; ++layer) {
        CompiledAtlasClientLocationDescription::TerrainData* terrainData =
            &locationDescription->terrain[layer];

        // Make sure this layer is enabled
        if (terrainData->terrainMeshIndex >= map->terrainMeshArraySize) continue;

        // Build the transformation matrix for this location
        D3DXMATRIX mat;
        D3DXMatrixIdentity(&mat);
        mat._22 = terrainData->height;
        mat._41 = mapX * 1.0f;
        mat._42 = terrainData->elevation;
        mat._43 = mapY * 1.0f;

        // Add the first set of geometry
        Geometry* geometry;
        GeometryDuplicateTransform(&map->terrainMeshArray[terrainData->terrainMeshIndex],
                                   &mat,
                                   &geometry);
        geometryCollection.insert(
            SubsetGeometryCollection::value_type(terrainData->textureIndex,
                                                 geometry));

        // Add the wall geometry, if it exists
        if (terrainData->wallTextureIndex <
              global_rendering_manager_->numberOfTextures()) {
          GeometryDuplicateTransform(&map->terrainMeshArray[TERRAINMESHSUBSET_WALL_SIDES],
                                     &mat,
                                     &geometry);
          geometryCollection.insert(
              SubsetGeometryCollection::value_type(terrainData->wallTextureIndex,
                                                   geometry));
        }
      }

      // Create the geometry for the scenery
      /// TODO: get rid of using indices and just link to the pointer on startup!
      size_t sceneryIndex = locationDescription->sceneryIndex;
      if (sceneryIndex < game_file_data_->scenery.sceneryArraySize) {
        GameFileData_Mesh* mesh = game_file_data_->scenery.sceneryArray[sceneryIndex].mesh;
        GameFileData_Skin* skin = game_file_data_->scenery.sceneryArray[sceneryIndex].skin;

        if (mesh && skin) {
          for (GameFileData_Mesh::const_iterator i = mesh->begin(); i != mesh->end(); ++i) {
            // Look up this subset's index in the skin to find a texture for it
            GameFileData_Skin::iterator mappedTexture = skin->find(i->first);
            if (APP_WARNING(mappedTexture == skin->end())("Subset of mesh for scenery %lu has no texture", sceneryIndex)) continue;  // The mapping is invalid

            // Build the transformation matrix for this location; put the
            // scenery on the bottom terrain layer (layer 0)
            D3DXMATRIX mat;
            D3DXMatrixIdentity(&mat);
            mat._41 = mapX * 1.0f;
            mat._42 = locationDescription->terrain[0].elevation;
            mat._43 = mapY * 1.0f;

            // Multiply in the scenery's transformation
            D3DXMatrixMultiply(&mat,
                               &game_file_data_->scenery.sceneryArray[sceneryIndex].transform,
                               &mat);

            // Copy and transform the geometry from the mesh
            Geometry* geometry;
            GeometryDuplicateTransform(i->second, &mat, &geometry); // TODO: catch errors

            // Add to the geometry collection
            geometryCollection.insert(SubsetGeometryCollection::value_type(mappedTexture->second, geometry));
          }
        }
      }
    }
  }

  // Now we have a collection of geometry for this chunk; reduce it to a single set
  SubsetGeometry mapChunkGeometry;
  if (APP_ERROR(FAILED(CollectGeometry(&geometryCollection,
                                       &mapChunkGeometry)))("Couldn't compact map chunk's geometry")) {
    DeallocateGeometry(&geometryCollection);
    return false;
  }

  // Free the geometry collection, since it's been compacted into a single subset geometry
  DeallocateGeometry(&geometryCollection);

  // Allocate the rendering batches
  chunk->batchArraySize = mapChunkGeometry.size();
  chunk->batchArray = new MapChunkBatchRenderingParameters[mapChunkGeometry.size()];

  // Find out how much geometry we need to create
  DWORD totalVertices, totalIndices;
  GetGeometryTotals(&mapChunkGeometry,
                    &totalVertices,
                    &totalIndices);

  // Allocate the vertex buffer
  LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer;
  if (APP_ERROR(FAILED(d3d_device_->CreateVertexBuffer(
              sizeof(GeometryVertex)*totalVertices, D3DUSAGE_WRITEONLY/*|(myUsingSoftwareDeviceFlag ? D3DUSAGE_SOFTWAREPROCESSING : 0)*/,
              D3DFVF_GEOMETRYVERTEX, D3DPOOL_DEFAULT, &d3dVertexBuffer, NULL)))
      ("Couldn't allocate vertex buffer for chunk at %i, %i", chunk_x, chunk_y))
  {
    DeallocateGeometry(&mapChunkGeometry);
    return false;
  }

  // Allocate the index buffer
  LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer;
  if (APP_ERROR(FAILED(d3d_device_->CreateIndexBuffer(
              sizeof(GeometryIndex)*totalIndices, D3DUSAGE_WRITEONLY/*|(myUsingSoftwareDeviceFlag ? D3DUSAGE_SOFTWAREPROCESSING : 0)*/,
              D3DFMT_GEOMETRYINDEX, D3DPOOL_DEFAULT, &d3dIndexBuffer, NULL)))
      ("Couldn't allocate index buffer for chunk at %i, %i", chunk_x, chunk_y))
  {
    SAFE_RELEASE(d3dVertexBuffer);
    DeallocateGeometry(&mapChunkGeometry);
    return false;
  }

//  DEBUG_INFO("Chunk buffers allocated:  %lu vertices/%lu idices", totalVertices, totalIndices);

  // Lock the buffers
  GeometryVertex* vertices;
  GeometryIndex* indices;
  if (APP_ERROR(FAILED(d3dVertexBuffer->Lock(0, 0, (VOID**)&vertices, 0)) ||
                FAILED(d3dIndexBuffer->Lock(0, 0, (VOID**)&indices, 0)))
     ("Failed to lock chunk buffers"))
  {
    SAFE_RELEASE(d3dVertexBuffer);
    SAFE_RELEASE(d3dIndexBuffer);
    DeallocateGeometry(&mapChunkGeometry);
    return false;
  }

  // Copy data into the batches and buffers
  size_t index = 0;
  DWORD currentVertex = 0, currentIndex = 0;
  for (SubsetGeometry::iterator i = mapChunkGeometry.begin();
        i != mapChunkGeometry.end();
        ++i, ++index) {
    // Get the batch for this element
    MapChunkBatchRenderingParameters* batch = &chunk->batchArray[index];

    // Get rendering information
    DWORD numVertices = i->second->NumVertices;
    DWORD numIndices = i->second->NumIndices;

    // Copy the texture into the batch
    batch->texture_index = i->first;

    // Copy rendering data
    batch->minimumVertexIndex = currentVertex;
    batch->numberOfVertices = numVertices;
    batch->startIndex = currentIndex;
    batch->triangles = numIndices / 3;

    // Copy vertices
    memcpy(&vertices[currentVertex], i->second->pVertices, sizeof(GeometryVertex) * numVertices);

    // Copy indices and offset them to point to the correct location in the vertex buffer
    for (DWORD index = 0; index < numIndices; ++index)
      indices[index + currentIndex] = i->second->pIndices[index] + currentVertex;

    // Advance the current vertex/index amounts
    currentVertex += numVertices;
    currentIndex += numIndices;
  }

  // Free the geometry
  DeallocateGeometry(&mapChunkGeometry);

  // Unlock the buffers
  d3dVertexBuffer->Unlock();
  d3dIndexBuffer->Unlock();

  // Copy the buffers
  chunk->myVB = d3dVertexBuffer;
  chunk->myIB = d3dIndexBuffer;

  // Success
  return true;
}


void GlobalMapManager::deallocateMapChunkData(MapChunk* chunk) {
  // Free the terrain information
  SAFE_DELETE_ARRAY(chunk->batchArray);

  SAFE_RELEASE(chunk->myIB);
  SAFE_RELEASE(chunk->myVB);

  ZeroMemory(chunk, sizeof(MapChunk));
}








GlobalMapManager::PerMapData::PerMapData() {
  memset(&header, 0, sizeof(header));
  memset(&defaultLocation, 0, sizeof(defaultLocation));
  chunksWide = 0;
  chunksHigh = 0;
  terrainMeshArray = NULL;
  terrainMeshArraySize = 0;
  descriptionTableArray = NULL;
  locationToDescriptionArray = NULL;
  locationToDescriptionArraySize = 0;
  nontableDescriptionArray = NULL;
  nontableDescriptionArraySize = 0;
}


bool GlobalMapManager::PerMapData::loadFromGameFile(dc::dcStreamIn* game_file,
                                                    LPDIRECT3DDEVICE9 d3d_device) {

  Cleanup cleanup;

  //=======================================================================================
  // Atlas Header Information
  //=======================================================================================
  CompiledAtlasClientHeader mapHeader;
  if (APP_ERROR(!game_file->read(&mapHeader, sizeof(mapHeader)))("couldn't read the map header"))
    return false;

  // Make sure the map header is valid
  if (APP_ERROR((mapHeader.mapBytesPerPixel != COMPILEDATLAS_MAP_BYTES_PER_PIXEL) ||
         (mapHeader.mapImageFormat != COMPILEDATLAS_MAP_FORMAT))
      ("the map is an invalid format")) return false;

  // Calculate the chunk size of the map and copy sizes
  chunksWide = mapHeader.width  / COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS;
  chunksHigh = mapHeader.height / COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS;

  //=======================================================================================
  // Terrain Meshes
  //=======================================================================================

  // Make sure we're at the right location
  //if (APP_ERROR(ftell(game_file) != mapHeader.fileLocations.terrainMeshes)("Terrain meshes in unexpected location"))
  if (APP_ERROR(game_file->tell() != mapHeader.fileLocations.terrainMeshes)("Terrain meshes in unexpected location"))
    return false;

  //fread(&terrainMeshArraySize, sizeof(terrainMeshArraySize), 1, game_file);
  CONFIRM(game_file->read(&terrainMeshArraySize, sizeof(terrainMeshArraySize))) else return false;
  terrainMeshArray = new Geometry[terrainMeshArraySize];

  // Make sure the array was allocated
  if (APP_ERROR(!terrainMeshArray)("Couldn't allocate terrain mesh geometry array"))
    return false;

  // Add to the cleanup variable
  cleanup.addArray((void**)&terrainMeshArray);

  // Read each entry in the terrain mesh
  for (size_t i = 0; i < terrainMeshArraySize; ++i)
  {
    // Read the subset index (this should be the same as 'i' because we want this
    // array to be direct-mapped)
    SubsetIndex subsetIndex;
    DWORD numVertices, numIndices;
    CONFIRM(game_file->read(&subsetIndex, sizeof(subsetIndex))) else return false;
    CONFIRM(game_file->read(&numVertices, sizeof(numVertices))) else return false;
    CONFIRM(game_file->read(&numIndices, sizeof(numIndices))) else return false;
    //if (APP_ERROR(!fread(&subsetIndex, sizeof(subsetIndex), 1, game_file) ||
    //              !fread(&numVertices, sizeof(numVertices), 1, game_file) ||
    //              !fread(&numIndices, sizeof(numIndices), 1, game_file))("Failed reading terrain mesh subset %lu", i))
    //  return false;

    // Make sure the index is valid
    if (APP_ERROR(subsetIndex != i)("Subset %lu of terrain meshes should have index %lu; there was a compilation error", subsetIndex, i))
      return false;

    // The geometry being filled
    Geometry* geometry = &terrainMeshArray[i];


    // Read the vertices
    geometry->NumVertices = numVertices;
    if (numVertices > 0)
    {
      // Allocate and read the array of vertices
      if (APP_FATAL(!(geometry->pVertices = new GeometryVertex[numVertices]))("Out of memory"))
         return false;
      cleanup.addArray((void**)&geometry->pVertices);
      //if (APP_ERROR(!fread(geometry->pVertices, sizeof(GeometryVertex), numVertices, game_file))("Couldn't read vertices for terrain mesh %lu", i))
      //   return false;
      CONFIRM(game_file->read(geometry->pVertices, sizeof(GeometryVertex) * numVertices)) else return false;
    }
    else
      DEBUG_WARNING("Terrain mesh %lu has no vertices", i);

    // Read the indices
    geometry->NumIndices = numIndices;
    if (numIndices > 0)
    {
      // Allocate and read the array of indices
      if (APP_FATAL(!(geometry->pIndices = new GeometryIndex[numIndices]))("Out of memory"))
        return false;
     cleanup.addArray((void**)&geometry->pIndices);
     //if (APP_ERROR(!fread(geometry->pIndices, sizeof(GeometryIndex), numIndices, game_file))("Couldn't read indices for terrain mesh %lu", i))
     //  return false;
     CONFIRM(game_file->read(geometry->pIndices, sizeof(GeometryIndex) * numIndices)) else return false;
    }
    else
      DEBUG_WARNING("Terrain mesh %lu has no vertices", i);

  }

  //=======================================================================================
  // Description Table
  //=======================================================================================

  // Make sure we're at the right location
  //if (APP_ERROR(ftell(game_file) != mapHeader.fileLocations.descriptionTable)("Description table in unexpected location"))
  if (APP_ERROR(game_file->tell() != mapHeader.fileLocations.descriptionTable)("Description table in unexpected location"))
    return false;

  // Allocate the table
  descriptionTableArray = new CompiledAtlasClientLocationDescription[COMPILEDATLAS_MAP_LOOKUPTABLE_SIZE];
  if (APP_FATAL(!descriptionTableArray)("Out of memory"))
    return false;
  cleanup.addArray((void**)&descriptionTableArray);

  // Read the table
  //if (APP_ERROR(!fread(descriptionTableArray,
  //  sizeof(CompiledAtlasClientLocationDescription), COMPILEDATLAS_MAP_LOOKUPTABLE_SIZE, game_file))
  //  ("Unable to read compiled atlas client description array")) return false;
  CONFIRM(game_file->read(descriptionTableArray,
                          sizeof(CompiledAtlasClientLocationDescription)
                            * COMPILEDATLAS_MAP_LOOKUPTABLE_SIZE)) else return false;


  //=======================================================================================
  // Map
  //=======================================================================================

  // Make sure we're at the right location
  //if (APP_ERROR(ftell(game_file) != mapHeader.fileLocations.map)("Map in unexpected location"))
  if (APP_ERROR((game_file->tell()) != mapHeader.fileLocations.map)("Map in unexpected location"))
    return false;

  size_t mapImageDataSize = 0;
  BYTE* mapImageData = 0;
  //fread(&mapImageDataSize, sizeof(mapImageDataSize), 1, game_file);
  CONFIRM(game_file->read(&mapImageDataSize, sizeof(mapImageDataSize))) else return false;
  mapImageData = new BYTE[mapImageDataSize];
  if (APP_ERROR(!mapImageData)("Couldn't load map image")) return false;

  // Load the data for this image from the file
  //if (APP_ERROR(!fread(mapImageData, sizeof(BYTE), mapImageDataSize, game_file))("Failed reading map image"))
  //  return false;
  CONFIRM(game_file->read(mapImageData, sizeof(BYTE) * mapImageDataSize)) else return false;

  // Load the texture from this image
  LPDIRECT3DTEXTURE9 mapImage = 0;
  D3DXIMAGE_INFO mapImageInfo;
  HRESULT hr;
  hr = D3DXCreateTextureFromFileInMemoryEx(d3d_device, mapImageData, mapImageDataSize, 0, 0, 1,
                      0, COMPILEDATLAS_MAP_FORMAT, D3DPOOL_SCRATCH,
                      D3DX_FILTER_POINT, D3DX_FILTER_POINT, 0, &mapImageInfo, 0, &mapImage);

  // Get rid of image memory
  SAFE_DELETE_ARRAY(mapImageData);

  // Make sure the texture was created correctly
  if (APP_ERROR(FAILED(hr))("Failed translating map in memory"))
    return false;

  // Create a new cleanup variable for the texture
  Cleanup cleanupMapImage;
  cleanupMapImage.addInterface((IUnknown**)&mapImage);

  // Make sure the map data and texture agree
  if (APP_ERROR(mapHeader.width  != mapImageInfo.Width || 
                mapHeader.height != mapImageInfo.Height)
    ("Compiling logic error: assigned size and scanned sizes are different"))
    return false;

  // Load data from this texture's raw pixels into memory, and delete the
  // texture so we don't have to constantly lock/unlock while reading
  locationToDescriptionArraySize = mapHeader.width * mapHeader.height;
  locationToDescriptionArray =
    new CompiledAtlasClientLocationDescription*[locationToDescriptionArraySize];

  // Check allocation
  if (APP_ERROR(!locationToDescriptionArray)("Couldn't allocate locationToDescriptionArray"))
    return false;

  // Lock the buffer
  D3DLOCKED_RECT mapImageLockedRect;
  if (APP_ERROR(FAILED(mapImage->LockRect(0, &mapImageLockedRect, NULL, 0)))
      ("Unable to lock the texture of the main map during compilation"))
    return false;

  // Grab data from the texture
  COMPILEDATLAS_MAP_PIXEL_DATATYPE* mapImageTextureData =
    (COMPILEDATLAS_MAP_PIXEL_DATATYPE*)mapImageLockedRect.pBits;
  mapImageLockedRect.Pitch /= COMPILEDATLAS_MAP_BYTES_PER_PIXEL;
  for (size_t y = 0; y < mapHeader.height; ++y)
  {
    for (size_t x = 0; x < mapHeader.width; ++x)
    {
      // Get the referenced value for this location
      COMPILEDATLAS_MAP_PIXEL_DATATYPE reference =
        mapImageTextureData[y*mapImageLockedRect.Pitch+x];

      // Assign the corresponding location in the location description array
      locationToDescriptionArray[y*mapHeader.width + x] =
        reference == COMPILEDATLAS_MAP_INDEX_NOTINTABLE ? 0 : &descriptionTableArray[reference];
    }
  }

  // Unlock and get rid of the terrain texture
  mapImage->UnlockRect(0);
  SAFE_RELEASE(mapImage);
  cleanupMapImage.clear();

  // Save the array size
  locationToDescriptionArraySize = locationToDescriptionArraySize;

  //=======================================================================================
  // Chunk to Nontable Description Index Table
  //=======================================================================================

  // Make sure we're at the right location
  //if (APP_ERROR(ftell(game_file) != mapHeader.fileLocations.chunkToNontableDescriptionIndicesTable)("Chunk table in unexpected location"))
  if (APP_ERROR(game_file->tell() != mapHeader.fileLocations.chunkToNontableDescriptionIndicesTable)("Chunk table in unexpected location"))
    return false;

  size_t chunkToNontableDescriptionIndexArraySize;
  //fread(&chunkToNontableDescriptionIndexArraySize,
  //  sizeof(chunkToNontableDescriptionIndexArraySize), 1, game_file);
  CONFIRM(game_file->read(&chunkToNontableDescriptionIndexArraySize, sizeof(chunkToNontableDescriptionIndexArraySize))) else return false;
  if (chunkToNontableDescriptionIndexArraySize >
    (1 + mapHeader.width  / COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS) *
    (1 + mapHeader.height / COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS)) {
  return false;
  }
  size_t* chunkToNontableDescriptionIndexArray = NULL;

  if (chunkToNontableDescriptionIndexArraySize > 0) {
    chunkToNontableDescriptionIndexArray = new size_t[chunkToNontableDescriptionIndexArraySize];

    // Make sure the allocation succeeded
    if (APP_ERROR(!chunkToNontableDescriptionIndexArray)("Unable to read/allocate chunk to nontable description array")) return false;

    // Add to the cleanup array, so that if the function returns prematurely
    // it will get deallocated
    cleanup.addArray((void**)&chunkToNontableDescriptionIndexArray);

    // Make sure this array is the right size
    if (APP_ERROR(chunksWide * chunksHigh != chunkToNontableDescriptionIndexArraySize)
        ("chunk to nontable description array was the wrong size")) return false;

    // Load the array of chunk to nontable description indices
    //if (APP_ERROR(!fread(chunkToNontableDescriptionIndexArray,
    //           sizeof(size_t), chunkToNontableDescriptionIndexArraySize, game_file))("Couldn't read chunk to nontable description index array"))
    // return false;
    CONFIRM(game_file->read(chunkToNontableDescriptionIndexArray,
                            sizeof(size_t) * chunkToNontableDescriptionIndexArraySize)) else return false;
  }

  //=======================================================================================
  // Nontable Descriptions
  // TODO: this could be accomplished much more efficiently if I'm certain the whole set of
  // nontable descriptions is going to be in memory--if so, we don't even NEED to have a
  // chunk to nontable description array; in fact, chunks can go away completely!
  //=======================================================================================

  // Make sure we're at the right location
  //if (APP_ERROR(ftell(game_file) != mapHeader.fileLocations.nontableDescriptions)("Nontable descriptions in unexpected location"))
  if (APP_ERROR(game_file->tell() != mapHeader.fileLocations.nontableDescriptions)("Nontable descriptions in unexpected location"))
    return false;


  // Allocate the array
  size_t nontableDescriptionIndexArraySize;
  CONFIRM(game_file->read(&nontableDescriptionIndexArraySize,
                          sizeof(nontableDescriptionIndexArraySize))) else return false;
  //fread(&nontableDescriptionIndexArraySize,
  //  sizeof(nontableDescriptionIndexArraySize), 1, game_file);
  if (nontableDescriptionIndexArraySize >= mapHeader.width * mapHeader.height)
    return false;
  nontableDescriptionArraySize = nontableDescriptionIndexArraySize;
  nontableDescriptionArray = new CompiledAtlasClientLocationDescription[nontableDescriptionIndexArraySize];

  // Load the table, if it exists
  if (nontableDescriptionIndexArraySize > 0)
  {
    // Make sure the allocation succeeded
    if (APP_ERROR(!nontableDescriptionArray)("Unable to read/allocate chunk to nontable description array")) return false;
    cleanup.addArray((void**)&nontableDescriptionArray);

    // Load the array of nontable descriptions
    //if (APP_ERROR(!fread(nontableDescriptionArray,
    //           sizeof(CompiledAtlasClientLocationDescription),
    //           nontableDescriptionIndexArraySize, game_file))("Couldn't read nontable description array"))
    // return false;
    if (APP_ERROR(game_file->read(&nontableDescriptionArray,
                                  sizeof(CompiledAtlasClientLocationDescription) * nontableDescriptionIndexArraySize
                                  ))("Couldn't read nontable description array")) return false;

    // Map entries in the table that are invalid
    for (int chunkY = 0; chunkY < chunksHigh; ++chunkY)
      for (int chunkX = 0; chunkX < chunksWide; ++chunkX)
      {
        // Get the index of the first nontable description here
        int nontableIndex = chunkToNontableDescriptionIndexArray[chunkY * chunksWide + chunkX];

        // Go through each location
        for (int y = 0; y < COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS; ++y)
          for (int x = 0; x < COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS; ++x)
          {
            int locationX = chunkX * COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS + x;
            int locationY = chunkY * COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS + y;
            CompiledAtlasClientLocationDescription** description = &locationToDescriptionArray[locationY * mapHeader.width + locationX];
            if ((*description) == 0)
            {
              // Make sure the index is less than the maximum
              if (!APP_ERROR(nontableIndex >= nontableDescriptionIndexArraySize)("Not enough nontable entries to fill map"))
                (*description) = &nontableDescriptionArray[nontableIndex];
              nontableIndex++;
            }
          }
      }
  }

  // Copy over the header information from the file
  memcpy(&header, &mapHeader, sizeof(mapHeader));

  // Initialize the default location
  defaultLocation.navigability = mapHeader.defaultNavigability;
  defaultLocation.sceneryIndex = 0xFFFFFFFF;
  defaultLocation.terrain[0].elevation = 0.0f;
  defaultLocation.terrain[0].height = 0.0f;
  defaultLocation.terrain[0].terrainMeshIndex = 0;
  defaultLocation.terrain[0].textureIndex = mapHeader.defaultTextureIndex;
  defaultLocation.terrain[0].wallTextureIndex = 0xFFFFFFFF;
  defaultLocation.terrain[1].textureIndex = 0xFFFFFFFF;
  defaultLocation.terrain[1].wallTextureIndex = 0xFFFFFFFF;

  // Erase the chunk index array
  SAFE_DELETE_ARRAY(chunkToNontableDescriptionIndexArray);

  cleanup.clear();

  // success
  return true;
}


void GlobalMapManager::PerMapData::destroy() {

  // Free atlas data
  SAFE_DELETE_ARRAY(nontableDescriptionArray);
  nontableDescriptionArraySize = 0;
  SAFE_DELETE_ARRAY(locationToDescriptionArray);
  locationToDescriptionArraySize = 0;
  SAFE_DELETE_ARRAY(descriptionTableArray);

  if (terrainMeshArraySize > 0 && terrainMeshArray)
  {
    // Free each element in the array
    for (size_t i = 0; i < terrainMeshArraySize; ++i)
    {
      SAFE_DELETE_ARRAY(terrainMeshArray[i].pVertices);
      SAFE_DELETE_ARRAY(terrainMeshArray[i].pIndices);
    }

    // Free the array
    SAFE_DELETE_ARRAY(terrainMeshArray);
  }
  terrainMeshArraySize = 0;
}



}