//------------------------------------------------------------------------------------------------
// File:    volucrisguikeybindingscreen.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "useractionqueue.h"





//------------------------------------------------------------------------------------------------
VolucrisGUIKeyBindingScreen::KeyboardSentinel::KeyboardSentinel(VolucrisGUIKeyBindingScreen* owner) {
  owner_ = owner;
  enabled_ = false;
}



//------------------------------------------------------------------------------------------------
void VolucrisGUIKeyBindingScreen::KeyboardSentinel::enable(bool enabled) {
  enabled_ = enabled;
}



//------------------------------------------------------------------------------------------------
bool VolucrisGUIKeyBindingScreen::KeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!enabled_) return false; // don't handle messages unless enabled
  if (GUIKeyboardSentinel::onKeyUp(key, keyboard)) return true; // don't process if the key was caught
  if (key == GUIKEY_ESCAPE) {
    owner_->myFinishedButton.push();
    return true;
  }
  return false;
}





//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIKeyBindingScreen
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUIKeyBindingScreen::VolucrisGUIKeyBindingScreen()
    : myKeyboardSentinel(this)
{
    myRenderingLayer = 0;
    myParentMouseSentinel = 0;
    myParentKeyboardSentinel = 0;
    myEffectDescriptionsArray = 0;
    myEffectDescriptionsArraySize = 0;
    user_action_queue_ = NULL;

    myKeyBindingPanel.setText("Key Bindings");
    myKeyBindingPanel.getCanvas()->relativeWidth(-80);
    myKeyBindingPanel.getCanvas()->relativeHeight(-160);
    myKeyBindingPanel.getCanvas()->alignXCenter(-0);
    myKeyBindingPanel.getCanvas()->alignYCenter(0);
    myKeyBindingPanel.getCanvas()->absoluteDepth(0.5f);

    myActionKeyBindings.getCanvas()->setSourceCanvas(myKeyBindingPanel.getCanvas());
    myActionKeyBindings.getCanvas()->absoluteHeight(200);
    myActionKeyBindings.getCanvas()->relativeWidth(-30);
    myActionKeyBindings.getCanvas()->alignXCenter(0);
    myActionKeyBindings.getCanvas()->alignYTop(16);
    myActionKeyBindings.getCanvas()->relativeDepth(- 0.1f);
    int action_key_height = myActionKeyBindings.getCanvas()->getArea()->calculateSize().height;

    myKeyBindingList.getCanvas()->setSourceCanvas(myKeyBindingPanel.getCanvas());
    myKeyBindingList.getCanvas()->relativeWidth(-30);
    myKeyBindingList.getCanvas()->relativeHeight(-(action_key_height + 24 + 10));
    myKeyBindingList.getCanvas()->alignXCenter(0);
    myKeyBindingList.getCanvas()->alignYTop(16 + action_key_height + 5);
    myKeyBindingList.getCanvas()->relativeDepth(-0.1f);

    myFinishedButton.setText("Finished");
    myFinishedButton.getCanvas()->absoluteWidth(200);
    myFinishedButton.getCanvas()->absoluteHeight(35);
    myFinishedButton.getCanvas()->alignXCenter(0);
    myFinishedButton.getCanvas()->alignYBottom(-10);
    myFinishedButton.getCanvas()->absoluteDepth(0.5f);
}



