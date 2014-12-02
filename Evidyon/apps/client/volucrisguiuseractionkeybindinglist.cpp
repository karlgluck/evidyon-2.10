//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include <dc/debug>

//------------------------------------------------------------------------------------------------
VolucrisGUIUserEventKeyBindingList::ActionKeyBindingListEntry::ActionKeyBindingListEntry() {
  owner_ = NULL;
  bound_key_ = GUIKEY_NULL;
  action_type_ = INVALID_ACTION;
}

//------------------------------------------------------------------------------------------------
void VolucrisGUIUserEventKeyBindingList::ActionKeyBindingListEntry::initializeListEntry(
    VolucrisGUIUserEventKeyBindingList* owner,
    GUIKey current_key,
    UserEventType action_type) {
  owner_ = owner;
  action_type_ = action_type;
  setBoundKey(current_key);
}

//------------------------------------------------------------------------------------------------
void VolucrisGUIUserEventKeyBindingList::ActionKeyBindingListEntry::setBoundKey(
    GUIKey key) {
  bound_key_ = key;

  // taken from volucrisguieffectkeybindinglist.cpp
  const char* keyText = VolucrisGUIEffectKeyBindingList::getKeyText(key);
  std::string label = keyText;
  if (label.length() < 8)
    label = std::string("        ").substr(0,8-label.length()).append(label);
  label.append(" : ");
  label.append(VolucrisGUIUserEventKeyBindingList::userActionTypeString(action_type_));

  // Set the name of this line based on the key mapping and effect
  setText(label.c_str());
}

//------------------------------------------------------------------------------------------------
void VolucrisGUIUserEventKeyBindingList::ActionKeyBindingListEntry::onStateChange(StateChange state_change) {
  if (!owner_) return;
  if (state_change == STATECHANGE_SELECTED) owner_->entryWantsKeyBinding(this);
}



//------------------------------------------------------------------------------------------------
VolucrisGUIUserEventKeyBindingList::KeyboardSentinel::KeyboardSentinel(VolucrisGUIUserEventKeyBindingList* owner) {
  owner_ = owner;
  enabled_ = false;
  next_entry_to_bind_ = NULL;
}

//------------------------------------------------------------------------------------------------
bool VolucrisGUIUserEventKeyBindingList::KeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!enabled_ || !next_entry_to_bind_) return false;
  if (key == GUIKEY_DELETE && next_entry_to_bind_->getBoundKey() != GUIKEY_NULL)
    next_entry_to_bind_->setBoundKey(GUIKEY_NULL);
  else
    next_entry_to_bind_->setBoundKey(key);
  next_entry_to_bind_->deselect();
  next_entry_to_bind_ = NULL;
  return true; // don't propogate
}

//------------------------------------------------------------------------------------------------
void VolucrisGUIUserEventKeyBindingList::KeyboardSentinel::watchForBinding(ActionKeyBindingListEntry* entry) {
  if (next_entry_to_bind_) next_entry_to_bind_->deselect();
  next_entry_to_bind_ = entry;
}

VolucrisGUIUserEventKeyBindingList::MouseSentinel::MouseSentinel(VolucrisGUIUserEventKeyBindingList* owner) {
  owner_ = owner;
  enabled_ = false;
}

bool VolucrisGUIUserEventKeyBindingList::MouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  if (!enabled_) return false;
  return GUIMouseSentinel::onMouseMove(position, oldPosition, mouse);
}

bool VolucrisGUIUserEventKeyBindingList::MouseSentinel::onMouseButtonAction(
    GUIMouseButtonAction action,
    GUIPoint position,
    GUIMouseInputSource* mouse) {
  if (!enabled_) return false;
  return GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
}

const char* VolucrisGUIUserEventKeyBindingList::userActionTypeString(UserEventType action_type) {
  switch (action_type) {
  case LOG_OUT_OF_WORLD:          return "Log out of world";
  case TOGGLE_VIEW_ANGLE:         return "Toggle view mode";
  case TOGGLE_SHOW_NAMES:         return "Toggle show names";
  case SAY_AGAIN:                 return "Say Again";
  case STOP_ACTION:               return "Stop Action";
  case OPEN_INVENTORY:            return "Inventory";
  case OPEN_KEY_BINDINGS:         return "Key Bindings";
  case OPEN_CHAT_AND_ACTIONS:     return "Chat and Actions";
  case OPEN_STAT_DISPLAY:         return "Avatar Attributes";
  case OPEN_CHAT:                 return "Chat";
  case TOGGLE_CHAT_LOG:           return "Chat Log";
  case OPEN_DROP:                 return "Drop";
  case OPEN_BAZAAR:               return "Open bazaar menu (when in range)";
  case TAKE_SCREENSHOT:           return "Take Screenshot";
  case CLOSE_CHAT_AND_ACTIONS:    
  case CLOSE_INVENTORY:        
  case CLOSE_KEY_BINDINGS:        
  case CLOSE_STAT_DISPLAY:        
  case CLOSE_CHAT:        
  case INVENTORY_DROP_ITEMS:        
  case INVENTORY_EQUIP_ITEMS:        
  case INVENTORY_UNEQUIP_ITEMS:        
  case ATTRIBUTES_ADD_ABILITY_POINT:        
  case SPEAK:                   return "!!!";
  default:                      return "???";
  }
}

