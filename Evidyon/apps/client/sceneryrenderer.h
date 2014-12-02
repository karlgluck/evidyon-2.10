//------------------------------------------------------------------------------------------------
// File:  sceneryrenderer.h
//
// Desc:  Class used to collect, transform and render scenery efficiently
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __SCENERYRENDERER_H__
#define __SCENERYRENDERER_H__


#include "hscenery.h"
#include <map>
#include <dcx/memorypool>

// Must be pre-declared
class VolucrisClient;

#define SCENERY_RENDERER_MAX_SCENERY    1000
#define SCENERY_RENDERER_MAX_SUBSETS    (10*(SCENERY_RENDERER_MAX_SCENERY))
#define SCENERY_RENDERER_VERTEX_CAPACITY  2000
#define SCENERY_RENDERER_TRIANGLE_CAPACITY  3000
#define SCENERY_RENDERER_INDEX_CAPACITY   (3*(SCENERY_RENDERER_TRIANGLE_CAPACITY))

struct Geometry;
struct D3DXMATRIX;
struct GameFileData_Scenery;

namespace Evidyon {
  class GlobalRenderingManager;
}

/** 
 * Renders geometry
 */
class SceneryRenderer
{
  /**
   * Holds information about scenery that has been added to the renderer
   */
  struct RenderableScenery
  {
    /// The combined scenery-local transform and the requested source transform.  This
    /// matrix is referenced by each subset texture
    D3DXMATRIXA16 worldAndSceneryTransform;

    /// The hash value for this scenery object.  Used to prevent handles from indexing
    /// incorrect scenery elements.
    unsigned short hash;

    /// The source scenery that this structure represents
    const GameFileData_Scenery* scenery;
  };


  /**
   * This structure holds the geometry for a given scenery object to actually render
   * for a given texture subset.
   */
  struct SceneryTextureSubsetGeometry
  {
    /// The source scenery handle for this geometry.  Used to find and erase
    /// this element when the source scenery is destroyed
    HSCENERY sourceScenery;

    /// Reference to the owner renderable scenery's transform for this geometry
    const D3DXMATRIXA16* renderableSceneryTransform;

    /// The geometry (loaded from the game file) to render
    const Geometry* geometryToRender;
  };

  /// This is the type used to reference subset geometry by texture index
  typedef std::multimap<size_t,SceneryTextureSubsetGeometry*> ScenerySubsetBatchTable;

  public:

    /**
     * Sets up the internal components of the renderer
     */
    SceneryRenderer();

    /**
     * Initializes this class
     */
    bool create(LPDIRECT3DDEVICE9 d3dDevice,
                bool softwareDevice);

    /**
     * Frees memory used by this class
     */
    void destroy();

    void setSceneryArray(const GameFileData_Scenery* scenery_array,
                         size_t scenery_array_size);

    /**
     * Adds a new scenery object to the renderer with the given transform
     */
    bool addScenery(const D3DXMATRIX* worldTransform,
                    const GameFileData_Scenery* scenery,
                    HSCENERY* sceneryHandle);
    bool addScenery(const D3DXMATRIX* worldTransform,
                    size_t scenery_index,
                    HSCENERY* sceneryHandle);

    /**
     * Updates the world transformation matrix for a piece of scenery that has been added to the renderer
     */
    void updateSceneryTransform(HSCENERY sceneryHandle, const D3DXMATRIX* worldTransform);

    /**
     * Erases the scenery with the given handle
     */
    void removeScenery(HSCENERY sceneryHandle);

    /**
     * Draws the scenery geometry associated with the given texture subset.  This method assumes
     * that the subset has been correctly initialized in the target device
     */
    bool renderTextureSubset(IsometricCamera* camera, size_t textureSubset, Evidyon::GlobalRenderingManager* rendering_manager);

    /**
     * Erases all of the scenery in this renderer
     */
    void clear();

  protected:

    /**
     * Resets all of the members of this class
     */
    void zero();


  protected:

    /// The device to use for rendering
    LPDIRECT3DDEVICE9 myD3DDevice;

    /// Dynamic vertex buffer for rendering geometry
    LPDIRECT3DVERTEXBUFFER9 myVB;

    /// Dynamic index buffer for rendering geometry
    LPDIRECT3DINDEXBUFFER9 myIB;

    /// When a subset is being rendered, this determines whether or not the buffers have been initialized in the device
    bool myDeviceHasBeenInitializedFlag;

    /// Holds all of the scenery entries
    typedef dcx::dcxMemoryPool<RenderableScenery,SCENERY_RENDERER_MAX_SCENERY> RenderableSceneryPool;
    RenderableSceneryPool myRenderableSceneryPool;

    /// Holds memory for the renderable subsets
    typedef dcx::dcxMemoryPool<SceneryTextureSubsetGeometry,SCENERY_RENDERER_MAX_SUBSETS> ScenerySubsetPool;
    ScenerySubsetPool myScenerySubsetPool;

    /// This table holds all of the batches so that they can be drawn
    ScenerySubsetBatchTable myScenerySubsetBatchTable;

    const GameFileData_Scenery* scenery_array_;
    size_t scenery_array_size_;
};


#endif
