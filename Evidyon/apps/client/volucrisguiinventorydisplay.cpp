//------------------------------------------------------------------------------------------------
// File:    volucrisguiinventorydisplay.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "useractionqueue.h"




//------------------------------------------------------------------------------------------------
VolucrisGUIInventoryDisplay::KeyboardSentinel::KeyboardSentinel(VolucrisGUIInventoryDisplay* display) {
  display_ = display;
  enabled_ = false;
}



//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryDisplay::KeyboardSentinel::enable(bool enabled) {
  enabled_ = enabled;
}



//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryDisplay::KeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!enabled_) return false; // don't handle messages unless enabled
  if (GUIKeyboardSentinel::onKeyUp(key, keyboard)) return true; // don't process if the key was caught
  if (key == GUIKEY_ESCAPE) {
    display_->myFinishedButton.push();
    return true;
  }
  return false;
}



//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIInventoryDisplay
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUIInventoryDisplay::VolucrisGUIInventoryDisplay()
: myEquippedItemList(this), myCarriedItemList(this), myKeyboardSentinel(this)
{
    myRenderingLayer = 0;
    myParentSentinel = 0;
    myItemDescriptionsArray = 0;
    myItemDescriptionsArraySize = 0;

    myItemDescriptionPanel.getCanvas()->setSourceCanvas(&screen_canvas_);
    myEquippedItemsPanel.getCanvas()->setSourceCanvas(&screen_canvas_);
    myCarriedItemsPanel.getCanvas()->setSourceCanvas(&screen_canvas_);
    myFinishedButton.getCanvas()->setSourceCanvas(&screen_canvas_);

    myItemDescriptionPanel.getCanvas()->absoluteWidth(380);
    myItemDescriptionPanel.getCanvas()->absoluteHeight(300-30);
    myItemDescriptionPanel.getCanvas()->alignXRight(-30);
    myItemDescriptionPanel.getCanvas()->alignYBottom(-30);
    myItemDescriptionPanel.getCanvas()->absoluteDepth(0.5f);

    myEquippedItemsPanel.setText("Equipment");
    myEquippedItemsPanel.getCanvas()->absoluteWidth(380);
    myEquippedItemsPanel.getCanvas()->relativeHeight(-300-60);
    myEquippedItemsPanel.getCanvas()->alignXRight(-30);
    myEquippedItemsPanel.getCanvas()->alignYTop(30);
    myEquippedItemsPanel.getCanvas()->absoluteDepth(0.5f);

    myEquippedItemList.getCanvas()->setSourceCanvas(myEquippedItemsPanel.getCanvas());
    myEquippedItemList.getCanvas()->relativeWidth(-30);
    myEquippedItemList.getCanvas()->relativeHeight(-30);
    myEquippedItemList.getCanvas()->alignXCenter(0);
    myEquippedItemList.getCanvas()->alignYTop(15);
    myEquippedItemList.getCanvas()->relativeDepth(-0.1f);

    myUnequipButton.setText("Unequip");
    myUnequipButton.getCanvas()->setSourceCanvas(myEquippedItemsPanel.getCanvas());
    myUnequipButton.getCanvas()->absoluteWidth(200);
    myUnequipButton.getCanvas()->absoluteHeight(35);
    myUnequipButton.getCanvas()->alignXCenter(0);
    myUnequipButton.getCanvas()->alignYBottom(-8);
    myUnequipButton.getCanvas()->relativeDepth(-0.1f);

    myCarriedItemsPanel.setText("Inventory");
    myCarriedItemsPanel.getCanvas()->absoluteWidth(380-30);
    myCarriedItemsPanel.getCanvas()->relativeHeight(-30-35-30-30);
    myCarriedItemsPanel.getCanvas()->alignXRight(-30-380-30);
    myCarriedItemsPanel.getCanvas()->alignYTop(30);
    myCarriedItemsPanel.getCanvas()->absoluteDepth(0.4f);

    myCarriedItemList.getCanvas()->setSourceCanvas(myCarriedItemsPanel.getCanvas());
    myCarriedItemList.getCanvas()->relativeWidth(-30);
    myCarriedItemList.getCanvas()->relativeHeight(-30);
    myCarriedItemList.getCanvas()->alignXCenter(0);
    myCarriedItemList.getCanvas()->alignYTop(15);
    myCarriedItemList.getCanvas()->relativeDepth(-0.1f);

    myEquipButton.setText("Equip");
    myEquipButton.getCanvas()->setSourceCanvas(myCarriedItemsPanel.getCanvas());
    myEquipButton.getCanvas()->absoluteWidth(125);
    myEquipButton.getCanvas()->absoluteHeight(35);
    myEquipButton.getCanvas()->alignXCenter(-80);
    myEquipButton.getCanvas()->alignYBottom(-10);
    myEquipButton.getCanvas()->relativeDepth(-0.1f);

    myDropButton.setText("Drop");
    myDropButton.getCanvas()->setSourceCanvas(myCarriedItemsPanel.getCanvas());
    myDropButton.getCanvas()->absoluteWidth(125);
    myDropButton.getCanvas()->absoluteHeight(35);
    myDropButton.getCanvas()->alignXCenter(+80);
    myDropButton.getCanvas()->alignYBottom(-10);
    myDropButton.getCanvas()->relativeDepth(-0.1f);

    myFinishedButton.setText("Finished");
    myFinishedButton.getCanvas()->absoluteWidth(200);
    myFinishedButton.getCanvas()->absoluteHeight(35);
    myFinishedButton.getCanvas()->alignXRight(-30-380-30-(380-30)/2+200/2);
    myFinishedButton.getCanvas()->alignYBottom(-30);
    myFinishedButton.getCanvas()->absoluteDepth(0.5f);
}



