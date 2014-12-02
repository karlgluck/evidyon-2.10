//------------------------------------------------------------------------------------------------
// File:    volucrisguitradedisplay.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
//#include "volucrisguitradedisplay.h"

#if 0

//------------------------------------------------------------------------------------------------
// Name: VolucrisGUITradeDisplay
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUITradeDisplay::VolucrisGUITradeDisplay()
{
    myRenderingLayer = 0;
    myParentSentinel = 0;
    myItemDescriptionsArray = 0;
    myItemDescriptionsArraySize = 0;


}



//------------------------------------------------------------------------------------------------
// Name: create
// Desc: Initializes this display of the trade window
//------------------------------------------------------------------------------------------------
bool VolucrisGUITradeDisplay::create(ClientItemDescription* itemDescriptions, size_t numberOfItemDescriptions,
    GUIFont* windowFont, GUIFont* titleFont, LPDIRECT3DTEXTURE9 guiTexture,
    GUILayer* layer, GUIMouseSentinel* parentSentinel)
{
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Erases all of the members of this display, and removes its components from the
//       display layer.
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::destroy()
{
}



//------------------------------------------------------------------------------------------------
// Name: displayTrade
// Desc: Sets up the renderer to display the given trade.  The indices that are traded
//       by this local client (from the inventory) are selected in the inventory display
//       list automatically.
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::displayTrade(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE],
    unsigned char* indicesInTrade, unsigned int numberOfIndices,
    TradeItem* itemsOfferedByOther, unsigned int numberOfItemsOfferedByOther)
{
}



//------------------------------------------------------------------------------------------------
// Name: hideDisplay
// Desc: Removes all of this display's items from the screen and mouse input chain
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::hideDisplay()
{
}



//------------------------------------------------------------------------------------------------
// Name: getSelectedItemsAndClearSelection
// Desc: Gets the list of items that the user has selected in either the inventory.  After the
//       list has been populated, the selected items are cleared.  This is used to send
//       the update offer message to the server.
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::getSelectedItemsAndClearSelection(unsigned char* inventoryIndexBuffer, size_t bufferSize, size_t* numberOfSelectedItems)
{
}



//------------------------------------------------------------------------------------------------
// Name: pollAcceptTradeButton
// Desc: If the accept trade button was pushed, this method returns true.  Subsequent calls will return false
//       until the user pushes the equip button again.
//------------------------------------------------------------------------------------------------
bool VolucrisGUITradeDisplay::pollAcceptTradeButton()
{
}



//------------------------------------------------------------------------------------------------
// Name: pollCancelTradeButton
// Desc: If the cancel trade button was pushed, this method returns true.  Subsequent calls will return false
//       until the user pushes the unequip button again.
//------------------------------------------------------------------------------------------------
bool VolucrisGUITradeDisplay::pollCancelTradeButton()
{
}



//------------------------------------------------------------------------------------------------
// Name: pollUpdateOfferButton
// Desc: If the update offer button was pushed, this method returns true.  Subsequent calls will return false
//       until the user pushes the unequip button again.
//------------------------------------------------------------------------------------------------
bool VolucrisGUITradeDisplay::pollUpdateOfferButton()
{
}

//------------------------------------------------------------------------------------------------
// Name: TradeItemList
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUITradeDisplay::TradeItemList::TradeItemList()
{
}



//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers the visible elements with the screen
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::TradeItemList::registerElements(GUILayer* layer)
{
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes elements from the given layer 
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::TradeItemList::unregisterElements(GUILayer* layer)
{
}




//------------------------------------------------------------------------------------------------
// Name: getMouseSentinel
// Desc: Gets this list's mouse sentinel so that it can be added to something which triggers its events
//------------------------------------------------------------------------------------------------
GUIMouseSentinel* VolucrisGUITradeDisplay::TradeItemList::getMouseSentinel()
{
}



//------------------------------------------------------------------------------------------------
// Name: clearSelection
// Desc: Deselects all of the items in this list
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::TradeItemList::clearSelection()
{
}



//------------------------------------------------------------------------------------------------
// Name: getCanvas
// Desc: Gets the canvas for this list so that it can be positioned
//------------------------------------------------------------------------------------------------
GUICanvas* VolucrisGUITradeDisplay::TradeItemList::getCanvas()
{
}

//------------------------------------------------------------------------------------------------
// Name: MouseLeftListSentinel
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUITradeDisplay::TradeItemList::MouseLeftListSentinel::MouseLeftListSentinel(TradeItemList* list)
{
}



//------------------------------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves.  The default implementation propogates
//       the message based on the focus/hierarchy structure established for the sentinels.
//       @return 'true' To indicate the event should stop propogating
//------------------------------------------------------------------------------------------------
bool VolucrisGUITradeDisplay::TradeItemList::MouseLeftListSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)
{
}

