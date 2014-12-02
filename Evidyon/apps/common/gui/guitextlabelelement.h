//------------------------------------------------------------------------------------------------
// File:  guitextlabelelement.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUITEXTLABELELEMENT_H__
#define __GUITEXTLABELELEMENT_H__

#include "guitextelement.h"
#include <string>

enum Alignment {
  GUIALIGN_LEFT,
  GUIALIGN_CENTER,
  GUIALIGN_RIGHT,
  GUIALIGN_LEFT_TOP,
};

class GUIFont;

/**
 * Renders a single line of text that is centered horizontally within
 * the containing canvas.
 */
class GUITextLabelElement : public GUITextElement
{
  public:

    /**
     * Initializes the class
     */
    GUITextLabelElement();

    /**
     * Changes the font used to render this text
     */
    void setFont(GUIFont* font);

    /**
     * Changes the displayed text
     */
    void setText(const std::string& text);

    /**
     * Changes this text's color
     */
    void setTextColor(D3DCOLOR color);

    /**
     * Changes the alignment of this text
     */
    void setAlignment(Alignment alignment);

    const std::string& getText() const { return myText; }

    D3DCOLOR getTextColor() const { return myTextColor; }

    /**
     * Gets the font to use to render the this label
     */
    GUIFont* getFont();


  protected:

    /**
     * Returns the text for this label
     */
    bool getNextTextSegment(TextSegmentInfo* nextSegmentInfo);


  protected:

    /// The font to use to render this label
    GUIFont* myFont;

    /// The text to put in this label
    std::string myText;

    /// The color of text to render
    D3DCOLOR myTextColor;

    /// How to align the text in its source canvas
    Alignment myAlignment;
};




#endif