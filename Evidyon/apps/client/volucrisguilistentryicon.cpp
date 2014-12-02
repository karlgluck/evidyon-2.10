//------------------------------------------------------------------------------------------------
// File:    volucrisguilistentryicon.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"



//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIListEntryIcon
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUIListEntryIcon::VolucrisGUIListEntryIcon(GUIList::ListEntry* owner)
{
    myStatusIconCanvas.alignXLeft(8);
    myStatusIconCanvas.alignYCenter(0);
    myStatusIconCanvas.absoluteWidth(16);
    myStatusIconCanvas.absoluteHeight(16);
    myStatusIconCanvas.relativeDepth(-0.01f);
    owner->addChildCanvas(&myStatusIconCanvas);
    myStatusIcon.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    setType(NONE);
}



//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Adds this list item's elements to the rendering process
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryIcon::registerElements(GUILayer* layer)
{
    layer->registerElement(&myStatusIcon);
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes this list item's elements from the screen
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryIcon::unregisterElements(GUILayer* layer)
{
    layer->unregisterElement(&myStatusIcon);
}



//------------------------------------------------------------------------------------------------
// Name: setType
// Desc: Changes the type of icon to display
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryIcon::setType(IconType type)
{
    unsigned int typeIndex = (unsigned int)type;
    GUIPoint point = { 32, 32 + 16 * typeIndex };
    GUISize size = { 16, 16 };
    GUIRect sourceArea;
    sourceArea.set(point, size);
    myStatusIcon.setSourceRegion(&sourceArea, VolucrisGUIDefs::TEXTURE_SIZE);
}



//------------------------------------------------------------------------------------------------
// Name: setClippingArea
// Desc: Sets the clipping region of the internal elements
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryIcon::setClippingArea(const GUIRect* clippingArea)
{
    myStatusIcon.setClippingArea(clippingArea);
}

