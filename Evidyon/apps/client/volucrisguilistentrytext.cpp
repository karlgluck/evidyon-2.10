//------------------------------------------------------------------------------------------------
// File:    volucrisguilistentrytext.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"


//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIListEntryText
// Desc: Initializes the class
//------------------------------------------------------------------------------------------------
VolucrisGUIListEntryText::VolucrisGUIListEntryText(GUIList::ListEntry* owner)
{
    myTextLabelCanvas.alignXLeft(0);
    myTextLabelCanvas.alignYTop(0);
    myTextLabelCanvas.relativeHeight(0);
    myTextLabelCanvas.relativeWidth(0);
    myTextLabelCanvas.relativeDepth(-0.01f);

    myTextLabelElement.setSourceCanvas(&myTextLabelCanvas);

    owner->addChildCanvas(&myTextLabelCanvas);
}



//------------------------------------------------------------------------------------------------
// Name: getCanvas
// Desc: Obtains the canvas for this element
//------------------------------------------------------------------------------------------------
GUICanvas* VolucrisGUIListEntryText::getCanvas()
{
    return &myTextLabelCanvas;
}



//------------------------------------------------------------------------------------------------
// Name: setFont
// Desc: Changes the font used to render this text.  The font must remain
//       valid.
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryText::setFont(GUIFont* font)
{
    myTextLabelElement.setFont(font);
}



//------------------------------------------------------------------------------------------------
// Name: setText
// Desc: Changes the displayed text
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryText::setText(const std::string& text)
{
    myTextLabelElement.setText(text);
}



//------------------------------------------------------------------------------------------------
// Name: setTextColor
// Desc: Changes this text's color
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryText::setTextColor(D3DCOLOR color)
{
    myTextLabelElement.setTextColor(color);
}



//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers the visible elements with the screen
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryText::registerElements(GUILayer* layer)
{
    layer->registerElement(&myTextLabelElement);
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes elements from the given layer 
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryText::unregisterElements(GUILayer* layer)
{
    layer->unregisterElement(&myTextLabelElement);
}





//------------------------------------------------------------------------------------------------
// Name: setClippingArea
// Desc: Sets the clipping region of the internal elements
//------------------------------------------------------------------------------------------------
void VolucrisGUIListEntryText::setClippingArea(const GUIRect* clippingArea)
{
    myTextLabelElement.setClippingArea(clippingArea);
}

