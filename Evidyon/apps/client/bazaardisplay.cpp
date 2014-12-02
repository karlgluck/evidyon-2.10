//-----------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"



//-----------------------------------------------------------------------------
BazaarDisplay::MouseSentinel::MouseSentinel() {
  enabled_ = false;
}

//-----------------------------------------------------------------------------
bool BazaarDisplay::MouseSentinel::onMouseMove(
  GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  if (!enabled_) return false;
  return GUIMouseSentinel::onMouseMove(position, oldPosition, mouse);
}

//-----------------------------------------------------------------------------
bool BazaarDisplay::MouseSentinel::onMouseButtonAction(
  GUIMouseButtonAction action,
  GUIPoint position,
  GUIMouseInputSource* mouse) {
  if (!enabled_) return false;
  return GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
}


//-----------------------------------------------------------------------------
BazaarDisplay::KeyboardSentinel::KeyboardSentinel(BazaarDisplay* owner) {
  owner_ = owner;
  enabled_ = false;
}



//-----------------------------------------------------------------------------
void BazaarDisplay::KeyboardSentinel::enable(bool enabled) {
  enabled_ = enabled;
}



//-----------------------------------------------------------------------------
bool BazaarDisplay::KeyboardSentinel::onKeyUp(
  GUIKey key,
  GUIKeyboardInputSource* keyboard) {
  if (!enabled_) return false; // don't handle messages unless enabled
  if (GUIKeyboardSentinel::onKeyUp(key, keyboard))
  return true; // don't process if the key was caught
  if (key == GUIKEY_ESCAPE) {
  if (owner_->display_mode_ == BAZAAR_MODE_LISTFORSALE_INPUTITEMPRICE) {
    owner_->listing_cancel_.push();
  } else {
    owner_->finished_button_.push();
  }
  return true;
  }
  if (key == GUIKEY_RETURN && owner_->display_mode_ == BAZAAR_MODE_LISTFORSALE_INPUTITEMPRICE) {
  owner_->listing_confirm_.push();
  return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
BazaarDisplay::InventoryItemList::InventoryItemList(BazaarDisplay* owner) {
  owner_ = owner;
  setLineHeight(30);
}

//-----------------------------------------------------------------------------
void BazaarDisplay::InventoryItemList::onMouseOverListEntryChanged() {
  if (!owner_) return;
  CONFIRM(owner_->display_mode_ == BAZAAR_MODE_LISTFORSALE) else return;
  const ClientItemDescription* desc = getMouseOverItemDescription();
  owner_->item_description_.setText(desc->description);
}

//-----------------------------------------------------------------------------
void BazaarDisplay::InventoryItemList::onSelectionChanged() {
  if (!owner_) return;
  CONFIRM(owner_->display_mode_ == BAZAAR_MODE_LISTFORSALE) else return;
  for (size_t i = 0; i < myNumberOfInventoryItems; ++i) {
  if (myInventoryItems[i].isSelected()) {
    owner_->listForSale(myInventoryItems[i].getItemIndex());
    break;
  }
  }
}

//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::BazaarItemListEntry::initialize(
  BazaarItemList* owner, GUIFont* font) {
  owner_ = owner;
  price_text_.setFont(font);
  price_text_.setTextColor(VolucrisGUIDefs::LIST_ENTRY_TEXT_COLOR);
  VolucrisGUIItemList::ItemListEntry::setTextFont(font);
  price_text_.setAlignment(GUIALIGN_RIGHT);
}

void BazaarDisplay::BazaarItemList::BazaarItemListEntry::copyDisplay(
  BazaarItemListEntry* other) {
  price_value_ = other->price_value_;
  owned_item_ = other->owned_item_;
  price_text_.setTextColor(other->price_text_.getTextColor());
  price_text_.setText(other->price_text_.getText());
  myText.setTextColor(other->myText.getTextColor());
  myText.setText(other->myText.getText());
  myText.setAlignment(GUIALIGN_LEFT);
  myBackground.setType(VolucrisGUIListEntryBackground::DARK);
  listing_id_ = other->listing_id_;
}

//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::BazaarItemListEntry::displayBazaarItem(
  const ItemForTrade* item, bool differentiate_for_owner) {
  bool differentiate = differentiate_for_owner && item->you_sell;
  bool sold = item->you_sell && item->purchased;
  price_value_ = item->price;
  owned_item_ = item->you_sell;
  VolucrisGUIItemList::ItemListEntry::displayItem(item->type, item->quantity);
  D3DCOLOR entry_color = (differentiate || sold)?
              VolucrisGUIDefs::LIST_ENTRY_TEXT_SPECIAL_COLOR :
              VolucrisGUIDefs::LIST_ENTRY_TEXT_COLOR;
  myText.setTextColor(entry_color);
  myText.setAlignment(GUIALIGN_LEFT);
  myBackground.setType(VolucrisGUIListEntryBackground::DARK);
  char price_str[128];
  if (sold) {
  sprintf_s(price_str, "(sold) %lu", item->price);
  listing_id_ = 0; // reset the listing ID so that a "redeem" message gets sent
  } else {
  sprintf_s(
    price_str,
    "%s%lu%s",
    differentiate ? "(" : "", item->price, differentiate ? ")" : "");
  listing_id_ = item->listing_id;
  }
  price_text_.setTextColor(entry_color);
  price_text_.setText(price_str);
}



