//------------------------------------------------------------------------------------------------
// File:    volucrisguistatdisplay.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguistatdisplay.h"
#include "useractionqueue.h"
#include "volucrisguidefs.h"



//------------------------------------------------------------------------------------------------
VolucrisGUIStatDisplay::KeyboardSentinel::KeyboardSentinel(VolucrisGUIStatDisplay* owner) {
  owner_ = owner;
  enabled_ = false;
}



//------------------------------------------------------------------------------------------------
void VolucrisGUIStatDisplay::KeyboardSentinel::enable(bool enabled) {
  enabled_ = enabled;
}



//------------------------------------------------------------------------------------------------
bool VolucrisGUIStatDisplay::KeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!enabled_) return false; // don't handle messages unless enabled
  if (GUIKeyboardSentinel::onKeyUp(key, keyboard)) return true; // don't process if the key was caught
  if (key == GUIKEY_ESCAPE) {
    owner_->myFinishedButton.push();
    return true;
  }
  return false;
}






//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIStatDisplay
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUIStatDisplay::VolucrisGUIStatDisplay() : keyboard_sentinel_(this)
{
    zero();
}



//------------------------------------------------------------------------------------------------
// Name: create
// Desc: Initializes this display of this stat display on the target rendering layer.  This method
//       doesn't draw anything to the layer until the display method is invoked.
//------------------------------------------------------------------------------------------------
bool VolucrisGUIStatDisplay::create(GUIFont* windowFont, GUIFont* titleFont, GUILayer* layer,
    GUIMouseSentinel* parentMouseSentinel, GUIKeyboardSentinel* parentKeyboardSentinel, UserEventQueue* user_action_queue)
{
    CONFIRM(windowFont && titleFont && parentMouseSentinel) else return false;

    // Save persistent parameters
    myRenderingLayer = layer;
    myParentMouseSentinel = parentMouseSentinel;
    user_action_queue_ = user_action_queue;
    keyboard_sentinel_.setParentKeyboardSentinel(parentKeyboardSentinel);

    // Set up all of the panel canvases
    myAvatarPanel.getCanvas()->setSourceCanvas(layer->getPrimaryCanvas());

    // Set the parameters for each of the panels
    myAvatarPanel.setFont(titleFont);

    for (int i = 0; i < 5; ++i)
    {
        myAbilityText[i].setFont(windowFont);
        myAbilityText[i].setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
        myAbilityButtons[i].setFont(windowFont);

    }

    myStatsText.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
    myStatsText.setFont(titleFont);

    // Set the parameters for the buttons
    myFinishedButton.setFont(titleFont);


    myDescriptionText.setFont(windowFont);
    myDescriptionText.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);


    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Erases all of the members of this screen, and removes its components from the
//       display layer.
//------------------------------------------------------------------------------------------------
void VolucrisGUIStatDisplay::destroy()
{
    hideDisplay();
    keyboard_sentinel_.removeFromKeyboardSentinelChain();

    zero();
}



//------------------------------------------------------------------------------------------------
// Name: display
// Desc: Sets up this display to show the given avatar
//------------------------------------------------------------------------------------------------
void VolucrisGUIStatDisplay::update(unsigned char level, unsigned char abilities[5],
    unsigned char availableAbilityPoints, int hp, int hpMax, int mp, int mpMax, int exp,
    int nextLevelExp, int defense)
{
    myStatPointsAreAvailableToBeAssigned = availableAbilityPoints > 0;

    if (availableAbilityPoints)
    {
        char text[128];
        sprintf_s(text, sizeof(text), "%lu stat point%s available", availableAbilityPoints, availableAbilityPoints != 1 ? "s are" : " is");
        myDescriptionText.setText(text);
    }
    else
        myDescriptionText.setText("");

    // Build attribute text
    for (int i = 0; i < 5; ++i)
    {
        const char* mask;
        switch(i)
        {
            case 0: mask = "%lu  strength"; break;
            case 1: mask = "%lu  agility"; break;
            case 2: mask = "%lu  constitution"; break;
            case 3: mask = "%lu  intelligence"; break;
            case 4: mask = "%lu  wisdom"; break;
        }

        char abilityText[128];
        sprintf_s(abilityText, sizeof(abilityText), mask, abilities[i]);

        myAbilityText[i].setText(abilityText);

        if (myStatPointsAreAvailableToBeAssigned)
        {
            myAbilityButtons[i].enable();
            myAbilityButtons[i].resetPushed();
        }
    }

    // Build the stat text
    char statText[512];
    sprintf_s(statText, sizeof(statText), "Level:   %i\n\n"\
                                           "HP:      %i / %i\n"\
                                           "MP:      %i / %i\n"\
                                           "EXP:     %i / %i\n\n"\
                                           "defense: %i",
               level, hp, hpMax, mp, mpMax, exp, nextLevelExp, defense);
    myStatsText.setText(statText);

    // Reset button states
    myFinishedButton.enable();
    myFinishedButton.resetPushed();
}



