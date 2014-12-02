//------------------------------------------------------------------------------------------------
// File:  guimouseinputsource.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIMOUSEINPUTSOURCE_H__
#define __GUIMOUSEINPUTSOURCE_H__


#include "guipoint.h"

/**
 * Defines the base class for an input source that provides keyboard input
 */
class GUIMouseInputSource
{
  public:

    /**
     * Initializes this class
     */
    GUIMouseInputSource();

    /**
     * Determines whether or not the primary button is down
     */
    bool isPrimaryButtonDown() const;

    /**
     * Determines whether or not the alternate button is down
     */
    bool isAlternateButtonDown() const;

    /**
     * Gets the mouse's current position
     */
    GUIPoint getMousePosition() const;

    /**
     * Gets the primary sentinel to which messages are passed.  All elements of
     * the mouse input chain should be children (at some point in the hierarchy)
     * of this sentinel.
     */
    static GUIMouseSentinel* getPrimarySentinel();

    /**
     * Forces this input source to propogate a mouse-movement message with the
     * current position as both the old and new locations.  This is used to
     * update the GUI when changes to dynamic components could cause the mouse
     * location to become out of sync with the GUI's display.
     */
    void generateMovementUpdate();

    /**
     * Forces the mouse to move to the given location.
     */
    //virtual void jumpTo(GUIPoint point) = 0;

    // Forces a primary-button mouse click to propagate
    void click();


  protected:

    /**
     * Called by the derived class to update the mouse's location on the screen
     */
    void mouseMovedTo(GUIPoint position);

    /**
     * Called by the derived class to update the mouse with action
     */
    void mouseButtonAction(GUIMouseButtonAction action);

    /**
     * Called when the mouse moves.  'amount' is the value / value per line
     */
    void mouseScroll(float amount);


  private:

    /// This sentinel is the root for all of the keyboard sentinels
    static GUIMouseSentinel myPrimarySentinel;

    /// The mouse's position
    GUIPoint myCurrentPosition;

    /// The mouse's button states
    bool myPrimaryButtonDown, myAlternateButtonDown;

    /// Once this reaches +/- 1.0f, it sends a "scroll line" message
    float myScrollWheelAccumulator;
};



#endif