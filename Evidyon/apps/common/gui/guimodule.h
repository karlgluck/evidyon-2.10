//------------------------------------------------------------------------------------------------
// File:  guimodule.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIMODULE_H__
#define __GUIMODULE_H__

#include <set>
#include "guicomponentset.h"

/**
 * A module is a fully-functional, self-contained part of a GUI.  Examples of
 * modules could be dialog boxes, inventory lists and key-binding lists.
 */
class GUIModule {
public:
  GUIModule();

  // Registers the components and submodules with the given layer, and enables
  // the input sentinels.
  void show(GUILayer* layer);

  // Unregisters the visual elements and disables input sentinels.  This method
  // returns the the previous parameter to show() or NULL.
  GUILayer* hide();

  // Whether or not this module is being drawn and is receiving input
  bool isActive() const;

  // Gets the mouse and keyboard focus for this module
  void obtainFocus();

  // Clears mouse and keyboard focus from this module
  void releaseFocus();

  // Determines whether or not this module has focus
  bool hasFocus() const;

  // Initializes the sentinels that pass information to this module.  Unless it
  // is shown, this module will not interpret messages.
  void setParentSentinels(GUIMouseSentinel* mouse, GUIKeyboardSentinel* keyboard);

  // Clears the parent sentinels
  void removeFromInputChain();

protected:

  // Called by the implementing module class to add another module to this one.
  // This method automatically initializes the module's parent sentinels, and
  // will call show() if the current layer is valid.
  void addSubmodule(GUIModule* module);

  // Erases the given submodule from reference.  This automatically calls
  // hide() on the submodule and removes its parent sentinels.
  void removeSubmodule(GUIModule* module);

  // Inserts the component into this module.  Until removed, its visual and
  // input aspects will be managed entirely by this class.
  void addComponent(GUIComponent* component);
  void removeComponent(GUIComponent* component);
  void clearComponents();


  // These input routines are invoked by the internal module sentinels.  For
  // those methods with a return value, 'true' indicates that the message has
  // been processed and should no longer propagate.
  // None of these methods will be called while the module is hidden.
protected:
  virtual void onGotMouseFocus();
  virtual void onLostMouseFocus();
  virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
  virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);
  virtual bool onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse);
  virtual void onGotKeyboardFocus();
  virtual void onLostKeyboardFocus();
  virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onTypedCharacter(char character, GUIKeyboardInputSource* keyboard);



private:

  // passes messages first to the owner module, then to children
  class ModuleKeyboardSentinel : public GUIKeyboardSentinel {
  public:
    ModuleKeyboardSentinel(GUIModule* owner);
    virtual void onGotKeyboardFocus();
    virtual void onLostKeyboardFocus();
    virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
    virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);
    virtual bool onTypedCharacter(char character, GUIKeyboardInputSource* keyboard);
  private:
    GUIModule* owner_;
  };

  // passes messages first to the owner module, then to children
  class ModuleMouseSentinel : public GUIMouseSentinel {
  public:
    ModuleMouseSentinel(GUIModule* owner);
    virtual void onGotMouseFocus();
    virtual void onLostMouseFocus();
    virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
    virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);
    virtual bool onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse);
  private:
    GUIModule* owner_;
  };

private:
  typedef std::set<GUIModule*> Submodules;

private:
  GUILayer* active_layer_;
  ModuleMouseSentinel module_mouse_sentinel_;
  ModuleKeyboardSentinel module_keyboard_sentinel_;
  Submodules submodules_;
  GUIComponentSet components_;
};




#endif
