//------------------------------------------------------------------------------------------------
// File:    volucrisguiinventoryitemlist.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"



//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIInventoryItemList
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUIInventoryItemList::VolucrisGUIInventoryItemList()
    : myMouseLeftListSentinel(this)
{
    myRenderingLayer = 0;
    myMouseOverListEntry = 0;
    myFont = 0;
    myInventoryItems = 0;
    myNumberOfInventoryItems = 0;
    setLineHeight(25);
}



//------------------------------------------------------------------------------------------------
// Name: ~VolucrisGUIInventoryItemList
// Desc: Cleans up this class when it disappears to prevent memory leaks.  This method doesn't
//       correctly unregister elements, but it does free their memory so it is NOT safe to
//       rely on this method.  Always call destroy() with a valid 'layer' parameter.
//------------------------------------------------------------------------------------------------
VolucrisGUIInventoryItemList::~VolucrisGUIInventoryItemList()
{
    destroy();
}



//------------------------------------------------------------------------------------------------
// Name: create
// Desc: Sets the texture and font that this list uses
//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryItemList::create(GUIFont* font, GUILayer* layer)
{
    // Fail unless all parameters are valid
    if (!font || !layer) return false;

    // Save the variables
    myFont = font;
    myRenderingLayer = layer;
    setLineHeight(font->getLineHeightInPixels());

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: initialize
// Desc: Sets up this list to display the given inventory.  This method can be used repeatedly
//       if the inventory needs to be refreshed.
//       @param inventory The full list of items to display.  'null' will cause the list to be cleared.
//       @param equipped Whether to display the equipped items or inventory items
//       @param layer The layer with which to register this inventory's elements
//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryItemList::initialize(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE],
                                               int update_indices[AVATAR_INVENTORY_SIZE],
                                               const ClientItemDescription* itemDescriptions,
                                               unsigned int numberOfItemDescriptions,  bool equipped,
                                               bool selectable)
{
    // Erase any existing elements
    clear();

    if (!inventory || !update_indices || !itemDescriptions) return true;

    // Parse the inventory to get the number of items we'll need
    unsigned int numberOfInventoryItems = 0;/*
    for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i)
    {
        if (inventory[i].equipped == equipped && inventory[i].quantity > 0 &&
            inventory[i].inventory_index == i && inventory[i].type < numberOfItemDescriptions)
            ++numberOfInventoryItems;
    }*/

    typedef std::multimap<int,ClientAvatarInventoryItem*> SortedInventory;
    SortedInventory sorted_inventory;
    for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i) {
      if (inventory[i].equipped == equipped &&
          inventory[i].quantity > 0 &&
          inventory[i].inventory_index == i &&
          inventory[i].type < numberOfItemDescriptions) {
        sorted_inventory.insert(SortedInventory::value_type(update_indices[i],&inventory[i]));
      }
    }
    numberOfInventoryItems = sorted_inventory.size();

    // Allocate the array
    myInventoryItems = new InventoryItemListEntry[numberOfInventoryItems];
    myNumberOfInventoryItems = numberOfInventoryItems;

    // Fill in each of the items
    size_t listIndex = 0;
    //for (int i = 0; i < AVATAR_INVENTORY_SIZE && listIndex < numberOfInventoryItems; ++i)
    for (SortedInventory::iterator i = sorted_inventory.begin(); i != sorted_inventory.end(); ++i)
    {
        //if (inventory[i].equipped == equipped && inventory[i].quantity > 0 &&
        //    /*inventory[i].inventory_index == i &&*/ inventory[i].type < numberOfItemDescriptions)
        {
            myInventoryItems[listIndex].setFont(myFont);

            //myInventoryItems[listIndex].initializeListEntry(this, &itemDescriptions[inventory[i].type], &inventory[i]);
            myInventoryItems[listIndex].initializeListEntry(this, &itemDescriptions[i->second->type], i->second);
            myInventoryItems[listIndex].setSelectable(selectable);
            listIndex++;
        }
    }

    // Refresh this list
    refresh();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Erases members of this list from the given layer and cleans up internal references.