//------------------------------------------------------------------------------------------------
// Name: show
// Desc: Displays the stat screen.  If it hasn't been updated, it will show invalid data.
//------------------------------------------------------------------------------------------------
void VolucrisGUIStatDisplay::show()
{
    // Remove all items from the display
    hideDisplay();
    
    keyboard_sentinel_.enable(true);

    // Make sure this class is valid
    CONFIRM(myRenderingLayer) else return;

    for (int i = 0; i < 5; ++i)
        myRenderingLayer->registerElement(&myAbilityText[i]);

    if (myStatPointsAreAvailableToBeAssigned)
    {
        for (int i = 0; i < 5; ++i)
        {
            myAbilityButtons[i].resetPushed();
            myAbilityButtons[i].registerElements(myRenderingLayer);
            myAbilityButtons[i].getMouseSentinel()->setParentMouseSentinel(myParentMouseSentinel);
        }
    }


    myAvatarPanel.registerElements(myRenderingLayer);
    myRenderingLayer->registerElement(&myStatsText);
    myRenderingLayer->registerElement(&myDescriptionText);
    //myFinishedButton.registerElements(myRenderingLayer);
    //myFinishedButton.getMouseSentinel()->setParentMouseSentinel(myParentMouseSentinel);
}



//------------------------------------------------------------------------------------------------
// Name: hideDisplay
// Desc: Removes all of this display's items from the screen and mouse/keyboard input chain
//------------------------------------------------------------------------------------------------
void VolucrisGUIStatDisplay::hideDisplay()
{
    keyboard_sentinel_.enable(false);
    if (!myRenderingLayer) return;

    for (int i = 0; i < 5; ++i)
    {
        myRenderingLayer->unregisterElement(&myAbilityText[i]);
        myAbilityButtons[i].unregisterElements(myRenderingLayer);
        myAbilityButtons[i].getMouseSentinel()->removeFromMouseSentinelChain();
    }

    myAvatarPanel.unregisterElements(myRenderingLayer);
    myRenderingLayer->unregisterElement(&myStatsText);
    myRenderingLayer->unregisterElement(&myDescriptionText);
    myFinishedButton.unregisterElements(myRenderingLayer);
    myFinishedButton.getMouseSentinel()->removeFromMouseSentinelChain();
}



//------------------------------------------------------------------------------------------------
// Name: pollFinishedButton
// Desc: If the finished button was pushed, this method returns true.  Subsequent calls will return false
//       until the user pushes the finished button again.
//------------------------------------------------------------------------------------------------
bool VolucrisGUIStatDisplay::pollFinishedButton()
{
    bool pushed = myFinishedButton.wasPushed();
    if (pushed) myFinishedButton.resetPushed();
    return pushed;
}





//------------------------------------------------------------------------------------------------
// Name: pollAddAbilityPoint
// Desc: If the user clicks a button to add an ability point then this will
//       return 'true' and set 'ability' to the index of that stat
//------------------------------------------------------------------------------------------------
bool VolucrisGUIStatDisplay::pollAddAbilityPoint(int* ability)
{
    for (int i = 0; i < 5; ++i)
    {
        if (myAbilityButtons[i].wasPushed())
        {
            *ability = i;
            myAbilityButtons[i].resetPushed();
            return true;
        }
    }
    return false;
}


