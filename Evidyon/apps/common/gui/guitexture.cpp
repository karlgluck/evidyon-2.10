//------------------------------------------------------------------------------------------------
// File:  guitexture.cpp
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "guitexture.h"



//------------------------------------------------------------------------------------------------
GUITexture::GUITexture() {
  d3d_texture_ = NULL;
}


//------------------------------------------------------------------------------------------------
GUITexture::~GUITexture() {
  release();
}


//------------------------------------------------------------------------------------------------
void GUITexture::acquire(LPDIRECT3DTEXTURE9 d3d_texture) {
  release();
  if (d3d_texture != NULL)
  (d3d_texture_ = d3d_texture)->AddRef();
}


//------------------------------------------------------------------------------------------------
void GUITexture::release() {
  if (d3d_texture_) d3d_texture_->Release();
  d3d_texture_ = NULL;
}


//------------------------------------------------------------------------------------------------
void GUITexture::initStage0Texture(LPDIRECT3DDEVICE9 d3d_device) {
  d3d_device->SetTexture(0, d3d_texture_);
}


//------------------------------------------------------------------------------------------------
void GUITexture::cleanStage0Texture(LPDIRECT3DDEVICE9 d3d_device) {
  d3d_device->SetTexture(0, NULL);
}


//------------------------------------------------------------------------------------------------
GUISize GUITexture::getSize() {
  D3DSURFACE_DESC desc;
  d3d_texture_->GetLevelDesc(0, &desc);
  GUISize size = { desc.Width, desc.Height };
  return size;
}


//------------------------------------------------------------------------------------------------
bool GUITexture::contains(LPDIRECT3DTEXTURE9 d3d_texture) {
  return d3d_texture_ == d3d_texture;
}