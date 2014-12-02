//------------------------------------------------------------------------------------------------
// File:    volucrisguibutton.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"





//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIButton
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUIButton::VolucrisGUIButton() {
  myBackgroundCanvas.setSourceCanvas(getCanvas());
  myBackgroundCanvas.alignXCenter(0);
  myBackgroundCanvas.alignYCenter(0);
  myBackgroundCanvas.relativeWidth(0);
  myBackgroundCanvas.relativeHeight(0);
  myBackgroundCanvas.relativeDepth(+0.01f);

  myText.setClippingArea(myBackgroundCanvas.getArea());

  myBackground.setSourceRegion(&VolucrisGUIDefs::BUTTON_BACKGROUND_NEUTRAL,
                                VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
                                VolucrisGUIDefs::TEXTURE_SIZE);
  myBackground.setSourceCanvas(&myBackgroundCanvas);


  onStateChange(GUIButton::STATECHANGE_MOUSEOVER);
}




//------------------------------------------------------------------------------------------------
// Name: setFont
// Desc: Changes the font this button will use to render its text
//------------------------------------------------------------------------------------------------
void VolucrisGUIButton::setFont(GUIFont* font)
{
    myText.setFont(font);
}



//------------------------------------------------------------------------------------------------
// Name: setText
// Desc: Sets that this button will display
//------------------------------------------------------------------------------------------------
void VolucrisGUIButton::setText(const char* text)
{
    myText.setText(text ? text : "");
    myText.setTextColor(VolucrisGUIDefs::BUTTON_COLOR_NEUTRAL);
    onStateChange(GUIButton::STATECHANGE_ENABLED);
}



//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers this button's elements with the layer so they can be drawn
//------------------------------------------------------------------------------------------------
void VolucrisGUIButton::registerElements(GUILayer* layer)
{
    myBackground.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    onStateChange(GUIButton::STATECHANGE_ENABLED);
    layer->registerElement(&myBackground);
    GUIButton::registerElements(layer);
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes this button's elements from the layer so they aren't drawn anymore
//------------------------------------------------------------------------------------------------
void VolucrisGUIButton::unregisterElements(GUILayer* layer)
{
    layer->unregisterElement(&myBackground);
    GUIButton::unregisterElements(layer);
}

void VolucrisGUIButton::setClippingArea(const GUIRect* area) {
  myBackground.setClippingArea(area);
  GUIButton::setClippingArea(area);
}


//------------------------------------------------------------------------------------------------
// Name: onStateChange
// Desc: 
//------------------------------------------------------------------------------------------------
void VolucrisGUIButton::onStateChange(GUIButton::StateChange stateChange) {
  switch(stateChange) {
      case GUIButton::STATECHANGE_RESET: {
        getMouseSentinel()->releaseMouseFocus();
        myText.setTextColor(VolucrisGUIDefs::BUTTON_COLOR_NEUTRAL);
        myBackground.setSourceRegion(
           &VolucrisGUIDefs::BUTTON_BACKGROUND_NEUTRAL,
            VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
            VolucrisGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_DISABLED: {
        myText.setTextColor(VolucrisGUIDefs::BUTTON_COLOR_DISABLED);
        myBackground.setSourceRegion(
           &VolucrisGUIDefs::BUTTON_BACKGROUND_NEUTRAL,
            VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
            VolucrisGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_ENABLED:
        myText.setTextColor(VolucrisGUIDefs::BUTTON_COLOR_NEUTRAL);

      case GUIButton::STATECHANGE_MOUSEOUT: {
        myBackground.setSourceRegion(
           &VolucrisGUIDefs::BUTTON_BACKGROUND_NEUTRAL,
            VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
            VolucrisGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_MOUSEOVER: {
        myBackground.setSourceRegion(&VolucrisGUIDefs::BUTTON_BACKGROUND_HOVER,
                                       VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
                                       VolucrisGUIDefs::TEXTURE_SIZE);

      } break;

      case GUIButton::STATECHANGE_MOUSEOVERANDDOWN: {
        myBackground.setSourceRegion(&VolucrisGUIDefs::BUTTON_BACKGROUND_MOUSEDOWN,
                                      VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
                                      VolucrisGUIDefs::TEXTURE_SIZE);

      } break;
    }
}

