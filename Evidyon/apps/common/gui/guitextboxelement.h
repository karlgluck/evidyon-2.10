//------------------------------------------------------------------------------------------------
// File:  guitextboxelement.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUITEXTBOXELEMENT_H__
#define __GUITEXTBOXELEMENT_H__


/**
 * Renders a scrollable text-box.  The clipping area is the area into which text is rendered; the
 * source canvas is the area in which text is positioned.
 */
class GUITextBoxElement : public GUITextLabelElement
{
  protected:


    /**
     * Returns the text for this text box
     */
    bool getNextTextSegment(TextSegmentInfo* nextSegmentInfo);
};




#endif