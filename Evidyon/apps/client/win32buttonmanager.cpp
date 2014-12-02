//------------------------------------------------------------------------------------------------
// File:    win32buttonmanager.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"




//------------------------------------------------------------------------------------------------
// Name:  Win32ButtonManager
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
Win32ButtonManager::Win32ButtonManager()
{
    myPollButtonClicked = -1;
    myPollButtonMouseOver = -1;
    myPollButtonMouseDown = false;
    myDefaultButtonID = -1;
}




//------------------------------------------------------------------------------------------------
// Name:  poll
// Desc:  Updates the manager to the current status of the mouse & keyboard
//------------------------------------------------------------------------------------------------
void Win32ButtonManager::poll(dcxWin32Mouse* mouse, dcxWin32Keyboard* keyboard)
{
    // Make sure the parameters are valid
    if (APP_ERROR(!mouse || !keyboard)("Invalid parameter to Win32ButtonManager::poll"))
        return;

    // Reset the button states
    myPollButtonClicked = -1;
    myPollButtonMouseOver = -1;

    // Check to see if the default button was activated
    if ((myDefaultButtonID != -1) && keyboard->keyPressed(KEY_RETURN))
        myPollButtonClicked = myDefaultButtonID;
    else
    {
        // Mouse click coordinates
        POINTS down, up;

        // The default button wasn't activated by the keyboard, so check each
        // of the buttons in the list to see if the mouse clicked them.  First,
        // get the click coordinates of the mouse
        if (mouse->getScaledClick(dcxWin32Mouse::BUTTON_LEFT, &down, &up))
        {
            // Convert the POINTS structure to POINT
            POINT ptDown = { down.x, down.y },
                  ptUp   = {   up.x,   up.y };

            // Check each of the buttons
            for (ButtonMap::const_iterator i = myButtons.begin(); i != myButtons.end(); ++i)
            {
                if (PtInRect(&(i->second), ptDown) && PtInRect(&(i->second), ptUp))
                {
                    // Save the ID number of the button
                    myPollButtonClicked = i->first;

                    // Exit the function
                    return;
                }
            }
        }
    }

    // Get the mouse's location on an 800x600 screen
    POINT ptMouse;
    {
        POINTS ptsMouse;
        mouse->getCoordinates(0, &ptsMouse);
        ptMouse.x = ptsMouse.x;
        ptMouse.y = ptsMouse.y;
    }

    // Find out if the mouse is over a button
    for (ButtonMap::const_iterator i = myButtons.begin(); i != myButtons.end(); ++i)
    {
        if (PtInRect(&(i->second), ptMouse))
        {
            myPollButtonMouseOver = i->first;
            myPollButtonMouseDown = mouse->isButtonDown(dcxWin32Mouse::BUTTON_LEFT);
            break;
        }
    }
}




//------------------------------------------------------------------------------------------------
// Name:  addOn800x600Screen
// Desc:  Adds a new button to the manager on an 800x600 screen
//------------------------------------------------------------------------------------------------
void Win32ButtonManager::addOn800x600Screen(int id, bool isDefault, int x1, int y1, int x2, int y2)
{
    // Make sure the ID is valid
    if (APP_ERROR(id < 0)("Invalid parameter to addOn800x600Screen")) return;

    // Add this button to the list
    {
        // Build a rectangle to contain the button
        RECT rc;
        SetRect(&rc, x1, y1, x2, y2);

        // Add to the list of buttons
        myButtons.insert(ButtonMap::value_type(id, rc));
    }
}




//------------------------------------------------------------------------------------------------
// Name:  clear
// Desc:  Clears all of the buttons from the screen
//------------------------------------------------------------------------------------------------
void Win32ButtonManager::clear()
{
    myPollButtonClicked = -1;
    myPollButtonMouseOver = -1;
    myPollButtonMouseDown = false;
    myDefaultButtonID = -1;
    myButtons.clear();
}




//------------------------------------------------------------------------------------------------
// Name:  query
// Desc:  Finds out which button, if any, were clicked as of the last poll
//------------------------------------------------------------------------------------------------
bool Win32ButtonManager::queryClick(int* buttonID)
{
    // Make sure the output is OK
    if (APP_ERROR(!buttonID)("Invalid parameter to queryClick"))
        return false;

    // Save the button ID
    *buttonID = myPollButtonClicked;

    // Return whether or not a button was clicked
    return myPollButtonClicked != -1;
}




//------------------------------------------------------------------------------------------------
// Name:  queryMouseOver
// Desc:  Finds out which button, if any, the mouse is over
//------------------------------------------------------------------------------------------------
bool Win32ButtonManager::queryMouseOver(int* buttonID, bool* mouseDown)
{
    // Make sure the outputs are OK
    if (APP_ERROR(!buttonID || !mouseDown)("Invalid parameter to queryMouseOver"))
        return false;

    // Store the results
    *buttonID = myPollButtonMouseOver;
    *mouseDown = myPollButtonMouseDown;

    // Return whether or not the mouse is over a button
    return myPollButtonMouseOver != -1;
}



//------------------------------------------------------------------------------------------------
// Name:  remove
// Desc:  Removes the button with the given ID
//------------------------------------------------------------------------------------------------
void Win32ButtonManager::remove(int id)
{
    myButtons.erase(id);
}




//------------------------------------------------------------------------------------------------
// Name:  clicked
// Desc:  Finds out whether or not the button with the given ID was clicked
//------------------------------------------------------------------------------------------------
/*bool Win32ButtonManager::clicked(int buttonID)
{
}*/
