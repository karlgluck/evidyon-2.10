//------------------------------------------------------------------------------------------------
// File:    volucrisguilistentrybackground.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"


//------------------------------------------------------------------------------------------------
// Name: ListEntryBackground
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUIListEntryBackground::VolucrisGUIListEntryBackground(GUIList::ListEntry* owner)
{
    myBackgroundCanvas.alignXLeft(0);
    myBackgroundCanvas.alignYTop(0);
    myBackgroundCanvas.relativeHeight(0);
    myBackgroundCanvas.relativeWidth(0);
    myBackgroundCanvas.relativeDepth(-0.01f);

    myBackground.setSourceCanvas(&myBackgroundCanvas);

    owner->addChildCanvas(&myBackgroundCanvas);
}




//------------------------------------------------------------------------------------------------
// Name: getCanvas
// Desc: Obtains the canvas for this element
//------------------------------------------------------------------------------------------------
GUICanvas* VolucrisGUIListEntryBackground::getCanvas()
{
    return &myBackgroundCanvas;
}



//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Adds this list item's elements to the rendering process
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryBackground::registerElements(GUILayer* layer)
{
    myBackground.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    setType(LIGHT);
    layer->registerElement(&myBackground);
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes this list item's elements from the screen
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryBackground::unregisterElements(GUILayer* layer)
{
    layer->unregisterElement(&myBackground);
}



//------------------------------------------------------------------------------------------------
// Name: setType
// Desc: Changes the type of background to display
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryBackground::setType(BackgroundType type)
{
    unsigned int typeIndex = (unsigned int)type;
    const GUIRect* source;
    /*
    GUIPoint pt = { 3*32, typeIndex*32 + 32 };
    GUISize size = { 32, 32 };
    GUIRect area;
    area.set(pt, size);*/
    switch (type) {
      case LIGHT: source = &VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_SELECTED; break;
      case MEDIUM: source = &VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_MOUSEOVER; break;
      case DARK: source = &VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_NEUTRAL; break;
    }
    myBackground.setSourceRegion(source, 4, VolucrisGUIDefs::TEXTURE_SIZE);
}



//------------------------------------------------------------------------------------------------
// Name: setClippingArea
// Desc: Sets the clipping region of the internal elements
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryBackground::setClippingArea(const GUIRect* clippingArea)
{
    myBackground.setClippingArea(clippingArea);
}

