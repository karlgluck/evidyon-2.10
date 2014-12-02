//------------------------------------------------------------------------------------------------
// File:    volucrisguislider.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"



//------------------------------------------------------------------------------------------------
// Name: VolucrisGUISlider
// Desc: Initializes this slider
//------------------------------------------------------------------------------------------------
VolucrisGUISlider::VolucrisGUISlider()
{
    getBackgroundElement()->setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    getHandleElement()->setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    setHandleGraphic(NEUTRAL);
}



//------------------------------------------------------------------------------------------------
// Name: setParentMouseSentinel
// Desc: Adds this sentinel to the given input chain
//------------------------------------------------------------------------------------------------
void VolucrisGUISlider::setParentMouseSentinel(GUIMouseSentinel* parentSentinel)
{
    getMouseSentinel()->setParentMouseSentinel(parentSentinel);
    getMouseSentinel()->obtainMouseFocus();
}



//------------------------------------------------------------------------------------------------
// Name: removeFromMouseInputChain
// Desc: Removes this sentin from the input chain
//------------------------------------------------------------------------------------------------
void VolucrisGUISlider::removeFromMouseInputChain()
{
    getMouseSentinel()->removeFromMouseSentinelChain();
}



//------------------------------------------------------------------------------------------------
// Name: setHandleGraphic
// Desc: Changes the element being used to draw the slider handle
//------------------------------------------------------------------------------------------------
void VolucrisGUISlider::setHandleGraphic(HandleGraphic graphic)
{
    // Make sure the background graphic is set
    {
        getBackgroundElement()->setSourceRegion(&VolucrisGUIDefs::SLIDER_BACKGROUND, 1, VolucrisGUIDefs::TEXTURE_SIZE);
    }

    // TODO: based on state
    {
        getHandleElement()->setSourceRegion(&VolucrisGUIDefs::SLIDER_HANDLE, VolucrisGUIDefs::TEXTURE_SIZE);
    }
}



//------------------------------------------------------------------------------------------------
// Name: onStateChange
// Desc: Called when the slider's state changes relative to user input.  If
//       the state change is that the slider's position changed, this method
//       updates the location of the list.
//------------------------------------------------------------------------------------------------
void VolucrisGUISlider::onStateChange(StateChange stateChange)
{
    switch(stateChange)
    {
        case STATECHANGE_MOUSEOVER_BACKGROUND:          break;
        case STATECHANGE_MOUSEOVERANDDOWN_HANDLE:       setHandleGraphic(DRAGGING);
        case STATECHANGE_MOUSEOVER_HANDLE:
        case STATECHANGE_MOUSEOVER_HANDLEANDBACKGROUND: setHandleGraphic(MOUSEOVER); break;
        case STATECHANGE_MOUSEOUT:                      setHandleGraphic(NEUTRAL);
        case STATECHANGE_HANDLEMOVED:
            onChangedPosition(calculateSliderPercentLocation());
            break;
    }
}