//------------------------------------------------------------------------------------------------
// Name: create
// Desc: Initializes this display of this inventory on the target rendering layer.  This method
//       doesn't draw anything to the layer until items are added.
//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryDisplay::create(ClientItemDescription* itemDescriptions, size_t numberOfItemDescriptions,
    GUIFont* windowFont, GUIFont* titleFont, LPDIRECT3DTEXTURE9 guiTexture, GUILayer* layer, GUIMouseSentinel* parentSentinel,
    GUIKeyboardSentinel* keyboardSentinel, UserEventQueue* userActionQueue)
{
    // Save persistent parameters
    myRenderingLayer = layer;
    myParentSentinel = parentSentinel;
    myItemDescriptionsArray = itemDescriptions;
    myItemDescriptionsArraySize = numberOfItemDescriptions;
    myUserEventQueue = userActionQueue;


    // Set up all of the panel canvases
    screen_canvas_.setSourceCanvas(layer->getPrimaryCanvas());
    { // Position our screen canvas
      GUICanvas* layer_canvas = layer->getPrimaryCanvas();
      GUISize layer_size = layer_canvas->getArea()->calculateSize();
      screen_canvas_.relativeWidth(0.0f);
      screen_canvas_.relativeHeight(0.0f);
      screen_canvas_.alignXRight(0);
      screen_canvas_.alignYTop(0);
      screen_canvas_.relativeDepth(0.0f);

      myFinishedButton.getCanvas()->setSourceCanvas(&screen_canvas_);
      myFinishedButton.getCanvas()->alignYBottom(-5);
      myFinishedButton.getCanvas()->alignXRight(-5);

      myItemDescriptionPanel.getCanvas()->absoluteWidth(min(layer_size.width,800)-30);
      myItemDescriptionPanel.getCanvas()->absoluteHeight(250-50);
      myItemDescriptionPanel.getCanvas()->alignXCenter(0);
      myItemDescriptionPanel.getCanvas()->alignYBottom(-40);
      myEquippedItemsPanel.getCanvas()->absoluteWidth(layer_size.width / 2 - 30);
      myEquippedItemsPanel.getCanvas()->absoluteHeight(layer_size.height / 2 - 60);
      myEquippedItemsPanel.getCanvas()->alignYTop(+40);
      myEquippedItemsPanel.getCanvas()->alignXLeft(+10);
      myCarriedItemsPanel.getCanvas()->absoluteWidth(layer_size.width / 2 - 30);
      myCarriedItemsPanel.getCanvas()->absoluteHeight(layer_size.height / 2 - 60);
      myCarriedItemsPanel.getCanvas()->alignXRight(-10);
      myCarriedItemsPanel.getCanvas()->alignYTop(+40);
    }

    // Set the parameters for each of the panels
    myItemDescriptionPanel.setFonts(titleFont, windowFont);
    myEquippedItemsPanel.setFont(titleFont);
    myCarriedItemsPanel.setFont(titleFont);


    myUnequipButton.disable();
    myEquipButton.disable();
    myDropButton.disable();

    // Set the parameters for the buttons
    myUnequipButton.setFont(titleFont);

    myEquipButton.setFont(titleFont);

    myDropButton.setFont(titleFont);

    myFinishedButton.setFont(titleFont);


    myKeyboardSentinel.setParentKeyboardSentinel(keyboardSentinel);

    // Finally, initialize the lists
    if (APP_ERROR(!myEquippedItemList.create(windowFont, layer) ||
                   !myCarriedItemList.create(windowFont, layer))("Couldn't initialize lists"))
        return false;

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Erases all of the members of this inventory display, and removes its components from the
//       display layer.
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryDisplay::destroy()
{
    hideDisplay();

    myKeyboardSentinel.removeFromKeyboardSentinelChain();


    myEquippedItemList.destroy();
    myCarriedItemList.destroy();

    myRenderingLayer = 0;
    myParentSentinel = 0;
    myItemDescriptionsArray = 0;
    myItemDescriptionsArraySize = 0;
}


void VolucrisGUIInventoryDisplay::updateInventory(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE], int update_indices[AVATAR_INVENTORY_SIZE]) {
  unsigned int equipped_first_line, carried_first_line;
  int equipped_line_offset, carried_line_offset;
  myEquippedItemList.getScrollLine(&equipped_first_line, &equipped_line_offset);
  myCarriedItemList.getScrollLine(&carried_first_line, &carried_line_offset);
  myEquippedItemList.updateDisplay(inventory, update_indices, true);
  myCarriedItemList.updateDisplay(inventory, update_indices, false);
  myEquippedItemList.scrollToLine(equipped_first_line, equipped_line_offset);
  myCarriedItemList.scrollToLine(carried_first_line, carried_line_offset);
  myEquippedItemList.refresh();
  myCarriedItemList.refresh();
  if (myKeyboardSentinel.isEnabled()) {
    myEquippedItemList.show();
    myCarriedItemList.show();
  }
}



