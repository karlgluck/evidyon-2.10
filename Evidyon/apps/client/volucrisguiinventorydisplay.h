//------------------------------------------------------------------------------------------------
// File:    volucrisguiinventorydisplay.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIINVENTORYDISPLAY_H__
#define __VOLUCRISGUIINVENTORYDISPLAY_H__


class UserEventQueue;


/**
 * This class is used to display a character's equipped and carried items
 */
class VolucrisGUIInventoryDisplay
{
  class KeyboardSentinel : public GUIKeyboardSentinel {
  public:
    KeyboardSentinel(VolucrisGUIInventoryDisplay* display);
    void enable(bool enabled);
    bool isEnabled() { return enabled_; }
    virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);

  private:
    VolucrisGUIInventoryDisplay* display_;
    bool enabled_;
  };

    /**
     * One of the item lists in the inventory
     */
    class ItemList : public VolucrisGUIInventoryItemList
    {
        public:

            /**
             * Initializes this class with its owning inventory
             */
            ItemList(VolucrisGUIInventoryDisplay* inventory);

            /**
             * Regenerates the list of items that this list shows
             */
            bool updateDisplay(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE], int update_indices[AVATAR_INVENTORY_SIZE], bool equippedList);

            /**
             * Gets the list of items that the user has selected
             */
            void getSelectedItemsAndClearSelection(
                unsigned char* inventoryIndexBuffer, size_t bufferSize, size_t* numberOfSelectedItems);


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
            VolucrisGUIInventoryDisplay* myDisplay;
    };

    public:

        /**
         * Initializes this class
         */
        VolucrisGUIInventoryDisplay();

        /**
         * Initializes this display of this inventory on the target rendering layer.  This method
         * doesn't draw anything to the layer until items are added.
         */
        bool create(ClientItemDescription* itemDescriptions, size_t numberOfItemDescriptions,
            GUIFont* windowFont, GUIFont* titleFont, LPDIRECT3DTEXTURE9 guiTexture, GUILayer* layer,
            GUIMouseSentinel* parentSentinel, GUIKeyboardSentinel* parentKeyboardSentinel, UserEventQueue* userActionQueue);

        /**
         * Erases all of the members of this inventory display, and removes its components from the
         * display layer.
         */
        void destroy();

        /**
         * Changes the items in the inventory
         * update_indices is an array used to sort the inventory.  1:1 mapping with entries in the inventory.  values
         * are simply sorted in ascending order with preference for earlier-occuring equal values.
         */
        void updateInventory(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE], int update_indices[AVATAR_INVENTORY_SIZE]);

        /**
         * Shows the inventory
         */
        void displayInventory();

        /**
         * Removes all of this display's items from the screen and mouse input chain
         */
        void hideDisplay();

        /**
         * Gets the list of items that the user has selected in either the equipped-items list or
         * the inventory list.  After the list has been populated, the selected items are cleared
         * so that they are no longer selected.
         */
        void getSelectedItemsAndClearSelection(bool equipped, unsigned char* inventoryIndexBuffer, size_t bufferSize, size_t* numberOfSelectedItems);

        /**
         * If the equip button was pushed, this method returns true.  Subsequent calls will return false
         * until the user pushes the equip button again.
         */
        bool pollEquipButton();

        /**
         * If the unequip button was pushed, this method returns true.  Subsequent calls will return false
         * until the user pushes the unequip button again.
         */
        bool pollUnequipButton();

        /**
         * If the drop button was pushed, this method returns true.  Subsequent calls will return false
         * until the user pushes the drop button again.
         */
        bool pollDropButton();

        /**
         * If the finished button was pushed, this method returns true.  Subsequent calls will return false
         * until the user pushes the finished button again.
         */
        bool pollFinishedButton();

        void updateActionQueue();


    protected:

      GUICanvas screen_canvas_; // controls the area into which this screen can render.  not always the full screen area!

        /// The target layer for this display
        GUILayer* myRenderingLayer;

        /// The owner sentinel for this display
        GUIMouseSentinel* myParentSentinel;

        /// The source item description array, obtained from the client game file
        ClientItemDescription* myItemDescriptionsArray;
        unsigned int myItemDescriptionsArraySize;

        /// The panel where the item descriptions are shown
        //VolucrisGUITitledPanel myItemDescriptionPanel;
        VolucrisGUIItemDescriptionPanel myItemDescriptionPanel;

        /// The panel where the equipped items are shown
        VolucrisGUITitledPanel myEquippedItemsPanel;

        /// Part of the equipped items panel.  When clicked, it means the user wants to unequip the
        /// selected items.
        VolucrisGUIButton myUnequipButton;

        /// Part of the equipped items panel.  Displays the items that the user has equipped.
        ItemList myEquippedItemList;

        /// The panel that displays shows carried items
        VolucrisGUITitledPanel myCarriedItemsPanel;

        /// Part of the carried items panel; displays the items that the character is carrying
        ItemList myCarriedItemList;

        /// When the user presses this button, they want to equip items from the carried item list
        VolucrisGUIButton myEquipButton;

        /// When the user presses this button, they want to drop selected items in the carried item list
        VolucrisGUIButton myDropButton;

        /// When the user presses this button, they want to leave the screen
        VolucrisGUIButton myFinishedButton;

        /// The queue into which actions performed in this menu are placed
        UserEventQueue* myUserEventQueue;
        KeyboardSentinel myKeyboardSentinel;
};



#endif