//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::BazaarItemListEntry::registerElements(
  GUILayer* layer) {
  price_text_.registerElements(layer);
  VolucrisGUIItemList::ItemListEntry::registerElements(layer);
}

//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::BazaarItemListEntry::unregisterElements(
  GUILayer* layer) {
  price_text_.unregisterElements(layer);
  VolucrisGUIItemList::ItemListEntry::unregisterElements(layer);
}

//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::BazaarItemListEntry::onMouseAction(
  MouseAction action) {
  if (action == CLICKED && owner_ && owner_->owner_) {
  if (owned_item_) {
    if (listing_id_ != 0) {
    owner_->owner_->withdraw(listing_id_); // this is our item, so pull it out
    } else {
    owner_->owner_->redeem(); // redeem sales
    }
  } else {
    owner_->owner_->buy(price_value_, listing_id_); // purchase this item
  }
  }
  VolucrisGUIItemList::ItemListEntry::onMouseAction(action);
}

//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::BazaarItemListEntry::refresh() {
  price_text_.setClippingArea(getClippingArea());
  VolucrisGUIItemList::ItemListEntry::refresh();
}


//-----------------------------------------------------------------------------
BazaarDisplay::BazaarItemList::BazaarItemList(BazaarDisplay* owner)
  : slider_(this) {
  owner_ = owner;
  number_of_entries_ = 0;
  background_canvas_.setSourceCanvas(&list_canvas_);
  background_canvas_.relativeWidth(-16);
  background_canvas_.relativeHeight(-16);
  background_canvas_.alignXCenter(0);
  background_canvas_.alignYCenter(0);
  background_canvas_.relativeDepth(-0.1f);
  background_.setSourceCanvas(&list_canvas_);
  slider_.setListCanvases(&background_canvas_);
}


//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::create(
  GUIFont* font) {
  for (int i = 0; i < Economy::MAX_ITEMS_PER_BAZAAR_PAGE; ++i) {
  entries_[i].setOwner(this);
  entries_[i].initialize(this, font);
  }
  setLineHeight(font->getLineHeightInPixels());

  slider_.setParentMouseSentinel(&mouse_sentinel_);
  GUIList::getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  background_.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
  background_.setSourceRegion(&VolucrisGUIDefs::CONTENT_COMPONENT_BACKGROUND, 8, VolucrisGUIDefs::TEXTURE_SIZE);
}


//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::destroy() {
}


//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::show(GUILayer* layer) {
  if (!layer) return;
  slider_.registerElements(layer);
  layer->registerElement(&background_);
  registerElements(layer);
}


//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::hide(GUILayer* layer) {
  if (!layer) return;
  slider_.unregisterElements(layer);
  layer->unregisterElement(&background_);
  unregisterElements(layer);
}




//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::updateItemList(
  GUILayer* layer,
  bool reregister_elements,
  bool display_seller_status,
  ItemForTrade* items,
  int number_of_items) {
  if (layer) hide(layer);
  updateItemList(display_seller_status, items, number_of_items);
  if (layer && reregister_elements) show(layer);
}

//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::clear(GUILayer* layer,
                      bool reregister_elements) {
  if (layer) hide(layer);
  number_of_entries_ = 0;
  if (layer && reregister_elements) show(layer);
}


