//------------------------------------------------------------------------------------------------
// File:    volucrisguitradedisplay.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUITRADEDISPLAY_H__
#define __VOLUCRISGUITRADEDISPLAY_H__


/**
 * Shows what's happening during a trade between this client and another
 */
class VolucrisGUITradeDisplay
{
    /**
     * One of the item lists in the inventory
     */
    class InventoryItemList : public VolucrisGUIInventoryItemList
    {
        public:

            /**
             * Initializes this class with its owning inventory
             */
            InventoryItemList(VolucrisGUITradeDisplay* trade);

            /**
             * Regenerates the list of items that this list shows.  The indices that are
             * in the trade are automatically selected.
             */
            bool updateDisplay(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE],
                                unsigned char* indicesInTrade, unsigned int numberOfIndices);

            /**
             * Gets the list of items that the user has selected
             */
            void getSelectedItems(
                unsigned char* inventoryIndexBuffer,
                size_t bufferSize,
                size_t* numberOfSelectedItems);


        protected:

            /**
             * This method updates the item text
             */
            void onMouseOverListEntryChanged();

            /**
             * When the selection changes, this method is invoked to notify the derived class
             */
            void onSelectionChanged();


        protected:

            /// The owner display structure
            VolucrisGUITradeDisplay* myDisplay;
    };

    /**
     * Displays the items that the players are offering to one another
     */
    class TradeItemList : public GUIList
    {
        /**
         * Handles an entry in this list
         */
        class TradeItemListEntry : public GUIList::ListEntry
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
                    MouseSentinel(TradeItemListEntry* listEntry);

                protected:

                    /**
                     * Called by the mouse input source when the mouse moves.  If the mouse 
                     * @return 'true' To indicate the event should stop propogating
                     */
                    bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);


                protected:

                    /// The entry that owns this sentinel
                    TradeItemListEntry* myListEntry;
            };

            public:

                /**
                 * Initializes this class
                 */
                TradeItemListEntry();

                /**
                 * Sets the rendering font for the text in this entry
                 */
                void setFont(GUIFont* font);

                /**
                 * Sets the text in this entry
                 */
                void setText(const std::string& text);

                /**
                 * Sets the texture for the elements
                 */
                void setGUITexture(LPDIRECT3DTEXTURE9 texture);

                /**
                 * Frees this element's texture reference if it exists
                 */
                void releaseGUITexture();

                /** 
                 * Adds this list item's elements to the rendering process
                 */
                void registerElements(GUILayer* layer);

                /**
                 * Removes this list item's elements from the screen
                 */
                void unregisterElements(GUILayer* layer);


            public:

                /**
                 * Updates the clipping region of the internal elements
                 */
                void refresh();


            public:

                /**
                 * Sets the description for the item stored here.  This automatically
                 * updates the name of the item by calling setText.
                 */
                void setItem(ClientItemDescription* description, unsigned int quantity);

                /**
                 * Gets the item stored in this entry
                 */
                const ClientItemDescription* getItemDescription() const;


            protected:

                /// The mouse-input sentinel for this line
                MouseSentinel myLineMouseSentinel;

                /// The canvas for the background of this line
                GUICanvas myBackgroundCanvas;

                /// The renderer for this line's text
                ListEntryTextElement myTextElement;

                /// The element used to use to fill the background of this line
                GUIBorderedFillElement myBackground;

                /// The canvas to clip rendering to
                GUICanvas* myClipCanvas;


            protected:

                /// The description of the item displayed by this entry
                ClientItemDescription* myItemDescription;

                /// This item's quantity
                unsigned int myItemQuantity;
        };

        /**
         * This slider is used to scroll through the list
         */
        class VolucrisSlider : public GUISlider
        {
            public:

                /**
                 * Initializes this class
                 */
                VolucrisSlider(TradeItemList* list);

            protected:

                /**
                 * Called when the slider's state changes relative to user input.  If
                 * the state change is that the slider's position changed, this method
                 * updates the location of the list.
                 */
                virtual void onStateChange(StateChange stateChange);


            protected:

                /// The list that this slider controls
                TradeItemList* myList;
        };

        /**
         * This sentinel is responsible for resetting the mouse-over entry when the mouse leaves the list
         */
        class MouseLeftListSentinel : public GUIMouseSentinel
        {
            public:

                /**
                 * Initializes this class
                 */
                MouseLeftListSentinel(TradeItemList* list);


            protected:

                /**
                 * Called by the mouse input source when the mouse moves.  The default implementation propogates
                 * the message based on the focus/hierarchy structure established for the sentinels.
                 * @return 'true' To indicate the event should stop propogating
                 */
                bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);


            protected:

                /// The list that contains this sentinel
                TradeItemList* myList;
        };

        public:

            /**
             * Initializes this class
             */
            TradeItemList();

            /**
             * Registers the visible elements with the screen
             */
            void registerElements(GUILayer* layer);

            /**
             * Removes elements from the given layer 
             */
            void unregisterElements(GUILayer* layer);

            /**
             * Sets the GUI texture for this list
             */
            void setGUITexture(LPDIRECT3DTEXTURE9 guiTexture);

            /**
             * Removes this class's references to the GUI texture
             */
            void releaseGUITexture();

            /**
             * Gets this list's mouse sentinel so that it can be added to something which triggers its events
             */
            GUIMouseSentinel* getMouseSentinel();

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

            /// The entries in this list
            TradeItemListEntry myListEntries[MAX_ITEMS_PER_TRADE];
            unsigned int myNumberOfItems;
    };

    public:

        /**
         * Initializes this class
         */
        VolucrisGUITradeDisplay();

        /**
         * Initializes this display of the trade window
         */
        bool create(ClientItemDescription* itemDescriptions, size_t numberOfItemDescriptions,
                     GUIFont* windowFont, GUIFont* titleFont, LPDIRECT3DTEXTURE9 guiTexture,
                     GUILayer* layer, GUIMouseSentinel* parentSentinel);

        /**
         * Erases all of the members of this display, and removes its components from the
         * display layer.
         */
        void destroy();

        /**
         * Sets up the renderer to display the given trade.  The indices that are traded
         * by this local client (from the inventory) are selected in the inventory display
         * list automatically.
         */
        void displayTrade(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE],
                           unsigned char* indicesInTrade, unsigned int numberOfIndices,
                           TradeItem* itemsOfferedByOther, unsigned int numberOfItemsOfferedByOther);

        /**
         * Removes all of this display's items from the screen and mouse input chain
         */
        void hideDisplay();

        /**
         * Gets the list of items that the user has selected in either the inventory.  After the
         * list has been populated, the selected items are cleared.  This is used to send
         * the update offer message to the server.
         */
        void getSelectedItemsAndClearSelection(unsigned char* inventoryIndexBuffer, size_t bufferSize, size_t* numberOfSelectedItems);

        /**
         * If the accept trade button was pushed, this method returns true.  Subsequent calls will return false
         * until the user pushes the equip button again.
         */
        bool pollAcceptTradeButton();

        /**
         * If the cancel trade button was pushed, this method returns true.  Subsequent calls will return false
         * until the user pushes the unequip button again.
         */
        bool pollCancelTradeButton();

        /**
         * If the update offer button was pushed, this method returns true.  Subsequent calls will return false
         * until the user pushes the unequip button again.
         */
        bool pollUpdateOfferButton();




    protected:

        /// The target layer for this display
        GUILayer* myRenderingLayer;

        /// The owner sentinel for this display
        GUIMouseSentinel* myParentSentinel;

        /// The source item description array, obtained from the client game file
        ClientItemDescription* myItemDescriptionsArray;
        unsigned int myItemDescriptionsArraySize;

        // Mouse-over item descriptions shown here
        VolucrisGUITitledPanel myItemDescriptionPanel;  /// The panel where the item descriptions are shown
        GUICanvas myItemDescriptionTextCanvas;          /// Text to display the item description
        GUITextLabelElement myItemDescriptionText;

        // The items being traded by the players
        TradeItemList myItemsToBeTradedAway;
        TradeItemList myItemsToBeReceived;

        // The player's inventory
        VolucrisGUITitledPanel myInventoryItemsPanel;   /// The panel that displays shows carried items
        InventoryItemList myInventoryItemList;          /// Part of the carried items panel; displays the items that the character is carrying

        // Various interaction buttons
        VolucrisGUIButton myAcceptTradeButton;   /// Accept the trade and lock it in
        VolucrisGUIButton myCancelTradeButton;   /// Gets out of the trade window
        VolucrisGUIButton myUpdateOfferButton;   /// Changes what the client is offering
};



#endif