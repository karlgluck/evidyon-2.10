//------------------------------------------------------------------------------------------------
// File:  guilayerbasic.cpp
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"
#include <d3dx9.h>
#include <map>
#include "guielement.h"
#include "guivertex.h"
#include "guiindex.h"
#include "guitexture.h"
#include "guilayer.h"
#include "guilayerbasic.h"



//------------------------------------------------------------------------------------------------
GUILayerBasic::GUILayerBasic() {
}


//------------------------------------------------------------------------------------------------
GUILayerBasic::~GUILayerBasic() {
  clearAllTextures();
}


//------------------------------------------------------------------------------------------------
GUITexture* GUILayerBasic::getTexture(int id) {
  TextureMap::iterator i = textures_.find(id);
  if (i == textures_.end()) return NULL;
  return i->second;
}


//------------------------------------------------------------------------------------------------
void GUILayerBasic::setTexture(int id, LPDIRECT3DTEXTURE9 d3d_texture) {
  if (d3d_texture != NULL) {
  GUITexture* gui_texture = getTexture(id);
  if (!gui_texture) {
    gui_texture = new GUITexture();
    textures_.insert(TextureMap::value_type(id, gui_texture));
  }
  gui_texture->acquire(d3d_texture);
  } else {
  removeTexture(id);
  textures_.insert(TextureMap::value_type(id, (GUITexture*)NULL));
  }
}


//------------------------------------------------------------------------------------------------
void GUILayerBasic::clearAllTextures() {
  for (TextureMap::iterator i = textures_.begin(); i != textures_.end(); ++i) {
  GUITexture* gui_texture = i->second;
  if (gui_texture) {
    gui_texture->release();
    delete gui_texture;
  }
  }
  textures_.clear();
}

//------------------------------------------------------------------------------------------------
bool GUILayerBasic::removeTexture(int id) {
  TextureMap::iterator i = textures_.find(id);
  if (i == textures_.end()) return false;
  GUITexture* gui_texture = i->second;
  if (gui_texture) {
  gui_texture->release();
  delete gui_texture;
  }
  textures_.erase(i);
  return true;
}

//------------------------------------------------------------------------------------------------
int GUILayerBasic::removeTexture(LPDIRECT3DTEXTURE9 d3d_texture) {
  TextureMap::iterator next;
  int number_erased = 0;
  if (d3d_texture == NULL) return 0; // don't erase NULL textures this way!

  for (TextureMap::iterator i = textures_.begin(); i != textures_.end(); i = next) {
  next = i; ++next;
  GUITexture* gui_texture = i->second;
  if (gui_texture == NULL) continue;
  if (gui_texture->contains(d3d_texture)) {
    gui_texture->release();
    delete gui_texture;
    textures_.erase(i);
    ++number_erased;
  }
  }

  return number_erased;
}
