//------------------------------------------------------------------------------------------------
// File:  guirect.cpp
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"



//------------------------------------------------------------------------------------------------
// Name: set
// Desc: Sets up this rectangle to have the top-left coordinate at 'point' and the
//     bottom-right coordinate at point + size
//------------------------------------------------------------------------------------------------
void GUIRect::set(GUIPoint point, GUISize size)
{
  left = point.x;
  top = point.y;
  right = left + size.width;
  bottom = top + size.height;
}



//------------------------------------------------------------------------------------------------
// Name: contains
// Desc: Determines whether or not this rectangle contains the given point
//------------------------------------------------------------------------------------------------
bool GUIRect::contains(GUIPoint point) const
{
  return point.x >= left && point.x < right && point.y >= top && point.y < bottom;
}



//------------------------------------------------------------------------------------------------
// Name: isValid
// Desc: Returns 'true' if the dimensions are non-negative
//------------------------------------------------------------------------------------------------
bool GUIRect::isValid() const
{
  GUISize size = calculateSize();
  return size.width >= 0 && size.height >= 0;
}




//------------------------------------------------------------------------------------------------
// Name: constrain
// Desc: Changes the coordinates of the given point so that the point lies within this rectangle
//------------------------------------------------------------------------------------------------
GUIPoint GUIRect::constrain(GUIPoint point) const
{
  point.x = point.x > left   ? point.x : left;
  point.y = point.y > top  ? point.y : top;
  point.x = point.x < right  ? point.x : right;
  point.y = point.y < bottom ? point.y : bottom;

  return point;
}



//------------------------------------------------------------------------------------------------
// Name: calculateSize
// Desc: Calculates the size of this rectangle
//------------------------------------------------------------------------------------------------
GUISize GUIRect::calculateSize() const
{
  GUISize size = { right - left, bottom - top };
  return size;
}



//------------------------------------------------------------------------------------------------
// Name: intersect
// Desc: Calculates the common rectangle between this rectangle and the other
//------------------------------------------------------------------------------------------------
void GUIRect::intersect(const GUIRect* other, GUIRect* intersection) const
{
#define min(a,b) ((a)<(b))?(a):(b);
#define max(a,b) ((a)>(b))?(a):(b);
  intersection->left   = max(left, other->left);
  intersection->right  = min(right, other->right);
  intersection->top  = max(top, other->top);
  intersection->bottom = min(bottom, other->bottom);
}



//------------------------------------------------------------------------------------------------
// Name: overlaps
// Desc: Determines whether or not this rectangle overlaps another
//------------------------------------------------------------------------------------------------
bool GUIRect::overlaps(const GUIRect* other) const
{
  GUIRect intersection;
  intersect(other, &intersection);
  return intersection.isValid();
}



//------------------------------------------------------------------------------------------------
// Name: zero
// Desc: Resets the members of this structure
//------------------------------------------------------------------------------------------------
void GUIRect::zero()
{
  left = 0;
  right = 0;
  top = 0;
  bottom = 0;
}