//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::removeListing(GUILayer* layer,
                          bool reregister_elements,
                          unsigned int listing_id) {
  if (layer) hide(layer);
  bool found_listing = false;
  int i = 0;
  for (; i < number_of_entries_; ++i) { // search until we find the listing indicated
  if (entries_[i].getListingID() == listing_id) {
    found_listing = true;
    break;
  }
  }
  for (;found_listing && i+1 < number_of_entries_; ++i) {
  entries_[i].copyDisplay(&entries_[i+1]);
  }
  if (found_listing) number_of_entries_--;
  if (layer && reregister_elements) show(layer);
}


//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::updateItemList(
  bool display_seller_status,
  ItemForTrade* items,
  int number_of_items) {
  number_of_entries_ = number_of_items;
  for (int i = 0; i < number_of_items; ++i) {
  entries_[i].displayBazaarItem(&items[i], display_seller_status);
  }
}

//-----------------------------------------------------------------------------
void BazaarDisplay::BazaarItemList::onChangeMouseOverEntry(
  ItemListEntry* entry) {
  if (!owner_) return;
  CONFIRM (owner_->display_mode_ == BAZAAR_MODE_VIEWSALES ||
       owner_->display_mode_ == BAZAAR_MODE_BROWSE) else return;
  owner_->item_description_.setText(entry?entry->getDisplayedItemDescription()->description:"");
}

//-----------------------------------------------------------------------------
void BazaarDisplay::changeMode(BazaarDisplayMode mode) {
  switch (display_mode_) {
  case BAZAAR_MODE_BROWSE:
  bazaar_items_.getMouseSentinel()->removeFromMouseSentinelChain();
  page_forward_.getMouseSentinel()->removeFromMouseSentinelChain();
  page_refresh_.getMouseSentinel()->removeFromMouseSentinelChain();
  page_backward_.getMouseSentinel()->removeFromMouseSentinelChain();
  break;
  case BAZAAR_MODE_LISTFORSALE:
  list_for_sale_.getMouseSentinel()->removeFromMouseSentinelChain();
  inventory_items_.getMouseSentinel()->removeFromMouseSentinelChain();
  break;
  case BAZAAR_MODE_VIEWSALES:
  list_for_sale_.getMouseSentinel()->removeFromMouseSentinelChain();
  bazaar_items_.getMouseSentinel()->removeFromMouseSentinelChain();
  break;
  case BAZAAR_MODE_LISTFORSALE_INPUTITEMPRICE:
  listing_price_.getMouseSentinel()->removeFromMouseSentinelChain();
  listing_price_.getKeyboardSentinel()->removeFromKeyboardSentinelChain();
  listing_confirm_.getMouseSentinel()->removeFromMouseSentinelChain();
  listing_cancel_.getMouseSentinel()->removeFromMouseSentinelChain();
  inventory_items_.deselectAll();
  listing_price_.releaseFocus();
  break;
  }
  switch (mode) {
  case BAZAAR_MODE_BROWSE:
  bazaar_items_panel_.setText("Bazaar - Browse");
  if (user_action_queue_) {
    // get listings in the bazaar
    user_action_queue_->addBazaarGetItemsForSale()->minimum_listing_id_ = getPageListingID();
  }
  buy_sell_toggle_button_.setText("Go to Merchant Menu");
  bazaar_items_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  page_forward_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  page_refresh_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  page_backward_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  break;
  case BAZAAR_MODE_LISTFORSALE:
  bazaar_items_panel_.setText("Bazaar - List Items for Sale");
  list_for_sale_.setText("View Sales");
  buy_sell_toggle_button_.setText("Browse & Purchase Items");
  inventory_items_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  list_for_sale_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  break;
  case BAZAAR_MODE_VIEWSALES: 
  bazaar_items_panel_.setText("Bazaar - Your Sales and Listings");
  if (user_action_queue_) user_action_queue_->addTypeOnlyAction(BAZAAR_GET_SELLER_ITEMS);
  list_for_sale_.setText("List for Sale");
  buy_sell_toggle_button_.setText("Browse & Purchase Items");
  list_for_sale_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  bazaar_items_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  break;
  case BAZAAR_MODE_LISTFORSALE_INPUTITEMPRICE:
  listing_price_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  listing_price_.getKeyboardSentinel()->setParentKeyboardSentinel(&keyboard_sentinel_);
  listing_confirm_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  listing_cancel_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  listing_price_.setText("");
  listing_prompt_.setText("Price?");
  listing_price_.obtainFocus();
  listing_price_.obtainFocus();
  break;
  }

  if (layer_) {
  switch (display_mode_) {
  case BAZAAR_MODE_BROWSE:
    bazaar_items_.hide(layer_);
    page_forward_.unregisterElements(layer_);
    page_refresh_.unregisterElements(layer_);
    page_backward_.unregisterElements(layer_);
    break;
  case BAZAAR_MODE_LISTFORSALE:
    inventory_items_.hide();
    list_for_sale_.unregisterElements(layer_);
    break;
  case BAZAAR_MODE_VIEWSALES:
    bazaar_items_.hide(layer_);
    list_for_sale_.unregisterElements(layer_);
    break;
  case BAZAAR_MODE_LISTFORSALE_INPUTITEMPRICE:
    listing_price_.unregisterElements(layer_);
    listing_confirm_.unregisterElements(layer_);
    listing_cancel_.unregisterElements(layer_);
    break;
  }
  switch (mode) {
  case BAZAAR_MODE_BROWSE:
    page_forward_.registerElements(layer_);
    page_refresh_.registerElements(layer_);
    page_backward_.registerElements(layer_);
    bazaar_items_.show(layer_);
    break;
  case BAZAAR_MODE_LISTFORSALE:
    inventory_items_.show();
    list_for_sale_.registerElements(layer_);
    break;
  case BAZAAR_MODE_VIEWSALES:
    bazaar_items_.show(layer_);
    list_for_sale_.registerElements(layer_);
    break;
  case BAZAAR_MODE_LISTFORSALE_INPUTITEMPRICE:
    listing_price_.registerElements(layer_);
    listing_confirm_.registerElements(layer_);
    listing_cancel_.registerElements(layer_);
    break;
  }
  }

  display_mode_ = mode;
}


