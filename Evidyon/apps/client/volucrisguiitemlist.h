//------------------------------------------------------------------------------------------------
// File:    volucrisguiitemlistentry.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIITEMLISTENTRY_H__
#define __VOLUCRISGUIITEMLISTENTRY_H__


/**
 * A list of items.  This class is notified when the mouse passes over a given list entry
 */
class VolucrisGUIItemList : public VolucrisGUIList
{
protected:
    /**
     * Stores a generic list entry with the three basic elements:  a left-side icon, text, and a
     * background.
     */
    class ItemListEntry : public VolucrisGUIList::VolucrisListEntryWithMouseInput
    {
        public:

            /**
             * Initializes this class
             */
            ItemListEntry();

            /**
             * Sets the list that manages this entry
             */
            void setOwner(VolucrisGUIItemList* owner);

            /**
             * Sets the item to display on this line
             * @param parameter Either the quantity (for stackable items) or its genite charge
             */
            void displayItem(unsigned int typeIndex, unsigned char parameter);

            /**
             * Sets the item to display on this line
             * @param parameter Either the quantity (for stackable items) or its genite charge
             */
            void displayItem(const ClientItemDescription* description, unsigned char parameter);

            /**
             * Gets the item being displayed on this line
             */
            const ClientItemDescription* getDisplayedItemDescription();

            /**
             * Sets the font used to display the text in this list entry
             */
            void setTextFont(GUIFont* font);


        public:

            /** 
             * Adds this list item's elements to the rendering process
             */
            virtual void registerElements(GUILayer* layer);

            /**
             * Removes this list item's elements from the screen
             */
            virtual void unregisterElements(GUILayer* layer);

            /**
             * Sets the clipping region of the internal elements
             */
            virtual void refresh();


        protected:

            /**
             * This method is called when the mouse interacts with this list entry
             */
            virtual void onMouseAction(MouseAction action);


        protected:

            /// The background GUI elmeent
            VolucrisGUIListEntryBackground myBackground;

            /// The text shown in the line
            VolucrisGUIListEntryText myText;


        private:

            /// The item being displayed on this line
            const ClientItemDescription* myItemDescription;

            /// The item parameter
            unsigned char myItemParameter;

            /// The list that owns this element
            VolucrisGUIItemList* myOwner;
    };

    public:

        /**
         * Initializes the class
         */
        VolucrisGUIItemList();

        /**
         * Sets the client's item descriptions array
         */
        void setItemDescriptions(const ClientItemDescription* itemDescriptions,
                                  size_t numberOfItemDescriptions);
        /**
         * Gets the client's description for a given item
         */
        bool getItemDescription(unsigned int itemTypeIndex, const ClientItemDescription** description);


    protected:

        /**
         * When the mouse moves over an entry (or leaves the list) this method is called.
         * The parameter is 'null' if the mouse left the list.
         */
        virtual void onChangeMouseOverEntry(ItemListEntry* item);


    protected:

        /**
         * Gets the entry that the mouse is currently over, if any
         */
        ItemListEntry* getCurrentMouseOverEntry();

        /**
         * Sets the current mouse over entry to NULL
         */
        void resetCurrentMouseOverEntry();


    private:

        /// The list of all of the information the client has about different item types
        const ClientItemDescription* myItemDescriptions;
        size_t myNumberOfItemDescriptions;


    private:

        /// The last entry that the mouse was over
        ItemListEntry* myCurrentMouseOverEntry;
};


#endif