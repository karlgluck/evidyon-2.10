//------------------------------------------------------------------------------------------------
// File:    volucrisguikeybindingscreen.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIKEYBINDINGSCREEN_H__
#define __VOLUCRISGUIKEYBINDINGSCREEN_H__


class UserEventQueue;


/**
 * This class is used to allow the user to configure what happens when the user presses a certain
 * key.
 */
class VolucrisGUIKeyBindingScreen
{
  class KeyboardSentinel : public GUIKeyboardSentinel {
  public:
    KeyboardSentinel(VolucrisGUIKeyBindingScreen* owner);
    void enable(bool enabled);
    bool isEnabled() { return enabled_; }
    virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);

  private:
    VolucrisGUIKeyBindingScreen* owner_;
    bool enabled_;
  };


public:

  /**
   * Initializes this class
   */
  VolucrisGUIKeyBindingScreen();

  /**
   * Initializes this display of this key binding screen on the target rendering layer.  This method
   * doesn't draw anything to the layer until items are added.
   */
  bool create(const ClientSpellDescription* allSpellDescriptions,
              size_t numberOfSpellDescriptions,
              GUIFont* windowFont, GUIFont* titleFont, GUILayer* layer,
              GUIMouseSentinel* parentMouseSentinel, GUIKeyboardSentinel* parentKeyboardSentinel,
              UserEventQueue* user_action_queue);

  /**
   * Erases all of the members of this screen, and removes its components from the
   * display layer.
   */
  void destroy();

  /**
   * Sets up this renderer to display the given key mappings.  Passing 'null' is
   * equivalent to calling hideDisplay.
   */
  void displayKeyBindings(const SpellAvailabilityMask* effectsToShow, const unsigned int currentKeyToEffectMapping[256], GUIKey key_to_actions[UserEvents::NUMBER_OF_BOUND_ACTION_KEYS]);

  /**
   * Constructs the list of key->effect mappings from user's selections
   */
  void getKeyBindings(unsigned int keyToEffectMapping[256], GUIKey key_to_actions[UserEvents::NUMBER_OF_BOUND_ACTION_KEYS]);

  /**
   * Removes all of this display's items from the screen and mouse/keyboard input chain
   */
  void hideDisplay();

  /**
   * If the finished button was pushed, this method returns true.  Subsequent calls will return false
   * until the user pushes the finished button again.
   */
  bool pollFinishedButton();

  void updateUserEvents();


protected:

  /// The target layer for this display
  GUILayer* myRenderingLayer;

  /// The owner mouse sentinel for this display
  GUIMouseSentinel* myParentMouseSentinel;

  /// The owner keyboard sentinel for this display
  GUIKeyboardSentinel* myParentKeyboardSentinel;

  /// The source effect description array, obtained from the client game file
  const ClientSpellDescription* myEffectDescriptionsArray;
  size_t myEffectDescriptionsArraySize;

  /// The panel where the key bindings are shown
  VolucrisGUITitledPanel myKeyBindingPanel;

  /// The list used to render the effect->key bindings
  VolucrisGUIEffectKeyBindingList myKeyBindingList;

  /// The list used to render the key->action bindings
  VolucrisGUIUserEventKeyBindingList myActionKeyBindings;

  /// When the user presses this button, they want to leave the screen
  VolucrisGUIButton myFinishedButton;

  KeyboardSentinel myKeyboardSentinel;
  UserEventQueue* user_action_queue_;
};


#endif