//------------------------------------------------------------------------------------------------
// File:  sceneryrenderer.cpp
//
// Desc:  Class used to collect, transform and render scenery efficiently
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"



//------------------------------------------------------------------------------------------------
// Name: SceneryRenderer
// Desc: Sets up the internal components of the renderer
//------------------------------------------------------------------------------------------------
SceneryRenderer::SceneryRenderer()
{
  zero();
}



//------------------------------------------------------------------------------------------------
// Name: create
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
bool SceneryRenderer::create(LPDIRECT3DDEVICE9 d3dDevice, bool softwareDevice)
{
  // Validate parameters
  CONFIRM(d3dDevice) else return false;

  // Make sure the class is clean
  destroy();

  // Create a vertex buffer
  if (FAILED(d3dDevice->CreateVertexBuffer(
          sizeof(GeometryVertex) * SCENERY_RENDERER_VERTEX_CAPACITY,
          D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|(softwareDevice ? D3DUSAGE_SOFTWAREPROCESSING : 0),
          D3DFVF_GEOMETRYVERTEX,
          D3DPOOL_DEFAULT,
          &myVB, 0)))
  {
    return false;
  }

  // Create an index buffer
  if (FAILED(d3dDevice->CreateIndexBuffer(
          sizeof(GeometryIndex) * SCENERY_RENDERER_INDEX_CAPACITY,
          D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|(softwareDevice ? D3DUSAGE_SOFTWAREPROCESSING : 0),
          D3DFMT_GEOMETRYINDEX,
          D3DPOOL_DEFAULT,
          &myIB, 0)))
  {
    // Clear out the VB
    myVB->Release();

    // Failed
    return false;
  }

  // Store the device pointer internally
  (myD3DDevice = d3dDevice)->AddRef();

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Frees memory used by this class
//------------------------------------------------------------------------------------------------
void SceneryRenderer::destroy()
{
  SAFE_RELEASE(myD3DDevice);
  SAFE_RELEASE(myVB);
  SAFE_RELEASE(myIB);
  zero();
}



void SceneryRenderer::setSceneryArray(const GameFileData_Scenery* scenery_array,
                                      size_t scenery_array_size) {
  scenery_array_ = scenery_array;
  scenery_array_size_ = scenery_array_size;
}


bool SceneryRenderer::addScenery(const D3DXMATRIX* worldTransform,
                                 size_t scenery_index,
                                 HSCENERY* sceneryHandle) {
  if (scenery_array_size_ <= scenery_index) {
    memset(sceneryHandle, 0, sizeof(HSCENERY));
    return false;
  }
  return addScenery(worldTransform, &scenery_array_[scenery_index], sceneryHandle);
}

//------------------------------------------------------------------------------------------------
// Name: addScenery
// Desc: Adds a new scenery object to the renderer with the given transform
//------------------------------------------------------------------------------------------------
bool SceneryRenderer::addScenery(const D3DXMATRIX* worldTransform, const GameFileData_Scenery* scenery, HSCENERY* sceneryHandle)
{
  CONFIRM(scenery) else return false;
  GameFileData_Mesh* mesh = scenery->mesh;
  GameFileData_Skin* skin = scenery->skin;
  CONFIRM(mesh && skin) else return false;


  RenderableScenery* renderableScenery = myRenderableSceneryPool.acquire();
  CONFIRM(renderableScenery) else return false;
  renderableScenery->hash = rand() % 0xFFFF;
  renderableScenery->scenery = scenery;
  HSCENERY handle = { myRenderableSceneryPool.indexOf(renderableScenery), renderableScenery->hash };
  updateSceneryTransform(handle, worldTransform);
  *sceneryHandle = handle;

  // Allocate each subset
  for (GameFileData_Skin::iterator skinEntry = skin->begin(); skinEntry != skin->end(); ++skinEntry)
  {
    GameFileData_Mesh::iterator mesh_iter = mesh->find(skinEntry->first);
    Geometry* geometry = mesh_iter == mesh->end() ? 0 : mesh_iter->second;
    if (geometry)
    {
      SceneryTextureSubsetGeometry* subset = myScenerySubsetPool.acquire();
      CONFIRM(subset) else break;

      // Add this subset to the table
      subset->sourceScenery = handle;
      subset->renderableSceneryTransform = &renderableScenery->worldAndSceneryTransform;
      subset->geometryToRender = geometry;
      myScenerySubsetBatchTable.insert(ScenerySubsetBatchTable::value_type(skinEntry->second, subset));
    }
  }

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name: updateSceneryTransform
// Desc: Updates the world transformation matrix for a piece of scenery that has been added to the renderer
//------------------------------------------------------------------------------------------------
void SceneryRenderer::updateSceneryTransform(HSCENERY sceneryHandle, const D3DXMATRIX* worldTransform)
{
  // Get the scenery element
  RenderableScenery* scenery = myRenderableSceneryPool.get(sceneryHandle.index);
  if (!scenery || scenery->hash != sceneryHandle.hash || !scenery->scenery) return;

  // Update the transformation
  if (worldTransform)
    D3DXMatrixMultiply(&scenery->worldAndSceneryTransform, &scenery->scenery->transform, worldTransform);
  else
    scenery->worldAndSceneryTransform = scenery->scenery->transform;
}



//------------------------------------------------------------------------------------------------
// Name: removeScenery
// Desc: Erases the scenery with the given handle
//------------------------------------------------------------------------------------------------
void SceneryRenderer::removeScenery(HSCENERY sceneryHandle)
{
  // Get the scenery element
  RenderableScenery* scenery = myRenderableSceneryPool.get(sceneryHandle.index);
  if (!scenery || scenery->hash != sceneryHandle.hash) return;

  // Regenerate the hash so that this doesn't get reused
  scenery->hash = rand()%0xFFFF;

  // Erase this element
  myRenderableSceneryPool.release(scenery);

  // Free all of the elements in the batch table
  for (ScenerySubsetBatchTable::iterator i = myScenerySubsetBatchTable.begin(); i != myScenerySubsetBatchTable.end();)
  {
    if (i->second->sourceScenery.index == sceneryHandle.index)
    {
      // Free memory for this element (multiple calls to this method will be ineffectual)
      myScenerySubsetPool.release(i->second);

      // Erase from the table
      ScenerySubsetBatchTable::iterator next = i; ++next;
      myScenerySubsetBatchTable.erase(i);
      i = next;
    }
    else
      ++i;
  }
}



//------------------------------------------------------------------------------------------------
// Name: renderTextureSubset
// Desc: Draws the scenery geometry associated with the given texture subset.  This method assumes
//     that the subset has been correctly initialized in the target device
//------------------------------------------------------------------------------------------------
bool SceneryRenderer::renderTextureSubset(IsometricCamera* camera,
                      size_t textureSubset,
                      Evidyon::GlobalRenderingManager* rendering_manager)
{
  size_t consumedVertices = 0, consumedIndices = 0;
  GeometryVertex* lockedVertices = 0;
  GeometryIndex* lockedIndices = 0;

  bool initializedBuffers = false;

  // Render all of the elements in this subset
  ScenerySubsetBatchTable::iterator end = myScenerySubsetBatchTable.upper_bound(textureSubset);
  ScenerySubsetBatchTable::iterator next;
  for (ScenerySubsetBatchTable::iterator i = myScenerySubsetBatchTable.lower_bound(textureSubset);
     i != end; i = next)
  {
    // Assign the next iterator
    next = i; ++next;

    // Get the geometry at this index
    const Geometry* geometry = i->second->geometryToRender;
    size_t numVertices = geometry->NumVertices;
    size_t numIndices = geometry->NumIndices;

    ASSERT(numVertices + consumedVertices < SCENERY_RENDERER_VERTEX_CAPACITY) else return false;
    ASSERT(numVertices + consumedIndices < SCENERY_RENDERER_INDEX_CAPACITY) else return false;

    // Lock the buffers
    myVB->Lock(consumedVertices * sizeof(GeometryVertex), numVertices * sizeof(GeometryVertex), (void**)&lockedVertices, consumedVertices ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
    myIB->Lock(consumedIndices * sizeof(GeometryIndex),   numIndices * sizeof(GeometryIndex), (void**)&lockedIndices, consumedIndices ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

    if (lockedVertices && lockedIndices)
    {
      // Add the geometry to the internal buffers
      memcpy(lockedVertices, geometry->pVertices, numVertices * sizeof(GeometryVertex));
      memcpy(lockedIndices, geometry->pIndices, numIndices * sizeof(GeometryIndex));

      // Offset each of the indices
      for (size_t index = 0; index < numIndices; ++index)
        lockedIndices[index] += consumedVertices;
    }


    // Free the buffers
    myVB->Unlock();
    myIB->Unlock();

    // Initialize them in the rendering device
    if (!initializedBuffers)
    {
      rendering_manager->changeCurrentTexture(textureSubset);
      myD3DDevice->SetStreamSource(0, myVB, 0, sizeof(GeometryVertex));
      myD3DDevice->SetFVF(D3DFVF_GEOMETRYVERTEX);
      myD3DDevice->SetIndices(myIB);
      initializedBuffers = true;
    }

    // Draw the geometry
    camera->setWorldMatrix(myD3DDevice, i->second->renderableSceneryTransform);
    myD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, consumedVertices, numVertices, consumedIndices, numIndices / 3);

    // Add to the consumed geometry data
    consumedVertices += numVertices;
    consumedIndices += numIndices;

    // Flush the buffer if we are out of room, or if we won't be able to fit
    // the next geometry element
    const Geometry* nextGeometry = next == end ? 0 : next->second->geometryToRender;
    if (!nextGeometry || (consumedVertices + nextGeometry->NumVertices >= SCENERY_RENDERER_VERTEX_CAPACITY) ||
               (consumedIndices + nextGeometry->NumIndices >= SCENERY_RENDERER_INDEX_CAPACITY))
    {
      consumedVertices = 0;
      consumedIndices = 0;
    }
  }

  // Sucess
  return true;
}




//------------------------------------------------------------------------------------------------
// Name: clear
// Desc: Erases all of the scenery in this renderer
//------------------------------------------------------------------------------------------------
void SceneryRenderer::clear()
{
  for (RenderableSceneryPool::Iterator i(&myRenderableSceneryPool); i.hasNext(); i.advance())
  {
    i.get()->hash = rand() % 0xFFFF;
    myRenderableSceneryPool.release(i.get());
  }
  for (ScenerySubsetPool::Iterator i(&myScenerySubsetPool); i.hasNext(); i.advance())
    myScenerySubsetPool.release(i.get());
  myScenerySubsetBatchTable.clear();
}



//------------------------------------------------------------------------------------------------
// Name: zero
// Desc: Resets all of the members of this class
//------------------------------------------------------------------------------------------------
void SceneryRenderer::zero()
{
  myD3DDevice = 0;
  myVB = 0;
  myIB = 0;
  myDeviceHasBeenInitializedFlag = 0;
  myRenderableSceneryPool.destroy();
  myScenerySubsetPool.destroy();
  myScenerySubsetBatchTable.clear();
}

