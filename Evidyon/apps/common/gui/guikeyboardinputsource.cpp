//------------------------------------------------------------------------------------------------
// File:  guikeyboardinputsource.cpp
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "guikeyboard.h"



GUIKeyboardSentinel GUIKeyboardInputSource::myPrimarySentinel;


//------------------------------------------------------------------------------------------------
// Name: GUIKeyboardInputSource
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
GUIKeyboardInputSource::GUIKeyboardInputSource()
{
  for (int i = 0; i < (int)GUIKEY_COUNT; ++i)
    myKeyDown[i] = false;
}


//------------------------------------------------------------------------------------------------
// Name: getPrimarySentinel
// Desc: Gets the primary sentinel to which messages are passed
//------------------------------------------------------------------------------------------------
GUIKeyboardSentinel* GUIKeyboardInputSource::getPrimarySentinel()
{
  return &myPrimarySentinel;
}




//------------------------------------------------------------------------------------------------
// Name: isKeyDown
// Desc: Determines whether or not the indicated key is pressed
//------------------------------------------------------------------------------------------------
bool GUIKeyboardInputSource::isKeyDown(GUIKey key) const
{
  return myKeyDown[key];
}



//------------------------------------------------------------------------------------------------
// Name: keyMoved
// Desc: This method is used by the derived class that implements this input source to
//     notify that the given key was pressed (down == true) or released (down == false)
//------------------------------------------------------------------------------------------------
void GUIKeyboardInputSource::keyMoved(GUIKey key, bool down)
{
  // Don't change key states if it's unnecessary
  if (myKeyDown[key] == down) return;

  // Set the key state
  myKeyDown[key] = down;

  // Notify the sentinels of the change in key state
  if (down)
    myPrimarySentinel.onKeyDown(key, this);
  else
    myPrimarySentinel.onKeyUp(key, this);
}



//------------------------------------------------------------------------------------------------
// Name: typedCharacter
// Desc: Used by the derived class to notify that the key for the designated character
//     was pressed and released ("typed")
//------------------------------------------------------------------------------------------------
void GUIKeyboardInputSource::typedCharacter(char character)
{
  // Notify the sentinels that the character was typed
  myPrimarySentinel.onTypedCharacter(character, this);
}

