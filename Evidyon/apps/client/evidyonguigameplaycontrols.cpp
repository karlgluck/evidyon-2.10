#include "evidyonguigameplaycontrols.h"
#include "../common/gui/guilayer.h"
#include <dc/debug>
#include <dc/filestream>


namespace Evidyon {


EvidyonGUIGameplayControls::MouseSentinel::MouseSentinel(EvidyonGUIGameplayControls* owner) {
  owner_ = owner;
}


bool EvidyonGUIGameplayControls::MouseSentinel::onMouseButtonAction(GUIMouseButtonAction action,
                                                                    GUIPoint position,
                                                                    GUIMouseInputSource* mouse) {
  if (owner_->isBinding()) {
    if ((action == GUIMOUSE_DOWN || action == GUIMOUSE_ALT_DOWN)) {
      owner_->cancelBinding();
    }
  } else {
    bool return_value = GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
    if (action == GUIMOUSE_CLICK_ALT_SINGLE) {
      obtainMouseFocus();
      mouse->click();
      started_binding_position_ = position;
      owner_->startBinding();
      if (!owner_->isBinding()) { // if nothing is being bound, move the mouse so that highlighting works again
        mouse->generateMovementUpdate();
      }
    } else {
      return return_value;
    }
  }

  // eat this message
  return true;
}


bool EvidyonGUIGameplayControls::MouseSentinel::onMouseMove(GUIPoint position,
                                                            GUIPoint oldPosition,
                                                            GUIMouseInputSource* mouse) {
  if (owner_->isBinding()) {
    int x = position.x - started_binding_position_.x;
    int y = position.y - started_binding_position_.y;
    if (abs(x) > 10 || abs(y) > 10) owner_->cancelBinding();
  }

  return GUIMouseSentinel::onMouseMove(position, oldPosition, mouse);
}



EvidyonGUIGameplayControls::KeyboardSentinel::KeyboardSentinel(EvidyonGUIGameplayControls* owner) {
  owner_ = owner;
}


bool EvidyonGUIGameplayControls::KeyboardSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (owner_->isBinding()) {
    owner_->bind(key);
    return true;
  } else {
    return GUIKeyboardSentinel::onKeyDown(key, keyboard);
  }
}




void EvidyonGUIGameplayControls::startBinding() {
  tooltip_box_.becomeOwner(&mouse_sentinel_);
  tooltip_box_.addContent(&binding_description_);
  tooltip_box_.showContentAfter(0.0);

  keyboard_sentinel_.obtainKeyboardFocus();


  const ClientSpellDescription* spell;
  int index;

  if (spells_.getClickedSpell(&index, &spell)) {
    std::string text = "Press key to bind ";
    text.append(spell->name);
    text.append("\n[del] - erase [esc] - cancel");
    GUISize panel_size;
    binding_description_.set(text, &panel_size);
    tooltip_box_.setContentSize(panel_size);
    current_binding_.action.type = Action::TYPE_SPELL;
    current_binding_.action.spell.spell_type = index;
    return;
  }

  // bind a consumable
  const ClientAvatarInventoryItem* inventory_item;
  const ClientItemDescription* item_description;
  if (unequipped_.getClickedItem(&inventory_item, &item_description) && // check before dialog!
      current_dialog_ == DIALOG_USE_ITEM) {
    std::string text = "Press key to bind to ";
    text.append(item_description->name);
    text.append("\n[del] - erase [esc] - cancel");
    GUISize panel_size;
    binding_description_.set(text, &panel_size);
    tooltip_box_.setContentSize(panel_size);
    current_binding_.action.type = Action::TYPE_CONSUMABLE;
    current_binding_.action.consumable.item_type = inventory_item->type;
    return;
  }

  // clear out the other lists to make sure the user doesn't accidentally
  // perform an action they were not intending.
  equipped_.getClickedItem(NULL, NULL);
  storage_.getClickedItem(NULL, NULL);

  // try finding a basic event type
  UserEventType user_event_type = INVALID_ACTION;
  const char* desc = NULL;
  EvidyonGUIGameDialogMenu::Button button = dialog_menu_.pollSelectionButtons();
  if (button < EvidyonGUIGameDialogMenu::NUMBER_OF_MENU_BUTTONS) {
    std::string text = "Press key to bind 'open ";
    text.append(EvidyonGUIGameDialogMenu::getButtonText(button));
    text.append(" screen");
    text.append("'\n[del] - erase [esc] - cancel");
    GUISize panel_size;
    binding_description_.set(text, &panel_size);
    tooltip_box_.setContentSize(panel_size);
    current_binding_.action.type = Action::TYPE_USEREVENT;
    current_binding_.action.user_event = EvidyonGUIGameDialogMenu::getButtonEventType(button);
    return;
  }

  cancelBinding();
}



