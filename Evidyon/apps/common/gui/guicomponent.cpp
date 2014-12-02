//------------------------------------------------------------------------------------------------
// File:  guicomponent.cpp
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
#include "guikeyboard.h"
#include "guimouse.h"
#include "guicomponent.h"

GUIComponent::~GUIComponent() {
}

void GUIComponent::setClippingArea(const GUIRect* area) {
}

void GUIComponent::registerElements(GUILayer* layer) {
}

void GUIComponent::unregisterElements(GUILayer* layer) {
}

GUIMouseSentinel* GUIComponent::getMouseSentinel() {
  return NULL;
}

GUIKeyboardSentinel* GUIComponent::getKeyboardSentinel() {
  return NULL;
}

void GUIComponent::removeFromInputChain() {
  {
  GUIMouseSentinel* mouse = getMouseSentinel();
  if (mouse) mouse->removeFromMouseSentinelChain();
  }
  {
  GUIKeyboardSentinel* keyboard = getKeyboardSentinel();
  if (keyboard) keyboard->removeFromKeyboardSentinelChain();
  }
}

void GUIComponent::setParentSentinels(GUIMouseSentinel* parent_mouse,
                    GUIKeyboardSentinel* parent_keyboard) {
  {
  GUIMouseSentinel* mouse = getMouseSentinel();
  if (mouse) mouse->setParentMouseSentinel(parent_mouse);
  }
  {
  GUIKeyboardSentinel* keyboard = getKeyboardSentinel();
  if (keyboard) keyboard->setParentKeyboardSentinel(parent_keyboard);
  }
}