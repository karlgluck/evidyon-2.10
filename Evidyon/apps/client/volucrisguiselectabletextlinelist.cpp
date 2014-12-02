//------------------------------------------------------------------------------------------------
// File:    volucrisguilist.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"


//------------------------------------------------------------------------------------------------
// Name: VolucrisGUISelectableTextLineList
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUISelectableTextLineList::VolucrisGUISelectableTextLineList()
    : mySlider(this)
{
    myTargetCanvas.setSourceCanvas(&myBackgroundCanvas);
    myTargetCanvas.relativeDepth(-0.001f);
    myTargetCanvas.relativeHeight(-10);
    myTargetCanvas.relativeWidth(-10 - 16);
    myTargetCanvas.alignXLeft(5);
    myTargetCanvas.alignYTop(5);


    mySlider.getCanvas()->setSourceCanvas(&myBackgroundCanvas);
    mySlider.getCanvas()->absoluteWidth(16);
    mySlider.getCanvas()->relativeHeight(-10);
    mySlider.getCanvas()->alignXRight(-5);
    mySlider.getCanvas()->alignYTop(5);
    mySlider.getCanvas()->relativeDepth(-0.001f);

    myBackgroundElement.setSourceRegion(&VolucrisGUIDefs::CONTENT_COMPONENT_BACKGROUND, 8, VolucrisGUIDefs::TEXTURE_SIZE);
    myBackgroundElement.setSourceCanvas(&myBackgroundCanvas);
    mySlider.getHandleElement()->setSourceRegion(&VolucrisGUIDefs::SLIDER_HANDLE, VolucrisGUIDefs::TEXTURE_SIZE);
    mySlider.getBackgroundElement()->setSourceRegion(&VolucrisGUIDefs::SLIDER_BACKGROUND, 1, VolucrisGUIDefs::TEXTURE_SIZE);

    GUIList::getMouseSentinel()->setParentMouseSentinel(&myRootMouseSentinel);
    mySlider.getMouseSentinel()->setParentMouseSentinel(&myRootMouseSentinel);
}



//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers the visible elements with the screen
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::registerElements(GUILayer* layer)
{
    myBackgroundElement.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    mySlider.getBackgroundElement()->setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    mySlider.getHandleElement()->setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    layer->registerElement(&myBackgroundElement);
    mySlider.registerElements(layer);
}


//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes elements from the given layer 
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::unregisterElements(GUILayer* layer)
{
    layer->unregisterElement(&myBackgroundElement);
    mySlider.unregisterElements(layer);
}



//------------------------------------------------------------------------------------------------
// Name: getMouseSentinel
// Desc: Gets this list's mouse sentinel so that it can be added to something which triggers its events
//------------------------------------------------------------------------------------------------
GUIMouseSentinel* VolucrisGUISelectableTextLineList::getMouseSentinel()
{
    return &myRootMouseSentinel;
}


//------------------------------------------------------------------------------------------------
// Name: getCanvas
// Desc: Gets the canvas for this button so that it can be positioned
//------------------------------------------------------------------------------------------------
GUICanvas* VolucrisGUISelectableTextLineList::getCanvas()
{
    return &myBackgroundCanvas;
}





//------------------------------------------------------------------------------------------------
// Name: VolucrisSlider
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUISelectableTextLineList::VolucrisSlider::VolucrisSlider(VolucrisGUISelectableTextLineList* list)
{
    myList = list;
}



//------------------------------------------------------------------------------------------------
// Name: onStateChange
// Desc: Called when the slider's state changes relative to user input.  If
//       the state change is that the slider's position changed, this method
//       updates the location of the list.
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::VolucrisSlider::onStateChange(StateChange stateChange)
{/*
    switch(stateChange)
    {
        case STATECHANGE_MOUSEOVER_BACKGROUND:
        case STATECHANGE_MOUSEOVERANDDOWN_HANDLE:
        case STATECHANGE_MOUSEOVER_HANDLE:
        case STATECHANGE_MOUSEOVER_HANDLEANDBACKGROUND:
        case STATECHANGE_MOUSEOUT:
        case STATECHANGE_HANDLEMOVED:
    }*/

    if (stateChange == STATECHANGE_HANDLEMOVED)
    {
        myList->scrollToPercent(calculateSliderPercentLocation());
        //myList->refresh();
    }
}






