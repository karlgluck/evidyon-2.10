//------------------------------------------------------------------------------------------------
// File:    volucrisguilist.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUILIST_H__
#define __VOLUCRISGUILIST_H__

#include "../common/gui/guilist.h"

/**
 * Generic list type
 */
class VolucrisGUIList : public GUIList
{
    public:

        /**
         * Stores a generic list entry that allows subclasses to initialize their
         * member elements.
         */
        class VolucrisListEntry : public GUIList::ListEntry
        {
            public:

                /** 
                 * Adds this list item's elements to the rendering process
                 */
                virtual void registerElements(GUILayer* layer) = 0;

                /**
                 * Removes this list item's elements from the screen
                 */
                virtual void unregisterElements(GUILayer* layer) = 0;

        };


        /**
         * Displays a list entry that watches for user input that could affect how this entry
         * is displayed.
         */
        class VolucrisListEntryWithMouseInput : public VolucrisListEntry
        {
            /**
             * Watches for mouse input and changes the state of the list entry depending on it
             */
            class StateMouseSentinel : public GUIMouseSentinel
            {
                public:

                    /**
                     * Initializes this sentinel with its owner entry
                     */
                    StateMouseSentinel(VolucrisListEntryWithMouseInput* owner);


                protected:

                    /**
                     * Called by the mouse input source when the mouse moves
                     * @return 'true' To indicate the event should stop propogating
                     */
                    bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);

                    /**
                     * Called by the mouse input source when a mouse button alters its state
                     * @return 'true' To indicate the event should stop propogating
                     */
                    bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);


                protected:

                    /// The entry that owns this sentinel
                    VolucrisListEntryWithMouseInput* myOwner;
            };


            public:

                /**
                 * Defines a state change for a list item.  The subclass
                 * handles processing this 
                 */
                enum MouseAction
                {
                    /// Mouse enters the state where its primary button is up and the
                    /// cursor is positioned over over the element
                    MOUSEOVER,

                    /// Mouse enters the state where it is down and over the element
                    MOUSEOVERANDDOWN,

                    /// Mouse leaves this element's area
                    MOUSEOUT,

                    /// The user has clicked somewhere in this list element
                    CLICKED,
                };


            public:

                /**
                 * Initializes the mouse sentinel
                 */
                VolucrisListEntryWithMouseInput();


            protected:

                /**
                 * This method is called when the mouse interacts with this list entry
                 */
                virtual void onMouseAction(MouseAction action) = 0;


            private:

                /// The sentinel that watches for state-changing mouse input
                StateMouseSentinel myStateMouseSentinel;
                
        };



        /**
         * A slider that can be used to control position in the list
         */
        class ListSlider : public VolucrisGUISlider
        {
            public:

                ListSlider(VolucrisGUIList* owner);

                /**
                 * Synchronizes this slider's position with the list to which it
                 * is attached.  This method is safe to call at any time and will
                 * not produce an infinite loop.
                 */
                void syncToList();

                /**
                 * Initializes this and the list canvas such that the slider
                 * and list share the area of the background canvas (both will
                 * be linked to this canvas.)  The slider will be positioned
                 * on the right.
                 */
                void setListCanvases(GUICanvas* backgroundCanvas);


            protected:

                /**
                 * Called by the compositor when the handle moves
                 */
                void onChangedPosition(float percentPosition);


            protected:

                /// The list to which this slider is attached
                VolucrisGUIList* myOwner;
        };


    public:

        /**
         * Invokes the method of the same name for all members of this list.  This method
         * assumes that the pointer returned by getListEntry can be cast to a VolucrisListEntry.
         */
        void registerElements(GUILayer* layer);

        /**
         * Invokes the method of the same name for all members of this list.  This method
         * assumes that the pointer returned by getListEntry can be cast to a VolucrisListEntry.
         */
        void unregisterElements(GUILayer* layer);
};







#endif