//------------------------------------------------------------------------------------------------
// Name: displayInventory
// Desc: Sets up the renderer to display the given inventory.  Passing a 'null' inventory will
//       make this renderer disappear.
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryDisplay::displayInventory()
{
    // Remove all of the items from the display, if they exist
    hideDisplay();

    myKeyboardSentinel.enable(true);

    // Update the lists
    myEquippedItemList.scrollToLine(0, 0);
    myCarriedItemList.scrollToLine(0, 0);
    myEquippedItemList.show();
    myCarriedItemList.show();

    // Register all of this display's elements
    myItemDescriptionPanel.registerElements(myRenderingLayer);
    myEquippedItemsPanel.registerElements(myRenderingLayer);
    myCarriedItemsPanel.registerElements(myRenderingLayer);
    //myUnequipButton.registerElements(myRenderingLayer);
    //myEquipButton.registerElements(myRenderingLayer);
    //myDropButton.registerElements(myRenderingLayer);
    myFinishedButton.registerElements(myRenderingLayer);

    // Register the item description
    //myRenderingLayer->registerElement(&myItemDescriptionText);

    // Update the lists
    myEquippedItemList.refresh();
    myCarriedItemList.refresh();

    // Reset button states
    myUnequipButton.disable();
    myEquipButton.disable();
    myDropButton.disable();
    myFinishedButton.enable();
    myFinishedButton.resetPushed();

    // Add to the input chain
    myEquippedItemList.getMouseSentinel()->setParentMouseSentinel(myParentSentinel);
    //myUnequipButton.getMouseSentinel()->setParentMouseSentinel(myParentSentinel);
    //myEquipButton.getMouseSentinel()->setParentMouseSentinel(myParentSentinel);
    //myDropButton.getMouseSentinel()->setParentMouseSentinel(myParentSentinel);
    myFinishedButton.getMouseSentinel()->setParentMouseSentinel(myParentSentinel);
    myCarriedItemList.getMouseSentinel()->setParentMouseSentinel(myParentSentinel);
}



