//------------------------------------------------------------------------------------------------
// File:  guicomponent.h
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUICOMPONENT_H__
#define __GUICOMPONENT_H__


#include "guicanvas.h"


struct GUIRect;
class GUILayer;
class GUIMouseSentinel;
class GUIKeyboardSentinel;



class GUIComponent {
public:
  virtual ~GUIComponent();
  virtual void setClippingArea(const GUIRect* area);
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);
  virtual GUIMouseSentinel* getMouseSentinel();
  virtual GUIKeyboardSentinel* getKeyboardSentinel();
  inline GUICanvas* getCanvas();

  // Removes the mouse and/or keyboard sentinel from the input chain
  void removeFromInputChain();

  // Sets the parent of the parent mouse/keyboard sentinels--if they exist
  void setParentSentinels(GUIMouseSentinel* parent_mouse,
              GUIKeyboardSentinel* parent_keyboard);

private:
  GUICanvas component_canvas_;
};


GUICanvas* GUIComponent::getCanvas() {
  return &component_canvas_;
}



#endif