void EvidyonGUIGameplayControls::cancelBinding() {
  keyboard_sentinel_.releaseKeyboardFocus();
  tooltip_box_.releaseOwnership(&mouse_sentinel_);
}


bool EvidyonGUIGameplayControls::isBinding() {
  return tooltip_box_.hasOwnership(&mouse_sentinel_);
}


void EvidyonGUIGameplayControls::bind(GUIKey key) {
  if (key == GUIKEY_ESCAPE) {
    cancelBinding();
    return;
  }

  key_bound_actions_.setKey(key_bound_actions_.getKeyForAction(&current_binding_.action), NULL);

  if (key != GUIKEY_DELETE) {
    key_bound_actions_.setKey(key, &current_binding_.action);
  }


  // if the user tries to unequip all of the dialog menu keys, re-equip
  // at least 1 key
  {
    int i = 0;
    for (; i < EvidyonGUIGameDialogMenu::NUMBER_OF_MENU_BUTTONS; ++i) {
      if (GUIKEY_NULL != key_bound_actions_.getKeyForUserEvent(
                            EvidyonGUIGameDialogMenu::getButtonEventType(
                              (EvidyonGUIGameDialogMenu::Button)i))) {
        break;
      }
    }
    if (i == EvidyonGUIGameDialogMenu::NUMBER_OF_MENU_BUTTONS) {
      Evidyon::Action action;
      action.type = Action::TYPE_USEREVENT;
      action.user_event = OPEN_INVENTORY;
      key_bound_actions_.setKey(GUIKEY_I, &action);
    }
  }

  dialog_menu_.sync(&key_bound_actions_);
  spells_.sync(&key_bound_actions_, spell_availability_mask_);

  if (current_dialog_ == DIALOG_USE_ITEM) { // update consumable key bindings
    unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_CONSUMABLES, &key_bound_actions_);
  }

  cancelBinding();
}


void EvidyonGUIGameplayControls::saveKeyBindings() {
  dc::dcFileStream fs;
  fs.open(keys_file_.c_str(), STREAM_OVERWRITE);
  key_bound_actions_.saveKeyBindings(&fs);
  fs.close();
}


EvidyonGUIGameplayControls::EvidyonGUIGameplayControls()
    : keyboard_sentinel_(this),
      mouse_sentinel_(this),
      key_bound_actions_(&targeter_) {
  primary_layer_ = NULL;
  spell_availability_mask_ = NULL;

  key_bound_actions_.setParentKeyboardSentinel(&keyboard_sentinel_);
  last_update_cursor_targeting_ = false;
}


