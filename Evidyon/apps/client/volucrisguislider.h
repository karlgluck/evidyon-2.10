//------------------------------------------------------------------------------------------------
// File:    volucrisguislider.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUISLIDER_H__
#define __VOLUCRISGUISLIDER_H__



/**
 * This slider is used to scroll through the list
 */
class VolucrisGUISlider : public GUISlider
{
    enum HandleGraphic
    {
        NEUTRAL,
        MOUSEOVER,
        DRAGGING
    };

public:

    /**
     * Changes the element being used to draw the slider handle
     */
    void setHandleGraphic(HandleGraphic graphic);

    /**
     * Called when the slider's state changes relative to user input.  If
     * the state change is that the slider's position changed, this method
     * updates the location of the list.
     */
    virtual void onStateChange(StateChange stateChange);


    /**
     * Initializes this slider
     */
    VolucrisGUISlider();

    /**
     * Adds this sentinel to the given input chain
     */
    void setParentMouseSentinel(GUIMouseSentinel* parentSentinel);

    /**
     * Removes this sentin from the input chain
     */
    void removeFromMouseInputChain();


protected:

    /**
     * Called when the handle moves
     */
    virtual void onChangedPosition(float position) = 0;
};


#endif