//------------------------------------------------------------------------------------------------
// Name: VolucrisListEntry
// Desc: Initializes this element
//------------------------------------------------------------------------------------------------
VolucrisGUISelectableTextLineList::VolucrisListEntry::VolucrisListEntry()
    : myTextElement(this), myLineMouseSentinel(this)
{
    myBackgroundCanvas.alignXLeft(0);
    myBackgroundCanvas.alignYTop(0);
    myBackgroundCanvas.relativeHeight(0);
    myBackgroundCanvas.relativeWidth(0);
    myBackgroundCanvas.relativeDepth(-0.01f);

    myTextElement.setSourceCanvas(&myBackgroundCanvas);

    myBackground.setSourceCanvas(&myBackgroundCanvas);

    addChildMouseSentinel(&myLineMouseSentinel);
    addChildCanvas(&myBackgroundCanvas);

    mySelectedFlag = false;
    mySelectableFlag = true;
}



//------------------------------------------------------------------------------------------------
// Name: select
// Desc: Causes this element to become selected
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::VolucrisListEntry::select()
{
    if (mySelectableFlag && !mySelectedFlag)
    {
        mySelectedFlag = true;
        stateChange(STATECHANGE_SELECTED);
    }
}



//------------------------------------------------------------------------------------------------
// Name: deselect
// Desc: Explicitly deselects this list element
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::VolucrisListEntry::deselect()
{
    if (mySelectedFlag)
    {
        mySelectedFlag = false;
        stateChange(STATECHANGE_DESELECTED);
    }
}



//------------------------------------------------------------------------------------------------
// Name: isSelected
// Desc: Determines whether or not this list entry is selected
//------------------------------------------------------------------------------------------------
bool VolucrisGUISelectableTextLineList::VolucrisListEntry::isSelected() const
{
    return mySelectedFlag;
}



//------------------------------------------------------------------------------------------------
// Name: setFont
// Desc: Sets the rendering font for the text in this entry
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::VolucrisListEntry::setFont(GUIFont* font)
{
    myTextElement.setFont(font);
}



//------------------------------------------------------------------------------------------------
// Name: setText
// Desc: Sets the text in this entry
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::VolucrisListEntry::setText(const std::string& text)
{
    myTextElement.setText(text);
}





