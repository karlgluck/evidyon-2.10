//------------------------------------------------------------------------------------------------
// File:    volucrisguibutton.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIBUTTON_H__
#define __VOLUCRISGUIBUTTON_H__

#include "../common/gui/guicanvas.h"
#include "../common/gui/guiborderedfillelement.h"
#include "../common/gui/guibutton.h"

class GUIFont;
class GUILayer;
struct GUIRect;

/**
 * Handles the rendering and behavior of a Volucris-specific button
 */
class VolucrisGUIButton : public GUIButton
{
    public:

        /**
         * Initializes this class
         */
        VolucrisGUIButton();

        /**
         * Changes the font this button will use to render its text
         */
        void setFont(GUIFont* font);

        /**
         * Sets that this button will display
         */
        void setText(const char* text);

        /**
         * Registers this button's elements with the layer so they can be drawn
         */
        virtual void registerElements(GUILayer* layer);

        /**
         * Removes this button's elements from the layer so they aren't drawn anymore
         */
        virtual void unregisterElements(GUILayer* layer);

        virtual void setClippingArea(const GUIRect* area);

    protected:

        /**
         * This method is called when button's state changes
         */
        virtual void onStateChange(GUIButton::StateChange stateChange);


    protected:

        /// The canvas for the background of this button
        /// The class that renders the background of this button
        GUICanvas myBackgroundCanvas;
        GUIBorderedFillElement myBackground;
};


#endif