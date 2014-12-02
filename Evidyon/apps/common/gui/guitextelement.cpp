//------------------------------------------------------------------------------------------------
// File:  guitextelement.cpp
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
#include "guielement.h"
#include "guivertex.h"
#include "guiindex.h"
#include "guilayer.h"
#include "guifont.h"
#include "guitextelement.h"



//------------------------------------------------------------------------------------------------
// Name: render
// Desc: Renders this text list into the given layer.
//------------------------------------------------------------------------------------------------
bool GUITextElement::render(GUILayer* layer)
{
  // Get the font with which to render
  GUIFont* font = getFont();
  if (!font) return true;

  // Initialize the subclass
  getNextTextSegment(0);

  // Grab all of the text segment information
  TextSegmentInfo nextSegmentInfo;
  ZeroMemory(&nextSegmentInfo, sizeof(nextSegmentInfo));
  while(getNextTextSegment(&nextSegmentInfo))
  {
    // Draw this text
    if (!font->renderText(
        layer,
        myClippingArea,
        nextSegmentInfo.x, nextSegmentInfo.y,
        mySourceCanvas ? mySourceCanvas->getDepth() - 0.001f : 0.001f, // TODO: epsilon?
        nextSegmentInfo.text,
        nextSegmentInfo.lineLength,
        nextSegmentInfo.color,
        nextSegmentInfo.obscuringCharacter
       ))
      return false;

    // Move the current y-coordinate
    if (nextSegmentInfo.linesToIncrementAfterward)
    {
      float pixelsToIncrement = font->getLineHeightInPixels() * nextSegmentInfo.linesToIncrementAfterward;
      nextSegmentInfo.y += pixelsToIncrement;
    }

    // Move the X-coordinate
    GUISize lastSegmentSize;
    font->getTextExtent(nextSegmentInfo.text, nextSegmentInfo.lineLength, &lastSegmentSize);
    nextSegmentInfo.x += lastSegmentSize.width;
  }

  // Success
  return true;
}


//------------------------------------------------------------------------------------------------
int GUITextElement::getTexture()
{
  GUIFont* font = getFont();
  if (font) return font->getTextureID();
  else return 0;
}
