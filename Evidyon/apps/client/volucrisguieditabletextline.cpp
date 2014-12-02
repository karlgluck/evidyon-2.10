//------------------------------------------------------------------------------------------------
// File:    volucrisguieditabletextline.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"



//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIEditableTextLine
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUIEditableTextLine::VolucrisGUIEditableTextLine()
{
    myNumbersOnlyFlag = false;
    myTextLengthLimit = -1;

    myBackground.setSourceCanvas(getCanvas());

    myTextElement.setSourceCanvas(&myTextCanvas);
    myTextElement.setClippingArea(myTextCanvas.getArea());

    myTextCanvas.setSourceCanvas(getCanvas());
    myTextCanvas.alignXCenter(0);
    myTextCanvas.alignYCenter(0);
    myTextCanvas.relativeWidth(-16);
    myTextCanvas.relativeHeight(-8);
    myTextCanvas.relativeDepth(-0.001f);

    myBackground.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
}



//------------------------------------------------------------------------------------------------
// Name: configure
// Desc: Calls the methods to set many text line parameters all at once
//------------------------------------------------------------------------------------------------
void VolucrisGUIEditableTextLine::configure(GUIFont* font,
    bool isPassword, bool restrictToNumbers, D3DCOLOR textColor)
{
    configure(font, isPassword, restrictToNumbers, textColor, -1);
}



//------------------------------------------------------------------------------------------------
// Name: configure
// Desc: Calls the methods to set many text line parameters all at once
//------------------------------------------------------------------------------------------------
void VolucrisGUIEditableTextLine::configure(GUIFont* font,
    bool isPassword, bool restrictToNumbers, D3DCOLOR textColor, int textLengthLimit)
{
    // Superclass methods
    setFont(font);
    setPasswordFlag(isPassword);
    numbersOnly(restrictToNumbers);
    setTextColor(textColor);
    setTextLengthLimit(textLengthLimit);
}






//------------------------------------------------------------------------------------------------
// Name: textLengthLimit
// Desc: Sets the number of characters this line can accept
//------------------------------------------------------------------------------------------------
void VolucrisGUIEditableTextLine::setTextLengthLimit(int characters)
{
    if (characters < 0)
        myTextLengthLimit = -1;
    else
        myTextLengthLimit = characters;
}



//------------------------------------------------------------------------------------------------
// Name: removeTextLengthLimit
// Desc: Makes this text line accept any number of characters (default behavior)
//------------------------------------------------------------------------------------------------
void VolucrisGUIEditableTextLine::removeTextLengthLimit()
{
    setTextLengthLimit(-1);
}



//------------------------------------------------------------------------------------------------
// Name: numbersOnly
// Desc: Changes the flag that restricts input so that only numbers are accepted
//------------------------------------------------------------------------------------------------
void VolucrisGUIEditableTextLine::numbersOnly(bool restrictToNumbers)
{
    myNumbersOnlyFlag = restrictToNumbers;
}



//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers this component's visible elements with the given layer.  This method calls
//       the superclass method.
//------------------------------------------------------------------------------------------------
void VolucrisGUIEditableTextLine::registerElements(GUILayer* layer)
{
    GUIRect sourceArea;
    GUIPoint loc = { 0, 0 };
    GUISize size = { 32, 32 };
    sourceArea.set(loc, size);
    myBackground.setSourceRegion(&sourceArea, 8, VolucrisGUIDefs::TEXTURE_SIZE);

    layer->registerElement(&myBackground);
    GUIEditableTextLine::registerElements(layer);
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Unregisters this component's visible elements.  This method calls the superclass method.
//------------------------------------------------------------------------------------------------
void VolucrisGUIEditableTextLine::unregisterElements(GUILayer* layer)
{
    GUIEditableTextLine::unregisterElements(layer);
    layer->unregisterElement(&myBackground);
}


//------------------------------------------------------------------------------------------------
void VolucrisGUIEditableTextLine::setBackgroundColor(D3DCOLOR color) {
  myBackground.setColor(color);
}



//------------------------------------------------------------------------------------------------
// Name: onTypedPrintableCharacter
// Desc: Invoked when the user enters a valid character that should be added to the internal buffer
//------------------------------------------------------------------------------------------------
void VolucrisGUIEditableTextLine::onTypedPrintableCharacter(char printableCharacter)
{
    bool acceptCharacter = true;
    acceptCharacter = acceptCharacter && (myNumbersOnlyFlag ? (printableCharacter >= '0' && printableCharacter <= '9') : true);
    acceptCharacter = acceptCharacter && (myTextLengthLimit >= 0 ? (myText.length() < myTextLengthLimit) : true);
    if (acceptCharacter) GUIEditableTextLine::onTypedPrintableCharacter(printableCharacter);
}

