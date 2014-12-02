//------------------------------------------------------------------------------------------------
// File:    volucrisguiitemdescriptionpanel.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"


//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIItemDescriptionPanel
// Desc: 
//------------------------------------------------------------------------------------------------
VolucrisGUIItemDescriptionPanel::VolucrisGUIItemDescriptionPanel()
{
    myItemDescriptionPanel.setText("~~~");

    myItemDescriptionTextCanvas.relativeWidth(-40);
    myItemDescriptionTextCanvas.relativeHeight(-40);
    myItemDescriptionTextCanvas.alignXCenter(0);
    myItemDescriptionTextCanvas.alignYCenter(0);
    myItemDescriptionTextCanvas.relativeDepth(-0.01f);
    myItemDescriptionTextCanvas.setSourceCanvas(myItemDescriptionPanel.getCanvas());

    myItemDescriptionText.setAlignment(GUIALIGN_LEFT_TOP);
    myItemDescriptionText.setSourceCanvas(&myItemDescriptionTextCanvas);
    myItemDescriptionText.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
}



//------------------------------------------------------------------------------------------------
// Name: getCanvas
// Desc: Gets the panel's canvas so that it can be positioned
//------------------------------------------------------------------------------------------------
GUICanvas* VolucrisGUIItemDescriptionPanel::getCanvas()
{
    return myItemDescriptionPanel.getCanvas();
}


//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers the visible elements with the screen
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemDescriptionPanel::registerElements(GUILayer* layer)
{
    layer->registerElement(&myItemDescriptionText);
    myItemDescriptionPanel.registerElements(layer);
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes elements from the given layer 
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemDescriptionPanel::unregisterElements(GUILayer* layer)
{
    layer->unregisterElement(&myItemDescriptionText);
    myItemDescriptionPanel.unregisterElements(layer);
}



//------------------------------------------------------------------------------------------------
// Name: setFonts
// Desc: Sets the GUI texture for this list
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemDescriptionPanel::setFonts(GUIFont* titleFont, GUIFont* descriptionFont)
{
    myItemDescriptionPanel.setFont(titleFont);
    myItemDescriptionText.setFont(descriptionFont);
}


//------------------------------------------------------------------------------------------------
// Name: display
// Desc: Changes the item that this description panel shows.  Passing 'null'
//       resets the panel to displaying nothing.
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemDescriptionPanel::display(const ClientItemDescription* description)
{
    if (description)
    {
        myItemDescriptionPanel.setText(description->name);
        myItemDescriptionText.setText(description->description);
    }
    else
    {
        myItemDescriptionPanel.setText("~~~");
        myItemDescriptionText.setText("");
    }
}

