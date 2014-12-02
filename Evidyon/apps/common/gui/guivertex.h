//------------------------------------------------------------------------------------------------
// File:  guivertex.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIVERTEX_H__
#define __GUIVERTEX_H__


/**
 * This is the vertex structure stored by GUI elements
 */
struct GUIVertex
{
  /// Screen location
  float x, y, depth;

  /// Shade
  D3DCOLOR color;

  /// Texture coordinates
  float u, v;

};

/// This constant defines the format of the GUI's vertices
#define GUIVERTEX_D3DBUFFERFORMAT   D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1

#endif