//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Adds this list item's elements to the rendering process
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::VolucrisListEntry::registerElements(GUILayer* layer)
{
    myBackground.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    refresh();
    stateChange(STATECHANGE_MOUSEOUT);
    layer->registerElement(&myTextElement);
    layer->registerElement(&myBackground);
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes this list item's elements from the screen
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::VolucrisListEntry::unregisterElements(GUILayer* layer)
{
    layer->unregisterElement(&myTextElement);
    layer->unregisterElement(&myBackground);
}



//------------------------------------------------------------------------------------------------
// Name: refresh
// Desc: Updates the clipping region of the internal elements
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::VolucrisListEntry::refresh()
{
    myBackground.setClippingArea(getClippingArea());
    myTextElement.setClippingArea(getClippingArea());
}



//------------------------------------------------------------------------------------------------
// Name: onStateChange
// Desc: This method is invoked when something (usually the mouse) causes the
//       line item's state to change
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::VolucrisListEntry::stateChange(StateChange stateChange)
{
    // Make sure the text is the right color
  myTextElement.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);

    // Handle the state change
    switch(stateChange)
    {
        case STATECHANGE_MOUSEOVER:
        {
            // Set the background state
            {
                GUIPoint pt = { 6*16, 4*16 }; // medium
                GUISize size = { 32, 32 };
                GUIRect area;
                area.set(pt, size);
                myBackground.setSourceRegion(&VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_MOUSEOVER, 3, VolucrisGUIDefs::TEXTURE_SIZE);
            }

        } break;

        case STATECHANGE_MOUSEOVERANDDOWN:
        {
            // Set the background state
            {
                GUIPoint pt = { 6*16, 6*16 }; // dark
                GUISize size = { 32, 32 };
                GUIRect area;
                area.set(pt, size);
                myBackground.setSourceRegion(&VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_MOUSEOVERANDDOWN, 3, VolucrisGUIDefs::TEXTURE_SIZE);
            }

        } break;
            
        case STATECHANGE_SELECTED:
        {
            {
                GUIPoint point = { 32, 64 };
                GUISize size = { 16, 16 };
                GUIRect sourceArea;
                sourceArea.set(point, size);
                myBackground.setSourceRegion(&VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_SELECTED, 3, VolucrisGUIDefs::TEXTURE_SIZE);
            }

        } break;

        default:
        case STATECHANGE_MOUSEOUT:
        {
            // Set the background state
            {
                GUIPoint pt = { 6*16, 2*16 }; // light
                GUISize size = { 32, 32 };
                GUIRect area;
                area.set(pt, size);
                myBackground.setSourceRegion(
                  isSelected() ? &VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_SELECTED :
                                 &VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_NEUTRAL, 3, VolucrisGUIDefs::TEXTURE_SIZE);
            }

        } break;

        case STATECHANGE_DESELECTED:
        {
          myBackground.setSourceRegion(
              &VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_NEUTRAL,
               3,
               VolucrisGUIDefs::TEXTURE_SIZE); // if this is deselected, the mouse is probably over it...
        } break;
    }

    // Pass this message to the derived class
    onStateChange(stateChange);
}


//------------------------------------------------------------------------------------------------
// Name: onStateChange
// Desc: Called to pass the state change method down to the derived class
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::VolucrisListEntry::onStateChange(StateChange stateChange)
{
}


//------------------------------------------------------------------------------------------------
// Name: MouseSentinel
// Desc: Initializes this sentinel with its owner entry
//------------------------------------------------------------------------------------------------
VolucrisGUISelectableTextLineList::VolucrisListEntry::MouseSentinel::MouseSentinel(VolucrisListEntry* listEntry)
{
    myListEntry = listEntry;
}



//------------------------------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves
//       @return 'true' To indicate the event should stop propogating
//------------------------------------------------------------------------------------------------
bool VolucrisGUISelectableTextLineList::VolucrisListEntry::MouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)
{
    // don't process if trapped by a child object
    if (GUIMouseSentinel::onMouseMove(position, oldPosition, mouse)) return true;
    if (!myListEntry->mySelectableFlag) return false;

    GUICanvas* listEntryCanvas = &myListEntry->myBackgroundCanvas;

    bool insideOld = myListEntry->getClippingArea()->contains(oldPosition) && listEntryCanvas->pointInside(oldPosition);
    bool insideNew = myListEntry->getClippingArea()->contains(position) && listEntryCanvas->pointInside(position);

    // Trigger state changes
    if (!insideOld &&  insideNew) myListEntry->stateChange(mouse->isPrimaryButtonDown() ? STATECHANGE_MOUSEOVERANDDOWN : STATECHANGE_MOUSEOVER);
    else if ( insideOld && !insideNew) myListEntry->stateChange(STATECHANGE_MOUSEOUT);

    // Don't capture
    return false;
}