//-----------------------------------------------------------------------------
BazaarDisplay::BazaarDisplay() :
  bazaar_items_(this),
  keyboard_sentinel_(this),
  inventory_items_(this) {
  layer_ = NULL;
  finished_button_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  buy_sell_toggle_button_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  user_action_queue_ = NULL;
  item_description_array_ = NULL;
  available_money_ = 0;
  current_page_index_ = 0;
}

//-----------------------------------------------------------------------------
BazaarDisplay::~BazaarDisplay() {
  destroy();
}

//-----------------------------------------------------------------------------
bool BazaarDisplay::create(
  const ClientItemDescription* item_descriptions,
  size_t num_item_descriptions,
  GUIFont* font,
  GUIMouseSentinel* parent_mouse_sentinel,
  GUIKeyboardSentinel* parent_keyboard_sentinel,
  UserEventQueue* user_action_queue,
  GUILayer* layer) {

  destroy();

  { // align the buttons
  finished_button_.getCanvas()->setSourceCanvas(bazaar_items_panel_.getCanvas());
  finished_button_.getCanvas()->alignXRight(-10);
  finished_button_.getCanvas()->alignYBottomOutside(+5);
  finished_button_.getCanvas()->absoluteWidth(220);
  finished_button_.getCanvas()->absoluteHeight(35);
  finished_button_.getCanvas()->absoluteDepth(0.8f);
  }

  listing_confirm_.setText("List For Sale");
  listing_cancel_.setText("Go Back");

  listing_prompt_canvas_.setSourceCanvas(inventory_items_.getCanvas());
  listing_prompt_canvas_.alignXLeft(20);
  listing_prompt_canvas_.alignYCenter(-200);
  listing_prompt_canvas_.absoluteWidth(300);
  listing_prompt_canvas_.absoluteHeight(30);
  listing_prompt_canvas_.relativeDepth(-0.1f);
  listing_prompt_.setSourceCanvas(&listing_prompt_canvas_);
  listing_prompt_.setFont(font);
  listing_prompt_.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
  listing_prompt_.setText("How much?");

  listing_price_.configure(font, false, true, VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR, 10);
  listing_price_.getCanvas()->setSourceCanvas(inventory_items_.getCanvas());
  listing_price_.getCanvas()->alignXLeft(20);
  listing_price_.getCanvas()->alignYCenter(-100);
  listing_price_.getCanvas()->absoluteWidth(300);
  listing_price_.getCanvas()->absoluteHeight(35);
  listing_price_.getCanvas()->relativeDepth(-0.1f);

  listing_confirm_.getCanvas()->setSourceCanvas(inventory_items_.getCanvas());
  listing_confirm_.getCanvas()->alignXRight(-10);
  listing_confirm_.getCanvas()->alignYCenter(-35-10);
  listing_confirm_.getCanvas()->absoluteWidth(200);
  listing_confirm_.getCanvas()->absoluteHeight(35);
  listing_confirm_.getCanvas()->relativeDepth(-0.1f);
  listing_confirm_.setFont(font);

  listing_confirm_.enable();

  listing_cancel_.getCanvas()->setSourceCanvas(inventory_items_.getCanvas());
  listing_cancel_.getCanvas()->alignXRight(-10);
  listing_cancel_.getCanvas()->absoluteWidth(200);
  listing_cancel_.getCanvas()->absoluteHeight(35);
  listing_cancel_.getCanvas()->alignYCenter(0);
  listing_cancel_.getCanvas()->relativeDepth(-0.1f);
  listing_cancel_.setFont(font);

  listing_cancel_.enable();


  GUISize layer_size = layer->getPrimaryCanvas()->getArea()->calculateSize();

  item_description_background_canvas_.setSourceCanvas(bazaar_items_panel_.getCanvas());
  item_description_background_canvas_.alignXRight(-20);
  item_description_background_canvas_.alignYTop(25);
  item_description_background_canvas_.absoluteWidth(300);
  item_description_background_canvas_.relativeHeight(-200);
  item_description_background_canvas_.relativeDepth(-0.1f);
  item_description_background_.setSourceCanvas(&item_description_background_canvas_);
  item_description_background_.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
  item_description_background_.setSourceRegion(&VolucrisGUIDefs::CONTENT_COMPONENT_BACKGROUND, 8, VolucrisGUIDefs::TEXTURE_SIZE);

  item_description_canvas_.setSourceCanvas(&item_description_background_canvas_);
  item_description_canvas_.alignXCenter(0);
  item_description_canvas_.alignYCenter(0);
  item_description_canvas_.relativeWidth(-16);
  item_description_canvas_.relativeHeight(-16);
  item_description_canvas_.relativeDepth(-0.1f);
  item_description_.setSourceCanvas(&item_description_canvas_);
  item_description_.setFont(font);
  item_description_.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
  item_description_.setAlignment(GUIALIGN_LEFT_TOP);
  item_description_.setClippingArea(item_description_canvas_.getArea());

  bazaar_items_panel_.getCanvas()->absoluteWidth(min(layer_size.width, 800)/2 + 400 - 60);
  bazaar_items_panel_.getCanvas()->absoluteHeight(min(layer_size.height,600)/2 + 300 - 100);
  bazaar_items_panel_.getCanvas()->alignXCenter(0);
  bazaar_items_panel_.getCanvas()->alignYCenter(0);
  bazaar_items_panel_.getCanvas()->absoluteDepth(0.8f);
  bazaar_items_panel_.setFont(font);

  inventory_items_.getCanvas()->setSourceCanvas(bazaar_items_panel_.getCanvas());
  inventory_items_.getCanvas()->alignXLeft(+20);
  inventory_items_.getCanvas()->alignYTop(+25);
  inventory_items_.getCanvas()->relativeWidth(-350);
  inventory_items_.getCanvas()->relativeHeight(-140);
  inventory_items_.getCanvas()->relativeDepth(-0.1f);
  inventory_items_.create(font, layer);

  bazaar_items_.getCanvas()->setSourceCanvas(bazaar_items_panel_.getCanvas());
  bazaar_items_.getCanvas()->alignXLeft(+20);
  bazaar_items_.getCanvas()->alignYTop(+25);
  bazaar_items_.getCanvas()->relativeWidth(-350);
  bazaar_items_.getCanvas()->relativeHeight(-140);
  bazaar_items_.getCanvas()->relativeDepth(-0.1f);
  bazaar_items_.create(font);
  bazaar_items_.setItemDescriptions(item_descriptions, num_item_descriptions);

  money_text_canvas_.setSourceCanvas(bazaar_items_panel_.getCanvas());
  money_text_canvas_.alignYBottom(-20);
  money_text_canvas_.alignXRight(+20);
  money_text_canvas_.absoluteWidth(300);
  money_text_canvas_.absoluteHeight(30);
  money_text_canvas_.relativeDepth(-0.1f);
  money_text_.setSourceCanvas(&money_text_canvas_);
  money_text_.setAlignment(GUIALIGN_LEFT_TOP);

  buy_sell_toggle_button_.getCanvas()->setSourceCanvas(inventory_items_.getCanvas());
  buy_sell_toggle_button_.getCanvas()->alignXCenter(0);
  buy_sell_toggle_button_.getCanvas()->alignYBottomOutside(+35+5+20);
  buy_sell_toggle_button_.getCanvas()->absoluteWidth(300);
  buy_sell_toggle_button_.getCanvas()->absoluteHeight(35);
  buy_sell_toggle_button_.getCanvas()->relativeDepth(-0.1f);
  buy_sell_toggle_button_.setFont(font);


  list_for_sale_.getCanvas()->setSourceCanvas(inventory_items_.getCanvas());
  list_for_sale_.getCanvas()->alignXCenter(0);
  list_for_sale_.getCanvas()->alignYBottomOutside(+15);
  list_for_sale_.getCanvas()->absoluteWidth(300);
  list_for_sale_.getCanvas()->absoluteHeight(35);
  list_for_sale_.getCanvas()->relativeDepth(-0.1f);

  list_for_sale_.setFont(font);

  page_forward_.getCanvas()->setSourceCanvas(inventory_items_.getCanvas());
  page_forward_.getCanvas()->alignXCenter(+125);
  page_forward_.getCanvas()->alignYBottomOutside(+15);
  page_forward_.getCanvas()->absoluteWidth(45);
  page_forward_.getCanvas()->absoluteHeight(35);
  page_forward_.getCanvas()->relativeDepth(-0.1f);

  page_forward_.setFont(font);
  page_forward_.setText(">");

  page_refresh_.getCanvas()->setSourceCanvas(inventory_items_.getCanvas());
  page_refresh_.getCanvas()->alignXCenter(0);
  page_refresh_.getCanvas()->alignYBottomOutside(+15);
  page_refresh_.getCanvas()->absoluteWidth(165);
  page_refresh_.getCanvas()->absoluteHeight(35);
  page_refresh_.getCanvas()->relativeDepth(-0.1f);

  page_refresh_.setFont(font);
  page_refresh_.setText("refresh");

  page_backward_.getCanvas()->setSourceCanvas(inventory_items_.getCanvas());
  page_backward_.getCanvas()->alignXCenter(-125);
  page_backward_.getCanvas()->alignYBottomOutside(+15);
  page_backward_.getCanvas()->absoluteWidth(45);
  page_backward_.getCanvas()->absoluteHeight(35);
  page_backward_.getCanvas()->relativeDepth(-0.1f);

  page_backward_.setFont(font);
  page_backward_.setText("<");

  user_action_queue_ = user_action_queue;
  item_description_array_ = item_descriptions;
  num_item_descriptions_ = num_item_descriptions;

  listings_.clear();

  finished_button_.setFont(font);

  finished_button_.setText("Leave Bazaar");


  mouse_sentinel_.setParentMouseSentinel(parent_mouse_sentinel);
  keyboard_sentinel_.setParentKeyboardSentinel(parent_keyboard_sentinel);
  mouse_sentinel_.enable(false);
  keyboard_sentinel_.enable(false);

  money_text_.setFont(font);
  money_text_.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
  updateMoney(available_money_);

  changeMode(BAZAAR_MODE_BROWSE);

  return true;
}