//------------------------------------------------------------------------------------------------
// Name: VolucrisSlider
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUITradeDisplay::TradeItemList::VolucrisSlider::VolucrisSlider(TradeItemList* list)
{
}



//------------------------------------------------------------------------------------------------
// Name: onStateChange
// Desc: Called when the slider's state changes relative to user input.  If
//       the state change is that the slider's position changed, this method
//       updates the location of the list.
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::TradeItemList::VolucrisSlider::onStateChange(StateChange stateChange)
{
}

//------------------------------------------------------------------------------------------------
// Name: TradeItemListEntry
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUITradeDisplay::TradeItemList::TradeItemListEntry::TradeItemListEntry()
{
}



//------------------------------------------------------------------------------------------------
// Name: setFont
// Desc: Sets the rendering font for the text in this entry
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::TradeItemList::TradeItemListEntry::setFont(GUIFont* font)
{
}



//------------------------------------------------------------------------------------------------
// Name: setText
// Desc: Sets the text in this entry
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::TradeItemList::TradeItemListEntry::setText(const std::string& text)
{
}





//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Adds this list item's elements to the rendering process
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::TradeItemList::TradeItemListEntry::registerElements(GUILayer* layer)
{
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes this list item's elements from the screen
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::TradeItemList::TradeItemListEntry::unregisterElements(GUILayer* layer)
{
}



//------------------------------------------------------------------------------------------------
// Name: refresh
// Desc: Updates the clipping region of the internal elements
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::TradeItemList::TradeItemListEntry::refresh()
{
}



//------------------------------------------------------------------------------------------------
// Name: setItem
// Desc: Sets the description for the item stored here.  This automatically
//       updates the name of the item by calling setText.
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::TradeItemList::TradeItemListEntry::setItem(ClientItemDescription* description, unsigned int quantity)
{
}



//------------------------------------------------------------------------------------------------
// Name: getItemDescription
// Desc: Gets the item stored in this entry
//------------------------------------------------------------------------------------------------
const ClientItemDescription* VolucrisGUITradeDisplay::TradeItemList::TradeItemListEntry::getItemDescription() const
{
}

//------------------------------------------------------------------------------------------------
// Name: MouseSentinel
// Desc: Initializes this sentinel with its owner entry
//------------------------------------------------------------------------------------------------
VolucrisGUITradeDisplay::TradeItemList::TradeItemListEntry::MouseSentinel::MouseSentinel(TradeItemListEntry* listEntry)
{
}



//------------------------------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves.  If the mouse 
//       @return 'true' To indicate the event should stop propogating
//------------------------------------------------------------------------------------------------
bool VolucrisGUITradeDisplay::TradeItemList::TradeItemListEntry::MouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)
{
}

//------------------------------------------------------------------------------------------------
// Name: InventoryItemList
// Desc: Initializes this class with its owning inventory
//------------------------------------------------------------------------------------------------
VolucrisGUITradeDisplay::InventoryItemList::InventoryItemList(VolucrisGUITradeDisplay* trade)
{
}



//------------------------------------------------------------------------------------------------
// Name: updateDisplay
// Desc: Regenerates the list of items that this list shows.  The indices that are
//       in the trade are automatically selected.
//------------------------------------------------------------------------------------------------
bool VolucrisGUITradeDisplay::InventoryItemList::updateDisplay(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE],
    unsigned char* indicesInTrade, unsigned int numberOfIndices)
{
}



//------------------------------------------------------------------------------------------------
// Name: getSelectedItems
// Desc: Gets the list of items that the user has selected
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::InventoryItemList::getSelectedItems(
    unsigned char* inventoryIndexBuffer,
    size_t bufferSize,
    size_t* numberOfSelectedItems)
{
}



//------------------------------------------------------------------------------------------------
// Name: onMouseOverListEntryChanged
// Desc: This method updates the item text
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::InventoryItemList::onMouseOverListEntryChanged()
{
}



//------------------------------------------------------------------------------------------------
// Name: onSelectionChanged
// Desc: When the selection changes, this method is invoked to notify the derived class
//------------------------------------------------------------------------------------------------
void VolucrisGUITradeDisplay::InventoryItemList::onSelectionChanged()
{
}


#endif