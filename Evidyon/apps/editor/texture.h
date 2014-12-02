//------------------------------------------------------------------------------------------------
// File:    texture.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __TEXTURE_H__
#define __TEXTURE_H__


/**
* A texture references an image and gives it a few interesting modifications (motion, for ex.)
*/
class Texture : public dc::dcGenericResource
{
public:

  /**
   * Contains raw information needed to display a preview of this resource
   */
  struct Preview
  {
      /// The resource that created this preview
      Texture* texture;

      /// The image with which this texture is associated
      Image::Preview* imagePreview;

      /// Whether to enable alpha-testing
      bool enableAlphaTesting;

      /// Whether to enable alpha-blending
      bool enableAlphaBlending;
  };


public:

  /**
   * Initializes this class
   */
  Texture();

  /**
   * Sets up the preview for this texture.  This method will output an error if the image
   * that this texture is associated with could not be found or its index in the image preview
   * array didn't match its index in the image table.  However, these conditions do not cause
   * the method to fail; instead, the imagePreview member will be 'null'.
   * @param imagePreviewArray The array of image previews previously loaded.  This should be a
   *                          direct, in-order loading of the root image table.
   * @param imagePreviews How many images exist in the image preview array
   * @param preview The destination preview element
   * @return Whether or not the preview could be initialized
   */
  bool loadPreview(Image::Preview* imagePreviewArray, size_t imagePreviews, Preview* preview);

  /**
   * Accessor for this class's reference
   */
  dc::dcTable<Image>::Reference* getImageRef() const { return (dcTable<Image>::Reference*)&myImageRef; }

  // TODO: rename to getEnableAlphaTestingValue()
  bool getEnableAlphaTestingValue() const { return myEnableAlphaTestingFlag.getValue(); }
  bool getEnableAlphaBlendingValue() const { return myEnableAlphaBlendingFlag.getValue(); }

  dc::dcBoolean* getFlipHorizontal() { return &myFlipHorizontal; }
  dc::dcBoolean* getFlipVertical() { return &myFlipVertical; }

  bool getFlipHorizontalValue() const { return myFlipHorizontal.getValue(); }
  bool getFlipVerticalValue() const { return myFlipVertical.getValue(); }

  dc::dcBoolean* getEnableAlphaTestingFlag() { return &myEnableAlphaTestingFlag; }
  dc::dcBoolean* getEnableAlphaBlendingFlag() { return &myEnableAlphaBlendingFlag; }

  /**
   * Sorts textures into correct rendering order
   */
  int compareTo(const dc::dcGenericResource* other) const;


public:

  /**
   * Sets up the texture stage indicated on the provided device for rendering, such that if
   * the D3D texture from the referenced image is set up on that stage, then polygons will
   * render with this texture's configuration state.
   * The default implementation sets the alpha testing/blending flags
   */
  virtual HRESULT configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage) = 0;


protected:

  /**
   * This method applies the D3DRS_ALPHABLENDENABLE and D3DRS_ALPHATESTENABLE states to the device,
   * as well as updating the texture transformation matrix for the flip-flags.
   * @param d3dDevice The device to change
   * @param stage The texture stage to affect
   * @param textureTransform The transformation to update
   * @return Whether or not the texture transform matrix was changed
   */
  bool configureBaseState(LPDIRECT3DDEVICE9 d3dDevice, DWORD stage, D3DMATRIX* textureTransform);


public:

  /**
   * Returns the type name of this class
   */
  static std::string staticTypeName();


protected:

  /// The source image that this texture refers to
  dc::dcTable<Image>::Reference myImageRef;

  /// Whether or not to flip this texture horizontally
  dc::dcBoolean myFlipHorizontal;

  /// Whether or not to flip this texture vertically
  dc::dcBoolean myFlipVertical;

  /// Whether or not to alpha test this texture
  dc::dcBoolean myEnableAlphaTestingFlag;

  /// Whether or not to blend this texture
  dc::dcBoolean myEnableAlphaBlendingFlag;
};




#endif