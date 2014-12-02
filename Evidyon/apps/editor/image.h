//------------------------------------------------------------------------------------------------
// File:    image.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __IMAGE_H__
#define __IMAGE_H__


#define D3DTEX_MANAGED      0x01 /* create in managed pool */
#define D3DTEX_MIPMAP       0x02 /* generate a mipmap chain */
#define D3DTEX_SYSTEMMEM    0x04 /* create in system memory; overrides D3DTEX_MANAGED */

#include <d3d9.h>

bool SegmentImage(LPDIRECT3DTEXTURE9 texture,
                  int subimage_width,
                  int subimage_height,
                  int rows,
                  int columns,
                  int selected_index,
                  LPDIRECT3DTEXTURE9* segment);

/**
* An image is an abstract representation of something that can produce an IDirect3DTexture9
* object filled with color information.  Right now, only D3DXImage, which loads various image
* formats from the disk, exists.  In the future, derived classes could create images using
* formulas or create new images using mixing techniques.
*/
class Image : public dc::dcGenericResource
{
public:

  /**
   * Contains raw information needed to display a preview of this resource
   */
  struct Preview
  {
      /// The resource that created this preview
      Image* image;

      /// The texture for the image
      LPDIRECT3DTEXTURE9 d3dTexture;
  };

public:

  /**
   * Initializes this resource
   */
  Image();

  /**
   * Generates a preview for this image
   * @param pd3dDevice Device onto which to load the D3D texture
   * @param preview Destination for the preview data
   * @return Whether or not the preview could be loaded
   */
  bool loadPreview(LPDIRECT3DDEVICE9 pd3dDevice, Preview* preview);


public:

  /**
   * Loads the image denoted by this class using the specified device
   * @param pd3dDevice Device onto which to load the texture
   * @param options Combination of one or more D3DTEX_* flags detailing the texture format
   * @param ppd3dTexture Return location for texture interface
   * @return Result code
   */
  virtual bool obtainD3DTexture(LPDIRECT3DDEVICE9 pd3dDevice, DWORD options, LPDIRECT3DTEXTURE9* ppd3dTexture) const = 0;


public:

  /**
   * Returns the type name of this class
   */
  static std::string staticTypeName();
};




#endif