//------------------------------------------------------------------------------------------------
// Name: onMouseButtonAction
// Desc: Called by the mouse input source when a mouse button alters its state
//       @return 'true' To indicate the event should stop propogating
//------------------------------------------------------------------------------------------------
bool VolucrisGUISelectableTextLineList::VolucrisListEntry::MouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse)
{
    // don't process if trapped by a child object
    if (GUIMouseSentinel::onMouseButtonAction(action, position, mouse)) return true;
    if (!myListEntry->mySelectableFlag) return false;

    GUICanvas* listEntryCanvas = &myListEntry->myBackgroundCanvas;

    // If this point is inside the list entry, do something with it
    if (listEntryCanvas->pointInside(position))
    {
        switch(action)
        {
            case GUIMOUSE_DOWN:         myListEntry->stateChange(STATECHANGE_MOUSEOVERANDDOWN);  break;
            case GUIMOUSE_UP:           break;//myListEntry->stateChange(STATECHANGE_MOUSEOVER); break;
            case GUIMOUSE_CLICK_SINGLE: myListEntry->isSelected() ? myListEntry->deselect() : myListEntry->select(); return true;
        }
    }

    // Don't capture this action
    return false;
}



//------------------------------------------------------------------------------------------------
// Name: VolucrisListEntryTextElement
// Desc: Initializes this element
//------------------------------------------------------------------------------------------------
VolucrisGUISelectableTextLineList::ListEntryTextElement::ListEntryTextElement(VolucrisGUISelectableTextLineList::VolucrisListEntry* listEntry)
{
    myListEntry = listEntry;
    myFont = 0;
    myTextColor = VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR;
}



//------------------------------------------------------------------------------------------------
// Name: setFont
// Desc: Sets the rendering font for the text in this entry
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::ListEntryTextElement::setFont(GUIFont* font)
{
    myFont = font;
}



//------------------------------------------------------------------------------------------------
// Name: setText
// Desc: Sets the text in this entry
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::ListEntryTextElement::setText(const std::string& text)
{
    myText = text;
}



//------------------------------------------------------------------------------------------------
// Name: setText
// Desc: Sets the text in this entry
//------------------------------------------------------------------------------------------------
void VolucrisGUISelectableTextLineList::ListEntryTextElement::setTextColor(D3DCOLOR color)
{
    myTextColor = color;
}




//------------------------------------------------------------------------------------------------
// Name: getFont
// Desc: Gets the font to use to render the segments of text
//------------------------------------------------------------------------------------------------
GUIFont* VolucrisGUISelectableTextLineList::ListEntryTextElement::getFont()
{
    return myFont;
}



//------------------------------------------------------------------------------------------------
// Name: getNextTextSegment
// Desc: Implemented by the derived class to get information about the next piece
//       of text to render.  This class calls this method with a 'null' parameter
//       to indicate that it is about to start a new render call and the subclass
//       should reset any necessary data.  This method will be called until the
//       return code is 'false'.  If the return code is 'true', it assumes the
//       information in nextSegmentInfo is correct and attempts to render it.
//------------------------------------------------------------------------------------------------
bool VolucrisGUISelectableTextLineList::ListEntryTextElement::getNextTextSegment(TextSegmentInfo* nextSegmentInfo)
{
    // Don't bother rendering if the list entry isn't visible
    if (!myListEntry->isVisible()) return false;

    if (nextSegmentInfo)
    {
        // Only render one line
        if (nextSegmentInfo->text == myText.c_str()) return false;

        nextSegmentInfo->text = myText.c_str();
        nextSegmentInfo->lineLength = -1;

        if (myFont && mySourceCanvas)
        {
            GUIPoint point;
            myFont->centerTextInArea(myText.c_str(), -1, mySourceCanvas->getArea(), &point);
            nextSegmentInfo->x = (float)(mySourceCanvas->getArea()->left + 4);
            nextSegmentInfo->y = point.y;
        }

        nextSegmentInfo->linesToIncrementAfterward = 0;
        nextSegmentInfo->color = myTextColor;

        // Render this segment
        return true;
    }
    else
        return true;
}

