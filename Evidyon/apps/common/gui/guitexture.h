//------------------------------------------------------------------------------------------------
// File:  guitexture.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUITEXTURE_H__
#define __GUITEXTURE_H__


#include <d3d9.h>
#include "guisize.h"


class GUITexture {
public:
  GUITexture();
  ~GUITexture();
  void acquire(LPDIRECT3DTEXTURE9 d3d_texture);
  void release();
  void initStage0Texture(LPDIRECT3DDEVICE9 d3d_device);
  void cleanStage0Texture(LPDIRECT3DDEVICE9 d3d_device);
  GUISize getSize();
  bool contains(LPDIRECT3DTEXTURE9 d3d_texture);

private:
  LPDIRECT3DTEXTURE9 d3d_texture_;
};



#endif