void EvidyonGUIGameplayControls::create(const std::string& keys_file,
                                        GUIFont* font,
                                        GUILayer* primary_layer,
                                        GUIMouseSentinel* root_mouse_sentinel,
                                        GUIKeyboardSentinel* root_keyboard_sentinel,
                                        GUIGenericCanvas* mouse_cursor_canvas,
                                        const ClientItemDescription* item_descriptions,
                                        size_t number_of_item_descriptions,
                                        const ClientAvatarInventoryItem* inventory,
                                        const ClientSpellDescription* spells,
                                        size_t number_of_spells,
                                        SpellAvailabilityMask* spell_availability_mask,
                                        GUISize hpMpXpSize,
                                        IsometricCamera* camera,
                                        GlobalActorManager* global_actor_manager,
                                        const ItemInStorage* storage) {
  keys_file_ = keys_file;
  primary_layer_ = primary_layer;
  keyboard_sentinel_.setParentKeyboardSentinel(root_keyboard_sentinel);
  mouse_sentinel_.setParentMouseSentinel(root_mouse_sentinel);
  dialog_menu_.setFont(font);
  dialog_menu_.setParentSentinels(&mouse_sentinel_, NULL);
  spells_.create(primary_layer->getPrimaryCanvas(), font, spells, number_of_spells, &tooltip_box_);
  spells_.setParentSentinels(&mouse_sentinel_, NULL);
  equipped_.create(primary_layer->getPrimaryCanvas(), font, item_descriptions, number_of_item_descriptions, inventory, &tooltip_box_);
  equipped_.setParentSentinels(&mouse_sentinel_, NULL);
  unequipped_.create(primary_layer->getPrimaryCanvas(), font, item_descriptions, number_of_item_descriptions, inventory, &tooltip_box_);
  unequipped_.setParentSentinels(&mouse_sentinel_, NULL);
  tooltip_box_.create(primary_layer, 20, mouse_cursor_canvas);
  spell_availability_mask_ = spell_availability_mask;


  chat_text_line_.create(font, &mouse_sentinel_, &keyboard_sentinel_, hpMpXpSize);

  stat_display_.create(font, font, primary_layer, &mouse_sentinel_, &keyboard_sentinel_, NULL);

  targeter_.create(camera, global_actor_manager);

  targeter_.setParentMouseSentinel(root_mouse_sentinel);
  targeter_.disable();

  storage_.create(primary_layer->getPrimaryCanvas(), font, item_descriptions, number_of_item_descriptions, storage, &tooltip_box_);
  storage_.setParentSentinels(&mouse_sentinel_, NULL);

  {
    GUICanvas* canvas = dialog_menu_.getCanvas();
    canvas->setSourceCanvas(primary_layer->getPrimaryCanvas());
    canvas->alignXLeft(0);
    canvas->alignYTop(0);
    canvas->absoluteDepth(0.2f);
    canvas->relativeHeight(0);
    canvas->relativeWidth(0);
  }

  binding_description_.setFont(font);
  key_bound_actions_.create(inventory, item_descriptions, spells);

  {
    current_dialog_ = DIALOG_INVENTORY; // make sure hideDialogs does something
    hideDialogs();
  }

  {
    bool keys_reset = true;
    dc::dcFileStream fs;
    if (fs.open(keys_file.c_str(), 0)) {
      keys_reset = false == key_bound_actions_.readKeyBindings(&fs);
    }
    fs.close();
    if (keys_reset) { // first-login, display inventory
      showDialog(EvidyonGUIGameplayControls::DIALOG_INVENTORY);
    }
  }
}


void EvidyonGUIGameplayControls::destroy() {
  saveKeyBindings();
  if (primary_layer_) {
    dialog_menu_.unregisterElements(primary_layer_);
  }
  spells_.destroy();
  equipped_.destroy();
  unequipped_.destroy();
  tooltip_box_.destroy();
  chat_text_line_.destroy();
  stat_display_.destroy();
  storage_.destroy();
}




int EvidyonGUIGameplayControls::getCursorState() const {
  return last_update_cursor_targeting_ ? 1 : 0;
}


