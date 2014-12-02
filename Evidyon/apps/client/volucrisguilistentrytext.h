//------------------------------------------------------------------------------------------------
// File:    volucrisguilistentrytext.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUILISTENTRYTEXT_H__
#define __VOLUCRISGUILISTENTRYTEXT_H__


/**
 * Renders a single line of text for a list entry
 */
class VolucrisGUIListEntryText
{
    public:

        /**
         * Initializes the class
         */
        VolucrisGUIListEntryText(GUIList::ListEntry* owner);

        /**
         * Obtains the canvas for this element
         */
        GUICanvas* getCanvas();

        /**
         * Changes the font used to render this text.  The font must remain
         * valid.
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
         * Registers the visible elements with the screen
         */
        void registerElements(GUILayer* layer);

        /**
         * Removes elements from the given layer 
         */
        void unregisterElements(GUILayer* layer);

        /**
         * Sets the clipping region of the internal elements
         */
        void setClippingArea(const GUIRect* clippingArea);

        GUIFont* getFont() { return myTextLabelElement.getFont(); }
        const std::string& getText() const { return myTextLabelElement.getText(); }
        D3DCOLOR getTextColor() const { return myTextLabelElement.getTextColor(); }
        void setAlignment(Alignment alignment) { myTextLabelElement.setAlignment(alignment); }

    protected:

        /// Canvas onto which to draw this label
        GUICanvas myTextLabelCanvas;

        /// The element with which to render the text
        GUITextLabelElement myTextLabelElement;
};




#endif