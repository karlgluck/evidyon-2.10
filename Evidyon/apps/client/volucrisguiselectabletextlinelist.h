//------------------------------------------------------------------------------------------------
// File:    volucrisguiselectabletextlinelist.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUISELECTABLETEXTLINELIST_H__
#define __VOLUCRISGUISELECTABLETEXTLINELIST_H__


/**
 * Manages a list with selectable text lines in Volucris
 */
class VolucrisGUISelectableTextLineList : public GUIList
{
protected:

    class VolucrisListEntry;

private:

    /**
     * Renders the text from a list item to the screen
     */
    class ListEntryTextElement : public GUITextElement
    {
        public:

            /**
             * Initializes this element
             */
            ListEntryTextElement(VolucrisListEntry* listEntry);

            /**
             * Sets the rendering font for the text in this entry
             */
            void setFont(GUIFont* font);

            /**
             * Sets the text in this entry
             */
            void setText(const std::string& text);

            /**
             * Changes this text's color
             */
            void setTextColor(D3DCOLOR color);


        protected:

            /**
             * Gets the font to use to render the segments of text
             */
            GUIFont* getFont();

            /**
             * Implemented by the derived class to get information about the next piece
             * of text to render.  This class calls this method with a 'null' parameter
             * to indicate that it is about to start a new render call and the subclass
             * should reset any necessary data.  This method will be called until the
             * return code is 'false'.  If the return code is 'true', it assumes the
             * information in nextSegmentInfo is correct and attempts to render it.
             */
            bool getNextTextSegment(TextSegmentInfo* nextSegmentInfo);


        protected:

            /// The font to use to render the text
            GUIFont* myFont;

            /// The text to render on this line
            std::string myText;

            /// The color of the text to render
            D3DCOLOR myTextColor;

            /// The list item that contains this element
            VolucrisListEntry* myListEntry;
    };


    protected:

        /**
         * Keeps track of an element in the list
         */
        class VolucrisListEntry : public GUIList::ListEntry
        {
            /**
             * Watches for mouse input and changes the state of the list entry depending on it
             */
            class MouseSentinel : public GUIMouseSentinel
            {
                public:

                    /**
                     * Initializes this sentinel with its owner entry
                     */
                    MouseSentinel(VolucrisListEntry* listEntry);


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
                    VolucrisListEntry* myListEntry;
            };

            protected:

                /**
                 * Defines a state change for a list item.  The subclass
                 * handles processing this 
                 */
                enum StateChange
                {
                    /// Mouse enters the state where its primary button is up and the
                    /// cursor is positioned over over the element
                    STATECHANGE_MOUSEOVER,

                    /// Mouse enters the state where it is down and over the element
                    STATECHANGE_MOUSEOVERANDDOWN,

                    /// Mouse leaves this element's area
                    STATECHANGE_MOUSEOUT,

                    /// The user has selected this list element
                    STATECHANGE_SELECTED,

                    /// The user has deselected this list element
                    STATECHANGE_DESELECTED,
                };

            public:

                /**
                 * Initializes this element
                 */
                VolucrisListEntry();

                /**
                 * Causes this element to become selected
                 */
                void select();

                /**
                 * Explicitly deselects this list element
                 */
                void deselect();

                /**
                 * Determines whether or not this list entry is selected
                 */
                bool isSelected() const;

                /**
                 * Sets the rendering font for the text in this entry
                 */
                void setFont(GUIFont* font);

                /**
                 * Sets the text in this entry
                 */
                void setText(const std::string& text);

                /** 
                 * Adds this list item's elements to the rendering process
                 */
                void registerElements(GUILayer* layer);

                /**
                 * Removes this list item's elements from the screen
                 */
                void unregisterElements(GUILayer* layer);

                void setSelectable(bool selectable) { mySelectableFlag = selectable; }

            public:

                /**
                 * Updates the clipping region of the internal elements
                 */
                virtual void refresh();


            protected:

                /**
                 * This method is invoked when something (usually the mouse) causes the
                 * line item's state to change
                 */
                void stateChange(StateChange stateChange);

                /**
                 * Called to pass the state change method down to the derived class
                 */
                virtual void onStateChange(StateChange stateChange);


            protected:

                /// The mouse-input sentinel for this line
                MouseSentinel myLineMouseSentinel;

                /// The canvas for the background of this line
                GUICanvas myBackgroundCanvas;

                /// The renderer for this line's text
                ListEntryTextElement myTextElement;

                /// The element used to use to fill the background of this line
                GUIBorderedFillElement myBackground;


            private:

                /// Whether or not this list element is selected
                bool mySelectedFlag;

                /// Whether or not this line is selectable
                bool mySelectableFlag;
        };

        /**
         * This slider is used to scroll through the list
         */
        class VolucrisSlider : public GUISlider
        {
            public:

                VolucrisSlider(VolucrisGUISelectableTextLineList* list);

            protected:

                /**
                 * Called when the slider's state changes relative to user input.  If
                 * the state change is that the slider's position changed, this method
                 * updates the location of the list.
                 */
                virtual void onStateChange(StateChange stateChange);


            protected:

                /// The list that this slider controls
                VolucrisGUISelectableTextLineList* myList;
        };

    public:

        /**
         * Initializes this class
         */
        VolucrisGUISelectableTextLineList();

        /**
         * Registers the visible elements with the screen
         */
        void registerElements(GUILayer* layer);

        /**
         * Removes elements from the given layer 
         */
        void unregisterElements(GUILayer* layer);

        /**
         * Gets this list's mouse sentinel so that it can be added to something which triggers its events
         */
        virtual GUIMouseSentinel* getMouseSentinel();

        /**
         * Deselects all of the items in this list
         */
        void clearSelection();

        /**
         * Gets the canvas for this list so that it can be positioned
         */
        GUICanvas* getCanvas();


    protected:

        /// The canvas for this list
        GUICanvas myBackgroundCanvas;

        /// The background for the list
        GUIBorderedFillElement myBackgroundElement;

        /// The canvas for this list's slider
        GUICanvas mySliderCanvas;

        /// The slider for this list
        VolucrisSlider mySlider;

        /// The list's root mouse sentinel
        GUIMouseSentinel myRootMouseSentinel;
};



#endif