Action::Type EvidyonGUIGameplayControls::update(double time,
                                                const ActionInstance** action_instance,
                                                bool* change_cursor_state) {

  if (current_dialog_ != DIALOG_NONE) {

    if (isBinding()) {
      tooltip_box_.update(time);
      return Action::TYPE_INVALID;
    }

    switch (dialog_menu_.pollSelectionButtons()) {
      case EvidyonGUIGameDialogMenu::BUTTON_INVENTORY:  showDialog(DIALOG_INVENTORY); break;
      case EvidyonGUIGameDialogMenu::BUTTON_USEITEM:    showDialog(DIALOG_USE_ITEM); break;
      case EvidyonGUIGameDialogMenu::BUTTON_DROP:       showDialog(DIALOG_DROP_ITEMS); break;
      case EvidyonGUIGameDialogMenu::BUTTON_CAST:       showDialog(DIALOG_CAST_SPELL); break;
      case EvidyonGUIGameDialogMenu::BUTTON_AVATARSTATS:showDialog(DIALOG_STAT_DISPLAY); break;
      case EvidyonGUIGameDialogMenu::BUTTON_CLOSE_MENU: hideDialogs(); break;
    }
  }

  if (current_dialog_ != DIALOG_NONE) {

    if (current_dialog_ == DIALOG_CHAT) {
      UserEvent user_event;
      if (chat_text_line_.update(&user_event)) {
        if (user_event.type_ == SPEAK) {
          strcpy_s(targeter_.speak()->text_, Evidyon::MAX_SPOKEN_TEXT_CHARACTERS+1, user_event.data_.speak_.text_);
        } else {
          hideDialogs(); // i happen to "know" this is what should go here
        }
      }
    }

    if (current_dialog_ == DIALOG_STAT_DISPLAY) {
      int ability;
      if (stat_display_.pollAddAbilityPoint(&ability)) {
        targeter_.attributesAddAbilityPoint()->ability = ability;
      }
    }

    const ClientAvatarInventoryItem* inventory_item;
    const ClientItemDescription* item_description;

    if (equipped_.getClickedItem(&inventory_item, &item_description)) {
      switch (current_dialog_) {
      case DIALOG_INVENTORY:
        targeter_.unequipItem()->inventory_index_ = inventory_item->inventory_index;
        break;
      }
    }

    if (unequipped_.getClickedItem(&inventory_item, &item_description)) {
      switch (current_dialog_) {
      case DIALOG_INVENTORY: // equip
        targeter_.equipItem()->inventory_index_ = inventory_item->inventory_index;
        break;
      case DIALOG_DROP_ITEMS: // drop
        targeter_.dropItem()->inventory_index_ = inventory_item->inventory_index;
        break;
      case DIALOG_USE_ITEM: // use
        CONFIRM(item_description->consumable) else break;
        targeter_.consumeItem(inventory_item->inventory_index, item_description->consuming_requires_target);
        hideDialogs();
        break;
      case DIALOG_STORAGE:
        targeter_.storageDepositItem()->inventory_index_ = inventory_item->inventory_index;
        break;
      case DIALOG_GEOSID:
        targeter_.geosidSacrificeItem()->inventory_index_ = inventory_item->inventory_index;
        break;
      }
    }

    if (current_dialog_ == DIALOG_STORAGE) {
      const ItemInStorage* storage_item;
      if (storage_.getClickedItem(&storage_item, &item_description)) {
        targeter_.storageWithdrawItem()->inventory_index_ = storage_item->index;
      }
    }

    tooltip_box_.update(time);
    unequipped_.updateTooltip();
    equipped_.updateTooltip();
    spells_.updateTooltip();
    storage_.updateTooltip();

    const ClientSpellDescription* spell_description;

    int index;
    if (spells_.getClickedSpell(&index, &spell_description)) {
      targeter_.castSpell(index, spell_description->needs_target);
      hideDialogs();
    }
  }


  Action::Type action_type = targeter_.update(time, action_instance);
  if (action_type == Action::TYPE_MOVE) { // inspect!
    hideDialogs();
  }

  if (last_update_cursor_targeting_ != targeter_.isTargeting()) {
    last_update_cursor_targeting_ = !last_update_cursor_targeting_;
    *change_cursor_state = true;
  } else {
    *change_cursor_state = false;
  }

  return action_type;
}


void EvidyonGUIGameplayControls::syncInventory() {
  equipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_EQUIPPED, NULL);
  if (current_dialog_ == DIALOG_USE_ITEM) {
    unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_CONSUMABLES, &key_bound_actions_);
  } else {
    unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);
  }
}

void EvidyonGUIGameplayControls::syncSpells() {
  spells_.sync(&key_bound_actions_, spell_availability_mask_);
}

void EvidyonGUIGameplayControls::syncStorageItems() {
  storage_.sync();
}


void EvidyonGUIGameplayControls::toggleDialog(Dialog dialog) {
  if (current_dialog_ == dialog) {
    hideDialogs();
  } else {
    showDialog(dialog);
  }
}

