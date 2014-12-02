//------------------------------------------------------------------------------------------------
// File:    volucrisguiinventoryitemlist.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIINVENTORYITEMLIST_H__
#define __VOLUCRISGUIINVENTORYITEMLIST_H__



/**
 * This class is used to display a list of items
 */
class VolucrisGUIInventoryItemList : public VolucrisGUISelectableTextLineList
{
    /**
     * This class is responsible for managing an entry in the list
     */
    class InventoryItemListEntry : public VolucrisListEntry
    {
        public:

            /**
             * Initializes this item
             */
            InventoryItemListEntry();

            /**
             * Called after the list entry has been set up in order to set
             * its displayed text.
             */
            void initializeListEntry(VolucrisGUIInventoryItemList* list, const ClientItemDescription* itemDescription, ClientAvatarInventoryItem* itemToDisplay);

            /**
             * Gets this list entry's item index
             */
            int getItemIndex() const;

            /**
             * Gets the item stored in this entry
             */
            const ClientItemDescription* getItemDescription() const;


        protected:

            /**
             * When the mouse moves over this item, this method sets the appropriate field in the
             * owner list class.  This method is not responsible for setting the field when the mouse
             * leaves this item, since it could potentially overwrite a valid field value.
             * This method will call the parent list's onSelectionChanged when this item is
             * either selected or deselected.
             */
            void onStateChange(StateChange stateChange);


        protected:

            /// The description of the item displayed by this entry
            const ClientItemDescription* myItemDescription;

            /// The inventory index of the item that this list entry displays
            int myItemIndex;

            /// The owner class
            VolucrisGUIInventoryItemList* myList;

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
            MouseLeftListSentinel(VolucrisGUIInventoryItemList* list);


        protected:

            /**
             * Called by the mouse input source when the mouse moves.  The default implementation propogates
             * the message based on the focus/hierarchy structure established for the sentinels.
             * @return 'true' To indicate the event should stop propogating
             */
            bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);


        protected:

            /// The list that contains this sentinel
            VolucrisGUIInventoryItemList* myList;
    };


    public:

        /**
         * Initializes this class
         */
        VolucrisGUIInventoryItemList();

        /**
         * Cleans up this class when it disappears to prevent memory leaks; however, it is
         * bad practice to rely on this method.
         */
        ~VolucrisGUIInventoryItemList();

        /**
         * Sets the texture and font that this list uses
         */
        bool create(GUIFont* font, GUILayer* layer);

        /**
         * Sets up this list to display the given inventory.  This method can be used repeatedly
         * if the inventory needs to be refreshed.
         * @param inventory The full list of items to display.  'null' will cause the list to be cleared.
         * @param equipped Whether to display the equipped items or inventory items
         * @param layer The layer with which to register this inventory's elements
         */
        bool initialize(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE],
                         int update_indices[AVATAR_INVENTORY_SIZE],
                         const ClientItemDescription* itemDescriptions,
                         unsigned int numberOfItemDescriptions,  bool equipped, bool selectable);

        /**
         * Registers all of this item list's members with the target layer
         */
        bool show();

        /**
         * Changes whether or not this has selectable elements
         */
        void setSelectable(bool selectable);

        /**
         * Unregisters all of this list's members from the target layer
         */
        void hide();

        /**
         * Erases members of this list from the given layer and cleans up internal references.
         */
        void destroy();

        /**
         * Erases all of the items in this list
         */
        void clear();

        /**
         * Gets the item that the mouse is currently over
         */
        const ClientItemDescription* getMouseOverItemDescription();

        /**
         * Calculates the number of selected items in this list
         */
        int getNumberOfSelectedItems() const;

        /**
         * Deselects all of the items in this list
         */
        void deselectAll();


    protected:

        /**
         * Returns the number of lines in this list box
         */
        unsigned int getNumberOfLines() const;

        /**
         * Gets the list entry base class that corresponds to the given list item
         */
        ListEntry* getListEntry(unsigned int index);


    protected:

        /**
         * When the item that the mouse is over changes, this method is called
         */
        virtual void onMouseOverListEntryChanged();

        /**
         * When the selection changes, this method is invoked to notify the derived class
         */
        virtual void onSelectionChanged();


    protected:

        /// This is the layer into which this inventory renders
        GUILayer* myRenderingLayer;

        /// This sentinel detects when the mouse leaves the list so that it can reset the
        /// mouse-over list entry.
        MouseLeftListSentinel myMouseLeftListSentinel;

        /// Holds the list entry that the mouse is currently over
        InventoryItemListEntry* myMouseOverListEntry;

        /// This font is passed to each of the list elements when they are created
        GUIFont* myFont;

        /// The list of entries in this list
        InventoryItemListEntry* myInventoryItems;

        /// How many inventory items there are in the list
        unsigned int myNumberOfInventoryItems;
};





#endif