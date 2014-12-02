//------------------------------------------------------------------------------------------------
// File:  guirect.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIRECT_H__
#define __GUIRECT_H__


#include "guipoint.h"
#include "guisize.h"

/**
 * Defines a rectangular area
 */
struct GUIRect
{
  short left, top, right, bottom;

  /**
   * Sets up this rectangle to have the top-left coordinate at 'point' and the
   * bottom-right coordinate at point + size
   */
  void set(GUIPoint point, GUISize size);

  /**
   * Determines whether or not this rectangle contains the given point
   */
  bool contains(GUIPoint point) const;

  /**
   * Returns 'true' if the dimensions are non-negative
   */
  bool isValid() const;

  /**
   * Changes the coordinates of the given point such that contains(constrain(point)) == true
   * with the least change in x/y coordinates.
   */
  GUIPoint constrain(GUIPoint point) const;

  /**
   * Calculates the size of this rectangle
   */
  GUISize calculateSize() const;

  /**
   * Calculates the common rectangle between this rectangle and the other
   */
  void intersect(const GUIRect* other, GUIRect* intersection) const;

  /**
   * Determines whether or not this rectangle overlaps another
   */
  bool overlaps(const GUIRect* other) const;

  /**
   * Resets the members of this structure
   */
  void zero();
};



#endif