//------------------------------------------------------------------------------------------------
// File:  guielement.cpp
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"
#include <d3dx9.h>
#include <map>
#include "guivertex.h"
#include "guiindex.h"
#include "guilayer.h"
#include "guielement.h"


//------------------------------------------------------------------------------------------------
// Name: GUIElement
// Desc: Initializes the class
//------------------------------------------------------------------------------------------------
GUIElement::GUIElement()
{
  mySourceCanvas = 0;
  myClippingArea = 0;
}



//------------------------------------------------------------------------------------------------
// Name: setSourceCanvas
// Desc: Sets the canvas that this class uses for position and size references
//------------------------------------------------------------------------------------------------
void GUIElement::setSourceCanvas(GUIGenericCanvas* sourceCanvas)
{
  mySourceCanvas = sourceCanvas;
}



//------------------------------------------------------------------------------------------------
// Name: setClippingArea
// Desc: Sets the canvas that this class uses for position and size references
//------------------------------------------------------------------------------------------------
void GUIElement::setClippingArea(const GUIRect* clippingArea)
{
  myClippingArea = clippingArea;
}


//------------------------------------------------------------------------------------------------
// Name: getClippingArea
// Desc: Gets the clipping area canvas
//------------------------------------------------------------------------------------------------
const GUIRect* GUIElement::getClippingArea()
{
  return myClippingArea;
}








//------------------------------------------------------------------------------------------------
// Name: advance
// Desc: Updates this element
//------------------------------------------------------------------------------------------------
void GUIElement::advance(double currentTime, double timeSinceLastUpdate)
{
}



//------------------------------------------------------------------------------------------------
// Name: render
// Desc: Renders this element into the given layer
//------------------------------------------------------------------------------------------------
bool GUIElement::render(GUILayer* layer)
{
  return true;
}

