//------------------------------------------------------------------------------------------------
// File:    volucrisguilistentryicon.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUILISTENTRYICON_H__
#define __VOLUCRISGUILISTENTRYICON_H__



/**
 * Draws an icon on the left hand side of a list entry that represents its state
 */
class VolucrisGUIListEntryIcon
{
    public:

        enum IconType
        {
            LIGHTBLUE,
            DARKBLUE,
            WHITE,
            RED,
            BLACK,
            NONE,
        };

    public:

        /**
         * Initializes this class
         */
        VolucrisGUIListEntryIcon(GUIList::ListEntry* owner);


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
         * Changes the type of icon to display
         */
        void setType(IconType type);

        /**
         * Sets the clipping region of the internal elements
         */
        void setClippingArea(const GUIRect* clippingArea);


    protected:

        /// The canvas for the icon element
        GUICanvas myStatusIconCanvas;

        /// The element used to use to fill the background of this line
        GUIPictureElement myStatusIcon;
};








#endif