//-----------------------------------------------------------------------------
void BazaarDisplay::destroy() {
  hide();
  mouse_sentinel_.removeFromMouseSentinelChain();
  keyboard_sentinel_.removeFromKeyboardSentinelChain();

  bazaar_items_.destroy();
}

//-----------------------------------------------------------------------------
void BazaarDisplay::show(GUILayer* layer) {
  if (layer == layer_) return;

  if (layer_) {
  inventory_items_.hide();
  bazaar_items_panel_.unregisterElements(layer_);
  bazaar_items_.hide(layer_);
  buy_sell_toggle_button_.unregisterElements(layer_);
  finished_button_.unregisterElements(layer_);
  layer_->unregisterElement(&money_text_);
  layer_->unregisterElement(&item_description_);
  layer_->unregisterElement(&item_description_background_);
  list_for_sale_.unregisterElements(layer_);
  layer_->unregisterElement(&listing_prompt_);
  listing_price_.unregisterElements(layer_);
  listing_confirm_.unregisterElements(layer_);
  listing_cancel_.unregisterElements(layer_);
  page_forward_.unregisterElements(layer_);
  page_backward_.unregisterElements(layer_);
  page_refresh_.unregisterElements(layer_);
  }

  layer_ = layer;
  if (layer) {
  changeMode(display_mode_);

  bazaar_items_panel_.registerElements(layer);
  buy_sell_toggle_button_.registerElements(layer);
  finished_button_.registerElements(layer);
  layer->registerElement(&money_text_);
  layer->registerElement(&item_description_);
  layer->registerElement(&item_description_background_);
  }

  GUICanvas* new_root_canvas = layer ? layer->getPrimaryCanvas() : NULL;
  //finished_button_.getCanvas()->setSourceCanvas(new_root_canvas);
  bazaar_items_panel_.getCanvas()->setSourceCanvas(new_root_canvas);

  bool is_enabled = layer != NULL;
  if (is_enabled) {
  mouse_sentinel_.obtainMouseFocus();
  } else {
  mouse_sentinel_.releaseMouseFocus();
  }
  mouse_sentinel_.enable(is_enabled);
  keyboard_sentinel_.enable(is_enabled);
}


