//------------------------------------------------------------------------------------------------
// File:  guimouse.h
//
// Desc:  Includes the sentinel and input source for the mouse and provides relavent definitions
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIMOUSE_H__
#define __GUIMOUSE_H__

/**
 * Defines different types of mouse button actions
 */
enum GUIMouseButtonAction
{
  /// States for the primary mouse button
  GUIMOUSE_DOWN,
  GUIMOUSE_UP,
  GUIMOUSE_CLICK_SINGLE,
  GUIMOUSE_CLICK_DOUBLE,

  /// States for the alternate mouse button
  GUIMOUSE_ALT_DOWN,
  GUIMOUSE_ALT_UP,
  GUIMOUSE_CLICK_ALT_SINGLE,
  GUIMOUSE_CLICK_ALT_DOUBLE,
};


// Pre-declare these classes, since they reference each other
class GUIMouseSentinel;
class GUIMouseInputSource;

// Include the sentinel and input source
#include "guimouseinputsource.h"
#include "guimousesentinel.h"



#endif