void EvidyonGUIGameplayControls::showDialog(Dialog dialog) {
  hideDialogs();

  // TODO: reset the scroll bar positions of the lists here

  // make sure movement messages don't get passed
  targeter_.disable();

  switch (dialog) {
    case DIALOG_INVENTORY:
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      setSplitInventoryCanvases();
      equipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_EQUIPPED, NULL);
      equipped_.show(primary_layer_);
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);
      unequipped_.show(primary_layer_);
      break;
    case DIALOG_USE_ITEM:
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      setSingleInventoryCanvas();
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_CONSUMABLES, &key_bound_actions_);
      unequipped_.show(primary_layer_);
      break;
    case DIALOG_DROP_ITEMS:
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      setSingleInventoryCanvas();
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);
      unequipped_.show(primary_layer_);
      break;
    case DIALOG_CAST_SPELL:
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      setSpellCanvas();
      spells_.sync(&key_bound_actions_, spell_availability_mask_);
      spells_.show(primary_layer_);
      break;
    case DIALOG_STAT_DISPLAY:
      targeter_.attributesAddAbilityPoint()->ability = -1; // ask the server for a stat-screen sync
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      stat_display_.show();
      break;
    case DIALOG_CHAT:
      chat_text_line_.show(primary_layer_);
      targeter_.enable();
      break;
    case DIALOG_STORAGE:
      setStorageCanvases();
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);
      unequipped_.show(primary_layer_);
      storage_.resetStorageContent(); // clears the storage, pending server message
      storage_.show(primary_layer_);
      break;
    case DIALOG_GEOSID:
      setSingleInventoryCanvas();
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);
      unequipped_.show(primary_layer_);
      break;
  }

  current_dialog_ = dialog;
}

bool EvidyonGUIGameplayControls::hideDialogs() {
  if (current_dialog_ == DIALOG_NONE) return false;
  targeter_.enable();
  current_dialog_ = DIALOG_NONE;
  equipped_.hide();
  unequipped_.hide();
  spells_.hide();
  chat_text_line_.show(NULL);
  dialog_menu_.unregisterElements(primary_layer_);
  tooltip_box_.becomeOwner(this);
  stat_display_.hideDisplay();
  storage_.hide();
  return true;
}


void EvidyonGUIGameplayControls::setSplitInventoryCanvases() {
  {
    GUICanvas* canvas = equipped_.getCanvas();
    canvas->alignXCenter(0);
    canvas->alignYTop(20+35+20);
    canvas->absoluteHeight(220);
    canvas->absoluteWidth(400);
    canvas->absoluteDepth(0.2f);
  }
  {
    GUICanvas* canvas = unequipped_.getCanvas();
    canvas->alignXCenter(0);
    canvas->alignYBottom(-20);
    canvas->relativeHeight(-(40+35+20+240));
    canvas->absoluteWidth(400);
    canvas->absoluteDepth(0.2f);
  }
}


void EvidyonGUIGameplayControls::setSingleInventoryCanvas() {
  {
    GUICanvas* canvas = unequipped_.getCanvas();
    canvas->alignXCenter(0);
    canvas->alignYBottom(-20);
    canvas->relativeHeight(-(40+35+20));
    canvas->absoluteWidth(400);
    canvas->absoluteDepth(0.2f);
  }

}

void EvidyonGUIGameplayControls::setSpellCanvas() {
  GUICanvas* canvas = spells_.getCanvas();
  canvas->alignXCenter(0);
  canvas->alignYBottom(-20);
  canvas->relativeHeight(-(40+35+20));
  canvas->absoluteWidth(400);
  canvas->absoluteDepth(0.2f);
}

void EvidyonGUIGameplayControls::setStorageCanvases() {
  { // put the storage on the left
    GUICanvas* canvas = storage_.getCanvas();
    canvas->alignXCenter(-350/2-10);
    canvas->alignYBottom(-20);
    canvas->relativeHeight(-(40+35+20));
    canvas->absoluteWidth(350);
    canvas->absoluteDepth(0.2f);
  }
  { // put the inventory on the right
    GUICanvas* canvas = unequipped_.getCanvas();
    canvas->alignXCenter(+350/2+10);
    canvas->alignYBottom(-20);
    canvas->relativeHeight(-(40+35+20));
    canvas->absoluteWidth(350);
    canvas->absoluteDepth(0.2f);
  }
}

}

