//------------------------------------------------------------------------------------------------
// File:    volucrisguistatdisplay.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUISTATDISPLAY_H__
#define __VOLUCRISGUISTATDISPLAY_H__


class UserEventQueue;

#include "volucrisguititledpanel.h"

/**
 * This class is used to show the user their avatar's stats and allow them to be edited
 */
class VolucrisGUIStatDisplay
{
  class KeyboardSentinel : public GUIKeyboardSentinel {
  public:
    KeyboardSentinel(VolucrisGUIStatDisplay* owner);
    void enable(bool enabled);
    bool isEnabled() { return enabled_; }
    virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);

  private:
    VolucrisGUIStatDisplay* owner_;
    bool enabled_;
  };

    public:

        /**
         * Initializes this class
         */
        VolucrisGUIStatDisplay();

        /**
         * Initializes this display of this stat display on the target rendering layer.  This method
         * doesn't draw anything to the layer until the display method is invoked.
         */
        bool create(GUIFont* windowFont, GUIFont* titleFont, GUILayer* layer,
                    GUIMouseSentinel* parentMouseSentinel, GUIKeyboardSentinel* parentKeyboardSentinel,
                    UserEventQueue* user_action_queue);

        /**
         * Erases all of the members of this screen, and removes its components from the
         * display layer.
         */
        void destroy();

        /**
         * Sets up this display to show the given avatar
         */
        void update(unsigned char level, unsigned char abilities[5], unsigned char availableAbilityPoints,
                    int hp, int hpMax, int mp, int mpMax, int exp, int nextLevelExp, int defense);

        /**
         * Displays the stat screen.  If it hasn't been updated, it will show invalid data.
         */
        void show();

        /**
         * Removes all of this display's items from the screen and mouse/keyboard input chain
         */
        void hideDisplay();

        /**
         * If the finished button was pushed, this method returns true.  Subsequent calls will return false
         * until the user pushes the finished button again.
         */
        bool pollFinishedButton();

        /**
         * If the user clicks a button to add an ability point then this will
         * return 'true' and set 'ability' to the index of that stat
         */
        bool pollAddAbilityPoint(int* ability);

        void updateUserEvents();


    protected:

        /**
         * Resets this class
         */
        void zero();


    protected:

        /// The target layer for this display
        GUILayer* myRenderingLayer;

        /// The owner mouse sentinel for this display
        GUIMouseSentinel* myParentMouseSentinel;

        /// The panel where the avatar stats are shown
        VolucrisGUITitledPanel myAvatarPanel;

        /// When the user presses this button, they want to leave the screen
        VolucrisGUIButton myFinishedButton;

        /// If there are points available, buttons are shown for the stats instead of text
        GUICanvas myAbilityTextCanvas[5];
        GUITextLabelElement myAbilityText[5];
        VolucrisGUIButton myAbilityButtons[5];

        /// Player stats (level/hp/mp/exp)
        GUICanvas myStatsTextCanvas;
        GUITextLabelElement myStatsText;

        /// Text declaring misc. information about the character
        GUICanvas myDescriptionTextCanvas;
        GUITextLabelElement myDescriptionText;

        /// Whether or not to prompt the user to assign stat points
        bool myStatPointsAreAvailableToBeAssigned;

        UserEventQueue* user_action_queue_;
        KeyboardSentinel keyboard_sentinel_;
};


#endif