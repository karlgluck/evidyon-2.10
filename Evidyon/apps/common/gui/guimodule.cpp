//------------------------------------------------------------------------------------------------
// File:  guimodule.cpp
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
#include "guikeyboard.h"
#include "guimouse.h"
#include "guicomponent.h"
#include "guimodule.h"




//-----------------------------------------------------------------------------
GUIModule::ModuleKeyboardSentinel::ModuleKeyboardSentinel(GUIModule* owner) {
  owner_ = owner;
}

//-----------------------------------------------------------------------------
void GUIModule::ModuleKeyboardSentinel::onGotKeyboardFocus() {
  owner_->onGotKeyboardFocus();
}

//-----------------------------------------------------------------------------
void GUIModule::ModuleKeyboardSentinel::onLostKeyboardFocus() {
  owner_->onLostKeyboardFocus();
}

//-----------------------------------------------------------------------------
bool GUIModule::ModuleKeyboardSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (owner_->onKeyDown(key, keyboard)) return true;
  return GUIKeyboardSentinel::onKeyDown(key, keyboard);
}

//-----------------------------------------------------------------------------
bool GUIModule::ModuleKeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (owner_->onKeyUp(key, keyboard)) return true;
  return GUIKeyboardSentinel::onKeyUp(key, keyboard);
}

//-----------------------------------------------------------------------------
bool GUIModule::ModuleKeyboardSentinel::onTypedCharacter(char character, GUIKeyboardInputSource* keyboard) {
  if (owner_->onTypedCharacter(character, keyboard)) return true;
  return GUIKeyboardSentinel::onTypedCharacter(character, keyboard);
}





//-----------------------------------------------------------------------------
GUIModule::ModuleMouseSentinel::ModuleMouseSentinel(GUIModule* owner) {
  owner_ = owner;
}

//-----------------------------------------------------------------------------
void GUIModule::ModuleMouseSentinel::onGotMouseFocus() {
  owner_->onGotMouseFocus();
}

//-----------------------------------------------------------------------------
void GUIModule::ModuleMouseSentinel::onLostMouseFocus() {
  owner_->onLostMouseFocus();
}

//-----------------------------------------------------------------------------
bool GUIModule::ModuleMouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  if (owner_->onMouseMove(position, oldPosition, mouse)) return true;
  else return GUIMouseSentinel::onMouseMove(position, oldPosition, mouse);
}

//-----------------------------------------------------------------------------
bool GUIModule::ModuleMouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse) {
  if (owner_->onMouseButtonAction(action, position, mouse)) return true;
  else return GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
}

//-----------------------------------------------------------------------------
bool GUIModule::ModuleMouseSentinel::onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse) {
  if (owner_->onMouseScroll(position, amount, lines, mouse)) return true;
  else return GUIMouseSentinel::onMouseScroll(position, amount, lines, mouse);
}









//-----------------------------------------------------------------------------
GUIModule::GUIModule() : module_mouse_sentinel_(this), module_keyboard_sentinel_(this) {
  active_layer_ = NULL;
  module_mouse_sentinel_.disable();
  module_keyboard_sentinel_.disable();
  components_.setParentSentinels(&module_mouse_sentinel_,
                                 &module_keyboard_sentinel_);
}

//-----------------------------------------------------------------------------
void GUIModule::show(GUILayer* layer) {

  // update all submodules
  for (Submodules::iterator i = submodules_.begin(); i != submodules_.end(); ++i) {
    GUIModule* submodule = (*i);
    submodule->show(layer);
  }

  if (layer == NULL) {
    // the layer is invalid, so this module is being hidden
    components_.unregisterElements();
    module_mouse_sentinel_.disable();
    module_keyboard_sentinel_.disable();
  } else {
    // the layer is valid, so this module is active
    components_.registerElements(layer);
    module_mouse_sentinel_.enable();
    module_keyboard_sentinel_.enable();
  }

  active_layer_ = layer;
}


//-----------------------------------------------------------------------------
GUILayer* GUIModule::hide() {
  GUILayer* previous_layer = active_layer_;
  show(NULL);
  return previous_layer;
}


//-----------------------------------------------------------------------------
bool GUIModule::isActive() const {
  return active_layer_ != NULL;
}


//-----------------------------------------------------------------------------
void GUIModule::obtainFocus() {
  module_keyboard_sentinel_.obtainKeyboardFocus();
  module_mouse_sentinel_.obtainMouseFocus();
}

//-----------------------------------------------------------------------------
void GUIModule::releaseFocus() {
  module_keyboard_sentinel_.releaseKeyboardFocus();
  module_mouse_sentinel_.releaseMouseFocus();
}

//-----------------------------------------------------------------------------
bool GUIModule::hasFocus() const {
  return module_keyboard_sentinel_.hasKeyboardFocus() && module_mouse_sentinel_.hasMouseFocus();
}

//-----------------------------------------------------------------------------
void GUIModule::setParentSentinels(GUIMouseSentinel* mouse, GUIKeyboardSentinel* keyboard) {
  module_mouse_sentinel_.setParentMouseSentinel(mouse);
  module_keyboard_sentinel_.setParentKeyboardSentinel(keyboard);
}

//-----------------------------------------------------------------------------
void GUIModule::removeFromInputChain() {
  module_mouse_sentinel_.removeFromMouseSentinelChain();
  module_keyboard_sentinel_.removeFromKeyboardSentinelChain();
}

//-----------------------------------------------------------------------------
void GUIModule::addSubmodule(GUIModule* module) {
  module->setParentSentinels(&module_mouse_sentinel_,
                             &module_keyboard_sentinel_);
  module->show(active_layer_);
  submodules_.insert(module);
}

//-----------------------------------------------------------------------------
void GUIModule::removeSubmodule(GUIModule* module) {
  module->hide();
  module->removeFromInputChain();
  submodules_.erase(module);
}

//-----------------------------------------------------------------------------
void GUIModule::addComponent(GUIComponent* component) {
  components_.add(component);
}

//-----------------------------------------------------------------------------
void GUIModule::removeComponent(GUIComponent* component) {
  components_.remove(component);
}

//-----------------------------------------------------------------------------
void GUIModule::clearComponents() {
  components_.clear();
}


// Default implementations of the input routines
void GUIModule::onGotMouseFocus() { }
void GUIModule::onLostMouseFocus() { }
bool GUIModule::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)  { return false; }
bool GUIModule::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse)  { return false; }
bool GUIModule::onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse)  { return false; }
void GUIModule::onGotKeyboardFocus() { }
void GUIModule::onLostKeyboardFocus() { }
bool GUIModule::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard)  { return false; }
bool GUIModule::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard)  { return false; }
bool GUIModule::onTypedCharacter(char character, GUIKeyboardInputSource* keyboard)  { return false; }
