//------------------------------------------------------------------------------------------------
// File:  win32buttonmanager.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __WIN32BUTTONMANAGER_H__
#define __WIN32BUTTONMANAGER_H__





/**
 * Manages a list of buttons
 */
class Win32ButtonManager
{
  /// Define the button map type
  typedef std::map<int,RECT> ButtonMap;


  public:

    /**
     * Initializes the class
     */
    Win32ButtonManager();

    /**
     * Updates the manager to the current status of the mouse & keyboard
     */
    void poll(dcxWin32Mouse* mouse, dcxWin32Keyboard* keyboard);

    /**
     * Adds a new button to the manager on an 800x600 screen.  Regardless of the
     * screen's actual size, all interaction with the buttons will be performed as if
     * the mouse were on an 800x600 screen.  If the ID number is negative, the button
     * WILL NOT be added, and this method will fail silently.
     */
    void addOn800x600Screen(int id, bool isDefault, int x1, int y1, int x2, int y2);

    /**
     * Clears all of the buttons from the screen
     */
    void clear();

    /**
     * Finds out which button, if any, were clicked as of the last poll.  In cases
     * of conflict (overlapping buttons that would both be clicked) the lowest
     * ID of a button is returned.
     */
    bool queryClick(int* buttonID);

    /**
     * Finds out which button, if any, the mouse is over.  This can only return a
     * single button.  In cases of conflict, the button with the lowest ID is
     * returned.
     */
    bool queryMouseOver(int* buttonID, bool* mouseDown);
    
    /**
     * Removes the button with the given ID
     */
    void remove(int id);


  // TODO: UNIMPLEMENTED METHODS THAT MIGHT BE WORTH IMPLEMENTING IN THE FUTURE
  protected:

    /**
     * Finds out whether or not the button with the given ID was clicked
     */
    bool clicked(int buttonID);


  protected:

    /// The button that was clicked during the last call to 'poll'
    int myPollButtonClicked;

    /// The button that the mouse is over
    int myPollButtonMouseOver;

    /// Whether or not the mouse was down over the button (only valid if
    /// myPollButtonMouseOver is not -1
    bool myPollButtonMouseDown;

    /// This button is triggered when the user hits the "enter" key
    int myDefaultButtonID;
    
    /// This is the list of all of the buttons currently being managed
    ButtonMap myButtons;
};

#endif