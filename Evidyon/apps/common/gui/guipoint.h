//------------------------------------------------------------------------------------------------
// File:  guipoint.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIPOINT_H__
#define __GUIPOINT_H__


/**
 * This structure represents a 32-bit container for a point on the screen
 */
struct GUIPoint
{
  short x, y;

  /**
   * Resets the members of this structure
   */
  void zero();
};




#endif