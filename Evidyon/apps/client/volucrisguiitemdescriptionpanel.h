//------------------------------------------------------------------------------------------------
// File:    volucrisguiitemdescriptionpanel.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIITEMDESCRIPTIONPANEL_H__
#define __VOLUCRISGUIITEMDESCRIPTIONPANEL_H__


/**
 *
 */
class VolucrisGUIItemDescriptionPanel
{
    public:

        /**
         *
         */
        VolucrisGUIItemDescriptionPanel();

        /**
         * Gets the panel's canvas so that it can be positioned
         */
        GUICanvas* getCanvas();

        /**
         * Registers the visible elements with the screen
         */
        void registerElements(GUILayer* layer);

        /**
         * Removes elements from the given layer 
         */
        void unregisterElements(GUILayer* layer);

        /**
         * Sets the fonts for this panel
         */
        void setFonts(GUIFont* titleFont, GUIFont* descriptionFont);

        /**
         * Changes the item that this description panel shows.  Passing 'null'
         * resets the panel to displaying nothing.
         */
        void display(const ClientItemDescription* description);


    protected:

        /// The panel where the item descriptions are shown
        VolucrisGUITitledPanel myItemDescriptionPanel;

        /// Text to display the item description
        GUICanvas myItemDescriptionTextCanvas;
        GUITextLabelElement myItemDescriptionText;
};


#endif