//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryItemList::show()
{
    // Don't bother doing anything if the rendering layer is invalid
    if (!myRenderingLayer) return false;
    registerElements(myRenderingLayer);

    // If the item list doesn't exist, return early
    if (!myInventoryItems) return true;
    for (size_t i = 0; i < myNumberOfInventoryItems; ++i)
        myInventoryItems[i].registerElements(myRenderingLayer);
    
    // Success
    return true;
}


void VolucrisGUIInventoryItemList::setSelectable(bool selectable) {
  for (size_t i = 0; i < myNumberOfInventoryItems; ++i)
    myInventoryItems[i].setSelectable(selectable);
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Erases members of this list from the given layer and cleans up internal references.
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryItemList::hide()
{
    // Remove this list's rendering elements
    if (!myRenderingLayer) return;
    unregisterElements(myRenderingLayer);

    // Remove list entries' rendering elements
    if (!myInventoryItems) return;
    for (size_t i = 0; i < myNumberOfInventoryItems; ++i)
        myInventoryItems[i].unregisterElements(myRenderingLayer);
}



//------------------------------------------------------------------------------------------------
// Name: clear
// Desc: Erases all of the items in this list.  This will unregister the elements if the layer
//       parameter is valid.
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryItemList::clear()
{
    // Remove all of the elements from the GUI
    hide();

    // Erase the list of inventory items
    if (myInventoryItems)
    {
        // Get rid of the list
        delete [] myInventoryItems;
        myInventoryItems = 0;
        myNumberOfInventoryItems = 0;
    }

    // Update the display
    refresh();
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Erases members of this list from the given layer and cleans up internal references.
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryItemList::destroy()
{
    // Erase elements in this list
    clear();
}



//------------------------------------------------------------------------------------------------
// Name: getMouseOverItemDescription
// Desc: Gets the description of the item that the mouse is currently over
//------------------------------------------------------------------------------------------------
const ClientItemDescription* VolucrisGUIInventoryItemList::getMouseOverItemDescription()
{
    return myMouseOverListEntry ? myMouseOverListEntry->getItemDescription() : 0;
}



//------------------------------------------------------------------------------------------------
// Name: getNumberOfSelectedItems
// Desc: Calculates the number of selected items in this list
//------------------------------------------------------------------------------------------------
int VolucrisGUIInventoryItemList::getNumberOfSelectedItems() const
{
    if (!myInventoryItems) return 0;
    int numberSelected = 0;
    for (size_t i = 0; i < myNumberOfInventoryItems; ++i)
        if (myInventoryItems[i].isSelected()) ++numberSelected;
    return numberSelected;
}



//------------------------------------------------------------------------------------------------
// Name: deselectAll
// Desc: Deselects all of the items in this list
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryItemList::deselectAll()
{
    bool selectionChanged = false;
    if (!myInventoryItems) return;
    for (size_t i = 0; i < myNumberOfInventoryItems; ++i)
    {
        if (myInventoryItems[i].isSelected())
        {
            selectionChanged = true;
            myInventoryItems[i].deselect();
        }
    }
    //if (selectionChanged) onSelectionChanged();
    refresh();
}





//------------------------------------------------------------------------------------------------
// Name: getNumberOfLines
// Desc: Returns the number of lines in this list box
//------------------------------------------------------------------------------------------------
unsigned int VolucrisGUIInventoryItemList::getNumberOfLines() const
{
    return myNumberOfInventoryItems;
}



//------------------------------------------------------------------------------------------------
// Name: getListEntry
// Desc: Gets the list entry base class that corresponds to the given list item
//------------------------------------------------------------------------------------------------
GUIList::ListEntry* VolucrisGUIInventoryItemList::getListEntry(unsigned int index)
{
    return (ListEntry*)&myInventoryItems[index];
}



//------------------------------------------------------------------------------------------------
// Name: onMouseOverListEntryChanged
// Desc: When the item that the mouse is over changes, this method is called
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryItemList::onMouseOverListEntryChanged()
{
}



//------------------------------------------------------------------------------------------------
// Name: onSelectionChanged
// Desc: When the selection changes, this method is invoked to notify the derived class
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryItemList::onSelectionChanged()
{
}


//------------------------------------------------------------------------------------------------
// Name: MouseLeftListSentinel
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUIInventoryItemList::MouseLeftListSentinel::MouseLeftListSentinel(VolucrisGUIInventoryItemList* list)
{
    myList = list;
}



//------------------------------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves.  The default implementation propogates
//       the message based on the focus/hierarchy structure established for the sentinels.
//       @return 'true' To indicate the event should stop propogating
//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryItemList::MouseLeftListSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)
{
    GUICanvas* listCanvas = &myList->myTargetCanvas;

    bool insideOld = listCanvas->pointInside(oldPosition);
    bool insideNew = listCanvas->pointInside(position);

    if (insideOld && !insideNew)
    {
        myList->myMouseOverListEntry = 0;
        myList->onMouseOverListEntryChanged();
    }

    // Don't capture
    return false;
}



//------------------------------------------------------------------------------------------------
// Name: InventoryItemListEntry
// Desc: Initializes this item
//------------------------------------------------------------------------------------------------
VolucrisGUIInventoryItemList::InventoryItemListEntry::InventoryItemListEntry()
{
    myList = 0;
    myItemIndex = -1;
    myItemDescription = 0;
}



//------------------------------------------------------------------------------------------------
// Name: initializeListEntry
// Desc: Called after the list entry has been set up in order to set
//       its displayed text.
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryItemList::InventoryItemListEntry::initializeListEntry(
    VolucrisGUIInventoryItemList* list, const ClientItemDescription* itemDescription,
    ClientAvatarInventoryItem* itemToDisplay)
{
    myList = list;
    myItemIndex = itemToDisplay->inventory_index;
    myItemDescription = itemDescription;

    char text[256];
    if (itemToDisplay->quantity > 1)
    {
        sprintf_s(text, sizeof(text), "%ix %s", itemToDisplay->quantity, itemDescription->name);
    }
    else
        strcpy_s(text, sizeof(text), itemDescription->name);

    setText(text);
}



//------------------------------------------------------------------------------------------------
// Name: getItemIndex
// Desc: Gets this list entry's item index
//------------------------------------------------------------------------------------------------
int VolucrisGUIInventoryItemList::InventoryItemListEntry::getItemIndex() const
{
    return myItemIndex;
}




//------------------------------------------------------------------------------------------------
// Name: getItemDescription
// Desc: Gets the item stored in this entry
//------------------------------------------------------------------------------------------------
const ClientItemDescription* VolucrisGUIInventoryItemList::InventoryItemListEntry::getItemDescription() const
{
    return myItemDescription;
}




//------------------------------------------------------------------------------------------------
// Name: onStateChange
// Desc: When the mouse moves over this item, this method sets the appropriate field in the
//       owner list class.  This method is not responsible for setting the field when the mouse
//       leaves this item, since it could potentially overwrite a valid field value.
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryItemList::InventoryItemListEntry::onStateChange(StateChange stateChange)
{
    // Only process messages if the list has been initialized
    if (!myList) return;

    // Send notifications to the owner list
    if (stateChange == STATECHANGE_MOUSEOVER)
    {
        myList->myMouseOverListEntry = this;
        myList->onMouseOverListEntryChanged();
    }
    else if (stateChange == STATECHANGE_SELECTED || stateChange == STATECHANGE_DESELECTED)
        myList->onSelectionChanged();
}