//------------------------------------------------------------------------------------------------
// Name: hideDisplay
// Desc: Removes all of this display's items from the screen and mouse input chain
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryDisplay::hideDisplay()
{
    myKeyboardSentinel.enable(false);

    myEquippedItemList.hide();
    myCarriedItemList.hide();

    myUnequipButton.getMouseSentinel()->removeFromMouseSentinelChain();
    myEquippedItemList.getMouseSentinel()->removeFromMouseSentinelChain();
    myCarriedItemList.getMouseSentinel()->removeFromMouseSentinelChain();
    myEquipButton.getMouseSentinel()->removeFromMouseSentinelChain();
    myDropButton.getMouseSentinel()->removeFromMouseSentinelChain();
    myFinishedButton.getMouseSentinel()->removeFromMouseSentinelChain();

    CONFIRM(myRenderingLayer) else return;

    myItemDescriptionPanel.unregisterElements(myRenderingLayer);
    myEquippedItemsPanel.unregisterElements(myRenderingLayer);
    myUnequipButton.unregisterElements(myRenderingLayer);
    myEquipButton.unregisterElements(myRenderingLayer);
    myEquipButton.unregisterElements(myRenderingLayer);
    myDropButton.unregisterElements(myRenderingLayer);
    myFinishedButton.unregisterElements(myRenderingLayer);
    myCarriedItemsPanel.unregisterElements(myRenderingLayer);
}



//------------------------------------------------------------------------------------------------
// Name: getSelectedItemsAndClearSelection
// Desc: Gets the list of items that the user has selected in either the equipped-items list or
//       the inventory list.  After the list has been populated, the selected items are cleared
//       so that they are no longer selected.
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryDisplay::getSelectedItemsAndClearSelection(bool equipped, unsigned char* inventoryIndexBuffer, size_t bufferSize, size_t* numberOfSelectedItems)
{
    if (equipped)
        myEquippedItemList.getSelectedItemsAndClearSelection(inventoryIndexBuffer, bufferSize, numberOfSelectedItems);
    else
        myCarriedItemList.getSelectedItemsAndClearSelection(inventoryIndexBuffer, bufferSize, numberOfSelectedItems);

    myEquipButton.disable();
    myUnequipButton.disable();
    myDropButton.disable();
}



//------------------------------------------------------------------------------------------------
// Name: pollEquipButton
// Desc: If the equip button was pushed, this method returns true.  Subsequent calls will return false
//       until the user pushes the equip button again.
//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryDisplay::pollEquipButton()
{
    bool pushed = myEquipButton.wasPushed();
    if (pushed) myEquipButton.resetPushed();
    return pushed;
}



//------------------------------------------------------------------------------------------------
// Name: pollUnequipButton
// Desc: If the unequip button was pushed, this method returns true.  Subsequent calls will return false
//       until the user pushes the unequip button again.
//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryDisplay::pollUnequipButton()
{
    bool pushed = myUnequipButton.wasPushed();
    if (pushed) myUnequipButton.resetPushed();
    return pushed;
}



//------------------------------------------------------------------------------------------------
// Name: pollDropButton
// Desc: If the drop button was pushed, this method returns true.  Subsequent calls will return false
//       until the user pushes the drop button again.
//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryDisplay::pollDropButton()
{
    bool pushed = myDropButton.wasPushed();
    if (pushed) myDropButton.resetPushed();
    return pushed;
}



