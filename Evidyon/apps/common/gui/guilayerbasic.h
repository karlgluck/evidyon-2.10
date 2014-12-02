//------------------------------------------------------------------------------------------------
// File:  guilayerbasic.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUILAYERBASIC_H__
#define __GUILAYERBASIC_H__


/**
 * Implements a GUILayer using a straightforward index-to-texture approach.
 */
class GUILayerBasic : public GUILayer {
  typedef std::map<int, GUITexture*> TextureMap;

public:
  GUILayerBasic();
  ~GUILayerBasic();

  virtual GUITexture* getTexture(int id);

  // setting a NULL texture is allowed and will behave as if that id simply wasn't mapped
  void setTexture(int id, LPDIRECT3DTEXTURE9 d3d_texture);
  void clearAllTextures();

  // returns whether or not a texture with the given ID was removed
  bool removeTexture(int id);

  // returns the number of textures removed.  will not erase anything if the parameter is NULL.
  int removeTexture(LPDIRECT3DTEXTURE9 d3d_texture);

private:
  TextureMap textures_;
};


#endif