//-----------------------------------------------------------------------------
void BazaarDisplay::updateInventory(
  ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE],
  int update_indices[AVATAR_INVENTORY_SIZE]) {
  unsigned int first_line_index;
  int first_line_offset;
  inventory_items_.getScrollLine(&first_line_index, &first_line_offset);
  inventory_items_.initialize(
    inventory,
    update_indices,
    item_description_array_,
    num_item_descriptions_,
    false,
    true);
  inventory_items_.scrollToLine(first_line_index, first_line_offset);
  inventory_items_.refresh();
  if (layer_ && display_mode_==BAZAAR_MODE_LISTFORSALE) inventory_items_.show();
}


//-----------------------------------------------------------------------------
void BazaarDisplay::updateBazaar(ItemForTrade* items_to_display, int number_of_items) {
  if (display_mode_ != BAZAAR_MODE_BROWSE) return; // only handle if browsing

  for (int i = 0; i < number_of_items; ++i) {
  listings_.insert(items_to_display[i].listing_id);
  }

  bazaar_items_.updateItemList(layer_, display_mode_==BAZAAR_MODE_BROWSE, true, items_to_display, number_of_items);
  bazaar_items_.scrollToLine(0, 0);
  bazaar_items_.refresh();
  bazaar_items_.syncSlider();
}


