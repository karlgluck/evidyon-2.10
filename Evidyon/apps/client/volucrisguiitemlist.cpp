//------------------------------------------------------------------------------------------------
// File:    volucrisguiitemlistentry.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"


//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIItemList
// Desc: Initializes the class
//------------------------------------------------------------------------------------------------
VolucrisGUIItemList::VolucrisGUIItemList()
{
    myItemDescriptions = 0;
    myNumberOfItemDescriptions = 0;
    myCurrentMouseOverEntry = 0;
}



//------------------------------------------------------------------------------------------------
// Name: setItemDescriptions
// Desc: Sets the client's item descriptions array
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::setItemDescriptions(const ClientItemDescription* itemDescriptions,
    size_t numberOfItemDescriptions)
{
    myItemDescriptions = itemDescriptions;
    myNumberOfItemDescriptions = numberOfItemDescriptions;
}



//------------------------------------------------------------------------------------------------
// Name: getItemDescription
// Desc: Gets the client's description for a given item
//------------------------------------------------------------------------------------------------
bool VolucrisGUIItemList::getItemDescription(unsigned int itemTypeIndex, const ClientItemDescription** description)
{
    if (itemTypeIndex >= myNumberOfItemDescriptions) return false;
    *description = &myItemDescriptions[itemTypeIndex];
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: onChangeMouseOverEntry
// Desc: When the mouse moves over an entry (or leaves the list) this method is called.
//       The parameter is 'null' if the mouse left the list.
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::onChangeMouseOverEntry(ItemListEntry* item)
{
}



//------------------------------------------------------------------------------------------------
// Name: getCurrentMouseOverEntry
// Desc: Gets the entry that the mouse is currently over, if any
//------------------------------------------------------------------------------------------------
VolucrisGUIItemList::ItemListEntry* VolucrisGUIItemList::getCurrentMouseOverEntry()
{
    return myCurrentMouseOverEntry;
}



//------------------------------------------------------------------------------------------------
// Name: resetCurrentMouseOverEntry
// Desc: Sets the current mouse over entry to NULL
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::resetCurrentMouseOverEntry()
{
    myCurrentMouseOverEntry = 0;
}




//------------------------------------------------------------------------------------------------
// Name: ItemListEntry
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUIItemList::ItemListEntry::ItemListEntry()
: myBackground(this), myText(this)
{
    myItemDescription = NULL;
    myItemParameter = 0;
    myText.setTextColor(VolucrisGUIDefs::LIST_ENTRY_TEXT_COLOR);
    myOwner = 0;
    myBackground.setType(VolucrisGUIListEntryBackground::DARK);
}

//------------------------------------------------------------------------------------------------
// Name: setOwner
// Desc: Sets the list that manages this entry
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::ItemListEntry::setOwner(VolucrisGUIItemList* owner)
{
    myOwner = owner;
}

//------------------------------------------------------------------------------------------------
// Name: displayItem
// Desc: Sets the item to display on this line
//       @param parameter Either the quantity (for stackable items) or its genite charge
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::ItemListEntry::displayItem(unsigned int typeIndex, unsigned char parameter)
{
    CONFIRM(myOwner) else return;
    const ClientItemDescription* description;
    if (myOwner->getItemDescription(typeIndex, &description))
        displayItem(description, parameter);
    myItemDescription = description;
    myItemParameter = typeIndex;
}


//------------------------------------------------------------------------------------------------
// Name: displayItem
// Desc: Sets the item to display on this line
//       @param parameter Either the quantity (for stackable items) or its genite charge
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::ItemListEntry::displayItem(const ClientItemDescription* description, unsigned char parameter)
{
    CONFIRM(description) else return;
    myText.setText(description->name);
}



//------------------------------------------------------------------------------------------------
// Name: getDisplayedItemDescription
// Desc: Gets the item being displayed on this line
//------------------------------------------------------------------------------------------------
const ClientItemDescription* VolucrisGUIItemList::ItemListEntry::getDisplayedItemDescription()
{
    return myItemDescription;
}



//------------------------------------------------------------------------------------------------
// Name: setTextFont
// Desc: Sets the font used to display the text in this list entry
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::ItemListEntry::setTextFont(GUIFont* font)
{
    myText.setFont(font);
}




//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Adds this list item's elements to the rendering process
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::ItemListEntry::registerElements(GUILayer* layer)
{
    myText.registerElements(layer);
    myBackground.registerElements(layer);
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes this list item's elements from the screen
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::ItemListEntry::unregisterElements(GUILayer* layer)
{
    myText.unregisterElements(layer);
    myBackground.unregisterElements(layer);
}



//------------------------------------------------------------------------------------------------
// Name: setClippingArea
// Desc: Sets the clipping region of the internal elements
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::ItemListEntry::refresh()
{
    myText.setClippingArea(getClippingArea());
    myBackground.setClippingArea(getClippingArea());
    myOwner->resetCurrentMouseOverEntry(); // remove current selection during scrolling
}



//------------------------------------------------------------------------------------------------
// Name: onMouseAction
// Desc: This method is called when the mouse interacts with this list entry
//------------------------------------------------------------------------------------------------
void VolucrisGUIItemList::ItemListEntry::onMouseAction(MouseAction action)
{
    CONFIRM(myOwner) else return;

    switch(action)
    {
        case MOUSEOVER:
            myOwner->myCurrentMouseOverEntry = this;
            myOwner->onChangeMouseOverEntry(this);
            myBackground.setType(VolucrisGUIListEntryBackground::MEDIUM);
            break;

        case MOUSEOUT:
            if (myOwner->myCurrentMouseOverEntry == this)
            {
                myOwner->resetCurrentMouseOverEntry();
                myOwner->onChangeMouseOverEntry(NULL);
            }
            myBackground.setType(VolucrisGUIListEntryBackground::DARK);
            break;
    }
}

