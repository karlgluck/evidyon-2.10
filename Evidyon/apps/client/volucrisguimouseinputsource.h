//------------------------------------------------------------------------------------------------
// File:    volucrisguimouseinputsource.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIMOUSEINPUTSOURCE_H__
#define __VOLUCRISGUIMOUSEINPUTSOURCE_H__

#include "../common/gui/guipoint.h"

/**
 * Gets mouse input and passes it into the GUI
 */
class VolucrisGUIMouseInputSource : public GUIMouseInputSource
{
public:

  VolucrisGUIMouseInputSource();
  LRESULT handleWindowsMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* returnResult);
  void setMouseLock(bool enable);
  //void jumpTo(GUIPoint point);

protected:
  bool lock_mouse_to_window_;
  HWND main_window;

};



#endif