//------------------------------------------------------------------------------------------------
// Name: create
// Desc: Initializes this display of this key binding screen on the target rendering layer.  This method
//       doesn't draw anything to the layer until items are added.
//------------------------------------------------------------------------------------------------
bool VolucrisGUIKeyBindingScreen::create(
    const ClientSpellDescription* allSpellDescriptions,
    size_t numberOfSpellDescriptions,
    GUIFont* windowFont, GUIFont* titleFont, GUILayer* layer,
    GUIMouseSentinel* parentMouseSentinel, GUIKeyboardSentinel* parentKeyboardSentinel,
    UserEventQueue* user_action_queue)
{
    // Save persistent parameters
    myRenderingLayer = layer;
    myParentMouseSentinel = parentMouseSentinel;
    myParentKeyboardSentinel = parentKeyboardSentinel;
    myEffectDescriptionsArray = allSpellDescriptions;
    myEffectDescriptionsArraySize = numberOfSpellDescriptions;
    user_action_queue_ = user_action_queue;

    myKeyboardSentinel.setParentKeyboardSentinel(parentKeyboardSentinel);

    // Set up all of the panel canvases
    myKeyBindingPanel.getCanvas()->setSourceCanvas(layer->getPrimaryCanvas());
    myFinishedButton.getCanvas()->setSourceCanvas(layer->getPrimaryCanvas());

    // Set the parameters for each of the panels
    myKeyBindingPanel.setFont(titleFont);

    // Set the parameters for the buttons
    myFinishedButton.setFont(titleFont);


    myKeyBindingList.getKeyboardSentinel()->setParentKeyboardSentinel(&myKeyboardSentinel);

    // Finally, initialize the lists
    if (APP_ERROR(!myKeyBindingList.create(windowFont, layer))("Couldn't create key binding list") ||
        APP_ERROR(!myActionKeyBindings.create(windowFont, parentMouseSentinel, &myKeyboardSentinel)) ("Couldn't create action key binding list"))
        return false;

    

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Erases all of the members of this screen, and removes its components from the
//       display layer.
//------------------------------------------------------------------------------------------------
void VolucrisGUIKeyBindingScreen::destroy()
{
    hideDisplay();

    myActionKeyBindings.destroy();
    myKeyBindingList.destroy();

    myKeyboardSentinel.removeFromKeyboardSentinelChain();

    // TODO: zero()
    myRenderingLayer = 0;
    myParentMouseSentinel = 0;
    myParentKeyboardSentinel = 0;
    myEffectDescriptionsArray = 0;
    myEffectDescriptionsArraySize = 0;
}



//------------------------------------------------------------------------------------------------
// Name: displayKeyBindings
// Desc: Sets up this renderer to display the given key mappings.  Passing 'null' is
//       equivalent to calling hideDisplay.
//------------------------------------------------------------------------------------------------
void VolucrisGUIKeyBindingScreen::displayKeyBindings(const SpellAvailabilityMask* effectsToShow,
                                                      const unsigned int currentKeyToEffectMapping[256],
                                                      GUIKey key_to_actions[UserEvents::NUMBER_OF_BOUND_ACTION_KEYS])
{
    // Remove all items from the display
    hideDisplay();

    // Update the lists
    myKeyBindingList.initialize(currentKeyToEffectMapping, effectsToShow,
                                 myEffectDescriptionsArray, myEffectDescriptionsArraySize);
    myKeyBindingList.show();
    myActionKeyBindings.show(myRenderingLayer);
    myActionKeyBindings.setKeyToActionBindings(key_to_actions);

    myKeyboardSentinel.enable(true);

    // Register display elements
    myKeyBindingPanel.registerElements(myRenderingLayer);
    myFinishedButton.registerElements(myRenderingLayer);

    // Update the lists
    myKeyBindingList.refresh();

    // Reset button states
    myFinishedButton.enable();
    myFinishedButton.resetPushed();

    // Add to the input chain
    myKeyBindingList.getMouseSentinel()->setParentMouseSentinel(myParentMouseSentinel);
    myFinishedButton.getMouseSentinel()->setParentMouseSentinel(myParentMouseSentinel);
}



//------------------------------------------------------------------------------------------------
// Name: getKeyBindings
// Desc: Constructs the list of key->effect mappings from user's selections
//------------------------------------------------------------------------------------------------
void VolucrisGUIKeyBindingScreen::getKeyBindings(unsigned int keyToEffectMapping[256], GUIKey key_to_actions[UserEvents::NUMBER_OF_BOUND_ACTION_KEYS])
{
    myKeyBindingList.buildKeyToEffectMappings(keyToEffectMapping);
    myActionKeyBindings.buildKeyToActionBindings(key_to_actions);
}



//------------------------------------------------------------------------------------------------
// Name: hideDisplay
// Desc: Removes all of this display's items from the screen and mouse/keyboard input chain
//------------------------------------------------------------------------------------------------
void VolucrisGUIKeyBindingScreen::hideDisplay()
{
    myKeyBindingList.hide();
    myActionKeyBindings.hide();

    myKeyboardSentinel.enable(false);

    if (myRenderingLayer) {
      myKeyBindingPanel.unregisterElements(myRenderingLayer);
      myFinishedButton.unregisterElements(myRenderingLayer);
    }

    myKeyBindingList.getMouseSentinel()->removeFromMouseSentinelChain();
    myFinishedButton.getMouseSentinel()->removeFromMouseSentinelChain();
}



//------------------------------------------------------------------------------------------------
// Name: pollFinishedButton
// Desc: If the finished button was pushed, this method returns true.  Subsequent calls will return false
//       until the user pushes the finished button again.
//------------------------------------------------------------------------------------------------
bool VolucrisGUIKeyBindingScreen::pollFinishedButton()
{
    bool pushed = myFinishedButton.wasPushed();
    if (pushed) myFinishedButton.resetPushed();
    return pushed;
}


void VolucrisGUIKeyBindingScreen::updateUserEvents() {
  if (pollFinishedButton()) {
    user_action_queue_->addCloseKeyBindings();
  }
}
