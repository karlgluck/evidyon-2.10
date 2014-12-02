//-----------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"


//-----------------------------------------------------------------------------
VolucrisGUIDropItemsScreen::MouseSentinel::MouseSentinel() {
  enabled_ = false;
}

//-----------------------------------------------------------------------------
bool VolucrisGUIDropItemsScreen::MouseSentinel::onMouseMove(
    GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  if (!enabled_) return false;
  return GUIMouseSentinel::onMouseMove(position, oldPosition, mouse);
}

//-----------------------------------------------------------------------------
bool VolucrisGUIDropItemsScreen::MouseSentinel::onMouseButtonAction(
    GUIMouseButtonAction action,
    GUIPoint position,
    GUIMouseInputSource* mouse) {
  if (!enabled_) return false;
  return GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
}




VolucrisGUIDropItemsScreen::ItemList::ItemList(VolucrisGUIDropItemsScreen* owner) {
  owner_ = owner;
  setLineHeight(30);
}

void VolucrisGUIDropItemsScreen::ItemList::onMouseOverListEntryChanged() {
  if (!owner_) return;
  owner_->item_description_display_.display(getMouseOverItemDescription());
}

void VolucrisGUIDropItemsScreen::ItemList::onSelectionChanged() {
  //if (!owner_||!owner_->user_action_queue_) return;
  //for (size_t i = 0; i < myNumberOfInventoryItems; ++i) {
  //  if (myInventoryItems[i].isSelected()) {
  //    UserEventDropItem* drop_item = owner_->user_action_queue_->addDropItem();
  //    drop_item->inventory_index_ = myInventoryItems[i].getItemIndex();
  //    break;
  //  }
  //}
  //deselectAll();
}

VolucrisGUIDropItemsScreen::VolucrisGUIDropItemsScreen() : item_list_(this) {
  layer_ = NULL;
  user_action_queue_ = NULL;
  item_list_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  item_description_array_ = NULL;
  num_item_descriptions_ = 0;
}

bool VolucrisGUIDropItemsScreen::create(
    const ClientItemDescription* item_descriptions,
    size_t num_item_descriptions,
    GUIFont* font,
    GUIMouseSentinel* parent_mouse_sentinel,
    UserEventQueue* user_action_queue,
    GUILayer* layer) {
  destroy();

  mouse_sentinel_.setParentMouseSentinel(parent_mouse_sentinel);
  item_description_array_ = item_descriptions;
  num_item_descriptions_ = num_item_descriptions;

  finished_button_.setText("Finished");
  finished_button_.setFont(font);

  finished_button_.enable();
  finished_button_.resetPushed();
  finished_button_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  finished_button_.getCanvas()->setSourceCanvas(layer->getPrimaryCanvas());
  finished_button_.getCanvas()->alignYBottom(-5);
  finished_button_.getCanvas()->alignXRight(-5);
  finished_button_.getCanvas()->absoluteWidth(200);
  finished_button_.getCanvas()->absoluteHeight(35);
  finished_button_.getCanvas()->absoluteDepth(0.5f);

  GUISize layer_size = layer->getPrimaryCanvas()->getArea()->calculateSize();
  item_panel_.setFont(font);
  item_panel_.setText("Drop Items");

  int panel_widths = min(layer_size.width,600)-30;
  item_panel_.getCanvas()->setSourceCanvas(layer->getPrimaryCanvas());
  item_panel_.getCanvas()->absoluteWidth(panel_widths);
  item_panel_.getCanvas()->absoluteHeight(layer_size.height / 2 - 65);
  item_panel_.getCanvas()->alignYBottom(-45);
  item_panel_.getCanvas()->alignXCenter(0);
  item_panel_.getCanvas()->absoluteDepth(0.4f);

  item_description_display_.getCanvas()->setSourceCanvas(layer->getPrimaryCanvas());
  item_description_display_.getCanvas()->absoluteWidth(panel_widths);
  item_description_display_.getCanvas()->absoluteHeight(250-50);
  item_description_display_.getCanvas()->alignXCenter(0);
  item_description_display_.getCanvas()->alignYTop(+40);
  item_description_display_.getCanvas()->absoluteDepth(0.5f);

  item_list_.getCanvas()->setSourceCanvas(item_panel_.getCanvas());
  item_list_.getCanvas()->relativeWidth(-35);
  item_list_.getCanvas()->relativeHeight(-35);
  item_list_.getCanvas()->alignXCenter(0);
  item_list_.getCanvas()->alignYTop(+15);
  item_list_.getCanvas()->relativeDepth(-0.01f);
  item_list_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);

  user_action_queue_ = user_action_queue;

  item_description_display_.setFonts(font, font);

  if (APP_ERROR(!item_list_.create(font, layer))("Couldn't create item list"))
    return false;

  return true;
}


void VolucrisGUIDropItemsScreen::destroy() {
  hide();


  finished_button_.getMouseSentinel()->removeFromMouseSentinelChain();
}


void VolucrisGUIDropItemsScreen::show(GUILayer* layer) {

  mouse_sentinel_.enable(layer != NULL);

  if (layer_) {
    item_description_display_.unregisterElements(layer_);
    finished_button_.unregisterElements(layer_);
    item_panel_.unregisterElements(layer_);
    item_list_.hide();
  }

  if (layer) {
    item_description_display_.registerElements(layer);
    finished_button_.getCanvas()->setSourceCanvas(layer->getPrimaryCanvas());
    item_panel_.getCanvas()->setSourceCanvas(layer->getPrimaryCanvas());
    item_list_.refresh();
    item_list_.show();
    finished_button_.registerElements(layer);
    item_panel_.registerElements(layer);
  }

  layer_ = layer;
}


void VolucrisGUIDropItemsScreen::update(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE], int update_indices[AVATAR_INVENTORY_SIZE]) {
  unsigned int first_line_index;
  int first_line_offset;
  item_list_.getScrollLine(&first_line_index, &first_line_offset);
  item_list_.initialize(inventory, update_indices, item_description_array_, num_item_descriptions_, false, true);
  item_list_.scrollToLine(first_line_index, first_line_offset);
  item_list_.refresh();
  if (layer_) item_list_.show();
}

void VolucrisGUIDropItemsScreen::updateUserEvents() {
  if (finished_button_.wasPushed()) {
    finished_button_.resetPushed();
    if (user_action_queue_) user_action_queue_->addCloseDrop();
  }
}
