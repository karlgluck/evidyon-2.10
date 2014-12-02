//------------------------------------------------------------------------------------------------
// File:    volucrisguilistentrybackground.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUILISTENTRYBACKGROUND_H__
#define __VOLUCRISGUILISTENTRYBACKGROUND_H__



/**
 * Draws a line in the GUI list with no member components
 */
class VolucrisGUIListEntryBackground
{
    public:

        enum BackgroundType
        {
            LIGHT,
            MEDIUM,
            DARK,
        };

    public:

        /**
         * Initializes this class
         */
        VolucrisGUIListEntryBackground(GUIList::ListEntry* owner);


        /**
         * Obtains the canvas for this element
         */
        GUICanvas* getCanvas();

        /** 
         * Adds this list item's elements to the rendering process
         */
        void registerElements(GUILayer* layer);

        /**
         * Removes this list item's elements from the screen
         */
        void unregisterElements(GUILayer* layer);

        /**
         * Changes the type of background to display
         */
        void setType(BackgroundType type);

        /**
         * Sets the clipping region of the internal elements
         */
        void setClippingArea(const GUIRect* clippingArea);


    protected:

        /// The canvas for the background rendering element of this line
        GUICanvas myBackgroundCanvas;

        /// The element used to use to fill the background of this line
        GUIBorderedFillElement myBackground;
};








#endif