void VolucrisGUIStatDisplay::updateUserEvents() {
  int ability;
  if (pollAddAbilityPoint(&ability)) {
    UserEventAddAbilityPoint* add_ability_point = user_action_queue_->addAttributesAddAbilityPoint();
    add_ability_point->ability = ability;
  }
  if (pollFinishedButton()) {
    user_action_queue_->addCloseStatDisplay();
  }
}



//------------------------------------------------------------------------------------------------
// Name: zero
// Desc: Resets this class
//------------------------------------------------------------------------------------------------
void VolucrisGUIStatDisplay::zero()
{
    myStatPointsAreAvailableToBeAssigned = false;

    myRenderingLayer = 0;
    myParentMouseSentinel = 0;

    myAvatarPanel.setText("Avatar Stats");
    myAvatarPanel.getCanvas()->absoluteWidth(600);
    myAvatarPanel.getCanvas()->absoluteHeight(400);
    myAvatarPanel.getCanvas()->alignXCenter(0);
    myAvatarPanel.getCanvas()->alignYCenter(0);
    myAvatarPanel.getCanvas()->absoluteDepth(0.5f);

    for (int i = 0; i < 5; ++ i)
    {
        myAbilityTextCanvas[i].absoluteWidth(150);
        myAbilityTextCanvas[i].absoluteHeight(35);
        myAbilityTextCanvas[i].alignXLeft(130);
        myAbilityTextCanvas[i].alignYTop(40 + 45 * i);
        myAbilityTextCanvas[i].relativeDepth(-0.1f);
        myAbilityTextCanvas[i].setSourceCanvas(myAvatarPanel.getCanvas());

        myAbilityText[i].setSourceCanvas(&myAbilityTextCanvas[i]);
        myAbilityText[i].setAlignment(GUIALIGN_LEFT);

        myAbilityButtons[i].setText("+");
        myAbilityButtons[i].getCanvas()->absoluteWidth(50);
        myAbilityButtons[i].getCanvas()->absoluteHeight(35);
        myAbilityButtons[i].getCanvas()->alignXLeft(20);
        myAbilityButtons[i].getCanvas()->alignYTop(35+45*i);
        myAbilityButtons[i].getCanvas()->relativeDepth(-0.1f);
        myAbilityButtons[i].getCanvas()->setSourceCanvas(myAvatarPanel.getCanvas());
    }

    myStatsTextCanvas.absoluteWidth(300);
    myStatsTextCanvas.relativeHeight(-30);
    myStatsTextCanvas.alignXRight(-15);
    myStatsTextCanvas.alignYTop(40);
    myStatsTextCanvas.relativeDepth(-0.01f);
    myStatsTextCanvas.setSourceCanvas(myAvatarPanel.getCanvas());

    myStatsText.setSourceCanvas(&myStatsTextCanvas);
    myStatsText.setTextColor(D3DCOLOR_XRGB(0,0,0));
    myStatsText.setAlignment(GUIALIGN_LEFT);

    myDescriptionTextCanvas.relativeWidth(-60);
    myDescriptionTextCanvas.absoluteHeight(70);
    myDescriptionTextCanvas.alignYBottom(-60);
    myDescriptionTextCanvas.alignXCenter(0);
    myDescriptionTextCanvas.relativeDepth(-0.1f);
    myDescriptionTextCanvas.setSourceCanvas(myAvatarPanel.getCanvas());

    myDescriptionText.setSourceCanvas(&myDescriptionTextCanvas);
    myDescriptionText.setTextColor(D3DCOLOR_XRGB(0,0,0));
    myDescriptionText.setAlignment(GUIALIGN_LEFT);

    myFinishedButton.setText("Finished");
    myFinishedButton.getCanvas()->absoluteWidth(200);
    myFinishedButton.getCanvas()->absoluteHeight(35);
    myFinishedButton.getCanvas()->alignXCenter(0);
    myFinishedButton.getCanvas()->alignYBottom(-20);
    myFinishedButton.getCanvas()->relativeDepth(-0.1f);
    myFinishedButton.getCanvas()->setSourceCanvas(myAvatarPanel.getCanvas());
}