//-----------------------------------------------------------------------------
void BazaarDisplay::updateSellerStatus(ItemForTrade* items_being_sold, int number_of_items) {
  if (display_mode_ != BAZAAR_MODE_VIEWSALES) return; // only handle if in sales mode
  unsigned int first_line_index;
  int first_line_offset;
  bazaar_items_.getScrollLine(&first_line_index, &first_line_offset);
  bazaar_items_.updateItemList(layer_, display_mode_==BAZAAR_MODE_VIEWSALES, false, items_being_sold, number_of_items);
  bazaar_items_.scrollToLine(0, 0);
  bazaar_items_.refresh();
  bazaar_items_.syncSlider();
}


//-----------------------------------------------------------------------------
void BazaarDisplay::updateMoney(unsigned int gold) {
  available_money_ = gold;
  char money_str[128];
  sprintf_s(money_str, "Gold:  %lu", gold);
  money_text_.setText(money_str);
}

//-----------------------------------------------------------------------------
void BazaarDisplay::removeListing(unsigned int listing_id) {
  bazaar_items_.removeListing(layer_, layer_ != NULL, listing_id);
  listings_.erase(listing_id);
}

//-----------------------------------------------------------------------------
void BazaarDisplay::updateUserEvents() {
  if (!layer_) return; // only process if active
  if (finished_button_.wasPushed()) {
  finished_button_.resetPushed();
  user_action_queue_->addTypeOnlyAction(CLOSE_BAZAAR);
  }

  if (buy_sell_toggle_button_.wasPushed()) {
  buy_sell_toggle_button_.resetPushed();
  changeMode(display_mode_ == BAZAAR_MODE_BROWSE ? BAZAAR_MODE_VIEWSALES : BAZAAR_MODE_BROWSE);
  }
  if (list_for_sale_.wasPushed()) {
  list_for_sale_.resetPushed();
  changeMode(display_mode_ == BAZAAR_MODE_VIEWSALES ? BAZAAR_MODE_LISTFORSALE : BAZAAR_MODE_VIEWSALES);
  }

  switch (display_mode_) {
  case BAZAAR_MODE_LISTFORSALE_INPUTITEMPRICE: {
    if (listing_cancel_.wasPushed()) {
    listing_cancel_.resetPushed();
    changeMode(BAZAAR_MODE_LISTFORSALE);
    }
    else if (listing_confirm_.wasPushed()) {
    listing_confirm_.resetPushed();

    // Get the price and inventory index
    UserEventBazaarListItem* action = user_action_queue_->addBazaarListItem();
    action->inventory_index_ = listing_selected_item_inventory_index_;
    sscanf_s(listing_price_.getText(), "%lu", &action->price_);
    listing_selected_item_inventory_index_ = 0;
    changeMode(BAZAAR_MODE_LISTFORSALE);
    }
  } break;

  case BAZAAR_MODE_BROWSE: {
    int new_page_index;
    VolucrisGUIButton* src_button;
    bool change_page = false;
    if (page_forward_.wasPushed()) {
    page_forward_.resetPushed();
    src_button = &page_forward_;
    change_page = true;  
    new_page_index = current_page_index_ + 1;
    }
    if (page_backward_.wasPushed()) {
    page_backward_.resetPushed();
    src_button = &page_backward_;
    change_page = true;
    new_page_index = current_page_index_ - 1;
    }
    if (change_page && ((listings_.size() / Economy::MAX_ITEMS_PER_BAZAAR_PAGE >= new_page_index) && (new_page_index >= 0))) {
    current_page_index_ = new_page_index;
    UserEventBazaarGetItemsForSale* sale = user_action_queue_->addBazaarGetItemsForSale();
    sale->minimum_listing_id_ = getPageListingID();
    bazaar_items_.clear(layer_, true);
    }
    if (page_refresh_.wasPushed()) {
    page_refresh_.resetPushed();
    UserEventBazaarGetItemsForSale* sale = user_action_queue_->addBazaarGetItemsForSale();
    sale->minimum_listing_id_ = 0;
    bazaar_items_.clear(layer_, true);
    current_page_index_ = 0;
    listings_.clear();
    }
  } break;
  }
}


