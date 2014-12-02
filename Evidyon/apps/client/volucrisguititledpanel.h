//------------------------------------------------------------------------------------------------
// File:    volucrisguititledpanel.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUITITLEDPANEL_H__
#define __VOLUCRISGUITITLEDPANEL_H__

#include "../common/gui/guicomponent.h"
#include "volucrisguisideiconselement.h"

/**
 * Draws a panel with a title
 */
class VolucrisGUITitledPanel
{
    public:

        /**
         * Initializes this class
         */
        VolucrisGUITitledPanel();

        /**
         * Changes the font this panel will use to render its text
         */
        void setFont(GUIFont* font);

        /**
         * Sets the label for this panel
         */
        void setText(const char* text);

        /**
         * Registers this panel's visible elements with the given layer
         */
        void registerElements(GUILayer* layer);

        /**
         * Unregisters this panel's visible elements
         */
        void unregisterElements(GUILayer* layer);

        /**
         * Gets the canvas for this panel so that it can be positioned
         */
        GUICanvas* getCanvas();


    protected:

        /// The canvas that specifies the area of the panel (the label extends slightly beyond this)
        GUICanvas myPanelCanvas;

        /// Title's canvas
        GUICanvas myTitleCanvas;

        /// Title background
        GUIBorderedFillElement myTitleBackground;

        /// Title text renderer
        GUITextLabelElement myTitleText;

        /// The window title's side icons
        VolucrisGUISideIconsElement myTitleSideIcons; // TODO: get rid of side icons! they're not used anymore

        /// Background for the whole panel
        GUIBorderedFillElement myPanelBackground;
};


#endif