//------------------------------------------------------------------------------------------------
// File:  guikeyboardinputsource.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIKEYBOARDINPUTSOURCE_H__
#define __GUIKEYBOARDINPUTSOURCE_H__


/**
 * Defines the base class for an input source that provides keyboard input
 */
class GUIKeyboardInputSource
{
  public:

    /**
     * Initializes this class
     */
    GUIKeyboardInputSource();

    /**
     * Gets the primary sentinel to which messages are passed.  All elements of
     * the keyboard input chain should be children (at some point in the hierarchy)
     * of this sentinel.
     */
    static GUIKeyboardSentinel* getPrimarySentinel();

    /**
     * Determines whether or not the indicated key is pressed
     */
    bool isKeyDown(GUIKey key) const;


  protected:

    /**
     * This method is used by the derived class that implements this input source to
     * notify that the given key was pressed (down == true) or released (down == false)
     */
    void keyMoved(GUIKey key, bool down);

    /**
     * Used by the derived class to notify that the key for the designated character
     * was pressed and released ("typed")
     */
    void typedCharacter(char character);


  private:

    /// This sentinel is the root for all of the keyboard sentinels
    static GUIKeyboardSentinel myPrimarySentinel;

    /// This is the state of each of the keys
    bool myKeyDown[GUIKEY_COUNT];
};



#endif