//-----------------------------------------------------------------------------
void BazaarDisplay::listForSale(unsigned char inventory_index) {
  //UserEventBazaarListItem* item = user_action_queue_->addBazaarListItem();
  //item->inventory_index_ = inventory_index;
  //item->price_ = 100; // TODO: ask user for price!
  CONFIRM(display_mode_ == BAZAAR_MODE_LISTFORSALE) else {
  inventory_items_.deselectAll();
  return;
  }
  listing_selected_item_inventory_index_ = inventory_index;
  changeMode(BAZAAR_MODE_LISTFORSALE_INPUTITEMPRICE);
}

//-----------------------------------------------------------------------------
void BazaarDisplay::buy(unsigned int price, unsigned int listing_id) {
  if (price < available_money_) {
  UserEventBazaarBuyItem* item = user_action_queue_->addBazaarBuyItem();
  item->listing_id_ = listing_id;
  }
}

//-----------------------------------------------------------------------------
void BazaarDisplay::withdraw(unsigned int listing_id) {
  UserEventBazaarWithdrawItem* item = user_action_queue_->addBazaarWithdrawItem();
  item->listing_id_ = listing_id;
}

//-----------------------------------------------------------------------------
void BazaarDisplay::redeem() {
  user_action_queue_->addTypeOnlyAction(BAZAAR_SELLER_REDEEM);
  bazaar_items_.clear(layer_, true);
}