VolucrisGUIUserEventKeyBindingList::VolucrisGUIUserEventKeyBindingList()
    : keyboard_sentinel_(this), mouse_sentinel_(this) {
  layer_ = NULL;

  getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);

  // Initialize list entries
  for (int i = 0; i < UserEvents::NUMBER_OF_BOUND_ACTION_KEYS; ++i) {
    entries_[i].initializeListEntry(this, GUIKEY_NULL, UserEvents::KEY_BOUND_ACTION_LIST[i]);
  }

  setLineHeight(30);
}

bool VolucrisGUIUserEventKeyBindingList::create(GUIFont* font,
                                                 GUIMouseSentinel* parent_mouse_sentinel,
                                                 GUIKeyboardSentinel* parent_keyboard_sentinel) {
  destroy();

  keyboard_sentinel_.setParentKeyboardSentinel(parent_keyboard_sentinel);
  mouse_sentinel_.setParentMouseSentinel(parent_mouse_sentinel);

  for (int i = 0; i < UserEvents::NUMBER_OF_BOUND_ACTION_KEYS; ++i) {
    entries_[i].setFont(font);
  }

  return true;
}

void VolucrisGUIUserEventKeyBindingList::destroy() {
  hide();
  keyboard_sentinel_.removeFromKeyboardSentinelChain();
  mouse_sentinel_.removeFromMouseSentinelChain();
  for (int i = 0; i < UserEvents::NUMBER_OF_BOUND_ACTION_KEYS; ++i) {
    entries_[i].setFont(NULL);
  }
}

void VolucrisGUIUserEventKeyBindingList::show(GUILayer* layer) {
  bool enabled = layer != NULL;

  keyboard_sentinel_.enable(enabled);
  mouse_sentinel_.enable(enabled);

  if (layer_) {
    unregisterElements(layer_);
    for (int i = 0; i < UserEvents::NUMBER_OF_BOUND_ACTION_KEYS; ++i) {
      entries_[i].unregisterElements(layer_);
    }
  }

  if (layer) {
    registerElements(layer);
    for (int i = 0; i < UserEvents::NUMBER_OF_BOUND_ACTION_KEYS; ++i) {
      entries_[i].registerElements(layer);
    }
  }

  layer_ = layer;
}

void VolucrisGUIUserEventKeyBindingList::setKeyToActionBindings(GUIKey keys[UserEvents::NUMBER_OF_BOUND_ACTION_KEYS]) {
  CONFIRM(keys != NULL) else return;
  for (int i = 0; i < UserEvents::NUMBER_OF_BOUND_ACTION_KEYS; ++i) {
    entries_[i].setBoundKey(keys[i]);
  }
}

// constructs a list of keys that map 1:1 to entries in BOUND_ACTION_KEYS.
// a value of GUIKEY_NULL means no key was bound.
void VolucrisGUIUserEventKeyBindingList::buildKeyToActionBindings(GUIKey keys[UserEvents::NUMBER_OF_BOUND_ACTION_KEYS]) {
  CONFIRM(keys != NULL) else return;
  for (int i = 0; i < UserEvents::NUMBER_OF_BOUND_ACTION_KEYS; ++i) {
    keys[i] = entries_[i].getBoundKey();
  }
}

void VolucrisGUIUserEventKeyBindingList::entryWantsKeyBinding(ActionKeyBindingListEntry* entry) {
  keyboard_sentinel_.watchForBinding(entry);
}


size_t VolucrisGUIUserEventKeyBindingList::getNumberOfLines() const {
  return UserEvents::NUMBER_OF_BOUND_ACTION_KEYS;
}

GUIList::ListEntry* VolucrisGUIUserEventKeyBindingList::getListEntry(size_t index) {
  return index < UserEvents::NUMBER_OF_BOUND_ACTION_KEYS ? static_cast<ListEntry*>(&entries_[index]) : NULL;
}