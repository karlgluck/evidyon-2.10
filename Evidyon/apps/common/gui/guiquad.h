//------------------------------------------------------------------------------------------------
// File:  guiquad.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIQUAD_H__
#define __GUIQUAD_H__

/**
 * Used to edit a quad of vertices
 */
struct GUIQuad
{
  /// The vertices of this quad.  They are arranged in the following pattern:
  ///
  ///   0----1
  ///   |  / |
  ///   | /  |
  ///   2----3
  GUIVertex vertices[4];

  /**
   * Sets the depth and color of all 4 vertices
   */
  void initialize(float depth, D3DCOLOR color);

  void createRotated(float center_x, float center_y, float radius, float angle);

  /**
   * Clips the vertices so that they don't go outside the bounds of the given rectangle.  If
   * the whole quad is outside of the rectangle, it is guaranteed to have at least one
   * zero dimension.
   */
  void clipSquare(const GUIRect* visibleArea, float texcoordsPerPixelOfWidth, float texcoordsPerPixelOfHeight);

  /**
   * Copies the vertices in this quad to the given pointer
   */
  GUIVertex* copyTo(GUIVertex* vertexBuffer);
};


#endif