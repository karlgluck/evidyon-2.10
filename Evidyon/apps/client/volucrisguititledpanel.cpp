//------------------------------------------------------------------------------------------------
// File:    volucrisguititledpanel.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"



//------------------------------------------------------------------------------------------------
// Name: VolucrisGUITitledPanel
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUITitledPanel::VolucrisGUITitledPanel()
{/*
    myTitleCanvas.setSourceCanvas(&myPanelCanvas);
    myTitleCanvas.relativeWidth(-64);
    myTitleCanvas.absoluteHeight(32);
    myTitleCanvas.alignXCenter(0);
    myTitleCanvas.alignYTop(-16);
    myTitleCanvas.relativeDepth(-0.01f);*/
    myTitleCanvas.setSourceCanvas(&myPanelCanvas);
    myTitleCanvas.relativeWidth(0);
    myTitleCanvas.absoluteHeight(32);
    myTitleCanvas.alignXCenter(0);
    myTitleCanvas.alignYTopOutside(0);
    myTitleCanvas.relativeDepth(0.0f);

    {
        myTitleBackground.setSourceCanvas(&myTitleCanvas);
        myTitleBackground.setSourceRegion(&VolucrisGUIDefs::WINDOW_TITLE_BACKGROUND,
                                           VolucrisGUIDefs::WINDOW_TITLE_BACKGROUND_BORDER,
                                           VolucrisGUIDefs::TEXTURE_SIZE);
    }

    {
        myPanelBackground.setSourceCanvas(&myPanelCanvas);
        myPanelBackground.setSourceRegion(&VolucrisGUIDefs::WINDOW_BACKGROUND,
                                            VolucrisGUIDefs::WINDOW_BACKGROUND_BORDER,
                                            VolucrisGUIDefs::TEXTURE_SIZE);
    }

    {
        myTitleText.setSourceCanvas(&myTitleCanvas);
        myTitleText.setTextColor(VolucrisGUIDefs::WINDOW_TITLE_TEXT_COLOR);
    }

    {
        myTitleSideIcons.setSourceCanvas(&myTitleCanvas);
        GUIRect area;
        GUIPoint point = { 32, 5*16 };
        GUISize size = {16,16};
        area.set(point, size);
        myTitleSideIcons.setSourceRegion(&area, VolucrisGUIDefs::TEXTURE_SIZE);
    }
}





//------------------------------------------------------------------------------------------------
// Name: setFont
// Desc: Changes the font this panel will use to render its text
//------------------------------------------------------------------------------------------------
void VolucrisGUITitledPanel::setFont(GUIFont* font)
{
    myTitleText.setFont(font);
}



//------------------------------------------------------------------------------------------------
// Name: setText
// Desc: Sets the label for this panel
//------------------------------------------------------------------------------------------------
void VolucrisGUITitledPanel::setText(const char* text)
{
    myTitleText.setText(text);
}



//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers this panel's visible elements with the given layer
//------------------------------------------------------------------------------------------------
void VolucrisGUITitledPanel::registerElements(GUILayer* layer)
{
    myPanelBackground.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    myTitleBackground.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    myTitleSideIcons.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    layer->registerElement(&myPanelBackground);
    layer->registerElement(&myTitleBackground);
    layer->registerElement(&myTitleText);
    //layer->registerElement(&myTitleSideIcons);
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Unregisters this panel's visible elements
//------------------------------------------------------------------------------------------------
void VolucrisGUITitledPanel::unregisterElements(GUILayer* layer)
{
    layer->unregisterElement(&myPanelBackground);
    layer->unregisterElement(&myTitleBackground);
    layer->unregisterElement(&myTitleText);
    layer->unregisterElement(&myTitleSideIcons);
}



//------------------------------------------------------------------------------------------------
// Name: getCanvas
// Desc: Gets the canvas for this button so that it can be positioned
//------------------------------------------------------------------------------------------------
GUICanvas* VolucrisGUITitledPanel::getCanvas()
{
    return &myPanelCanvas;
}