//------------------------------------------------------------------------------------------------
// Name: pollFinishedButton
// Desc: If the finished button was pushed, this method returns true.  Subsequent calls will return false
//       until the user pushes the finished button again.
//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryDisplay::pollFinishedButton()
{
    bool pushed = myFinishedButton.wasPushed();
    if (pushed) myFinishedButton.resetPushed();
    return pushed;
}


void VolucrisGUIInventoryDisplay::updateActionQueue() {
  if (pollEquipButton()) {
    myUserEventQueue->addInventoryEquipItems();
  } else if (pollUnequipButton()) {
    myUserEventQueue->addInventoryUnequipItems();
  } else if (pollDropButton()) {
    myUserEventQueue->addInventoryDropItems();
  } else if (pollFinishedButton()) {
    myUserEventQueue->addCloseInventory();
  }
}



//------------------------------------------------------------------------------------------------
// Name: ItemList
// Desc: Initializes this class with its owning inventory
//------------------------------------------------------------------------------------------------
VolucrisGUIInventoryDisplay::ItemList::ItemList(VolucrisGUIInventoryDisplay* inventory)
{
    myDisplay = inventory;
}



//------------------------------------------------------------------------------------------------
// Name: updateDisplay
// Desc: Regenerates the list of items that this list shows
//------------------------------------------------------------------------------------------------
bool VolucrisGUIInventoryDisplay::ItemList::updateDisplay(
    ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE], int update_indices[AVATAR_INVENTORY_SIZE], bool equippedList)
{
    return initialize(inventory, update_indices,
                       myDisplay->myItemDescriptionsArray,
                       myDisplay->myItemDescriptionsArraySize,
                       equippedList,
                       true); // selectable
}



//------------------------------------------------------------------------------------------------
// Name: getSelectedItemsAndClearSelection
// Desc: Gets the list of items that the user has selected
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryDisplay::ItemList::getSelectedItemsAndClearSelection(
    unsigned char* inventoryIndexBuffer, size_t bufferSize, size_t* numberOfSelectedItems)
{
    // Trap invalid arguments
    if (APP_ERROR(!inventoryIndexBuffer || !numberOfSelectedItems)("Invalid parameter")) return;

    // Get all of the selected items
    size_t selectedItems = 0;
    for (size_t i = 0; i < myNumberOfInventoryItems && selectedItems < bufferSize; ++i)
    {
        if (myInventoryItems[i].isSelected())
            inventoryIndexBuffer[selectedItems++] = myInventoryItems[i].getItemIndex();
    }
    *numberOfSelectedItems = selectedItems;

    // Remove any selected items in this list
    deselectAll();
}



//------------------------------------------------------------------------------------------------
// Name: onMouseOverListEntryChanged
// Desc: This method updates the item text
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryDisplay::ItemList::onMouseOverListEntryChanged()
{
    const ClientItemDescription* description = getMouseOverItemDescription();
    myDisplay->myItemDescriptionPanel.display(description);
}




//------------------------------------------------------------------------------------------------
// Name: onSelectionChanged
// Desc: When the selection changes, this method is invoked to notify the derived class
//------------------------------------------------------------------------------------------------
void VolucrisGUIInventoryDisplay::ItemList::onSelectionChanged()
{
    // Only grab the selection focus if items were selected
    if (!getNumberOfSelectedItems()) return;

    if (this == &(myDisplay->myEquippedItemList))
    {
        myDisplay->myCarriedItemList.deselectAll();
        myDisplay->myEquipButton.disable();
        myDisplay->myDropButton.disable();
        myDisplay->myUnequipButton.enable();

        myDisplay->myUnequipButton.push();
    }
    else
    {
        myDisplay->myEquippedItemList.deselectAll();
        myDisplay->myEquipButton.enable();
        myDisplay->myDropButton.enable();
        myDisplay->myUnequipButton.disable();

        myDisplay->myEquipButton.push();
    }
}

