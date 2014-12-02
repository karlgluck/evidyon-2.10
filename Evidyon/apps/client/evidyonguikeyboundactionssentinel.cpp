#include "evidyonguikeyboundactionssentinel.h"
#include "actiontargeter.h"
#include "../shared/client-editor/client_gamefile.h"
#include "../shared/client-editor/clientspelldescription.h"
#include "../shared/evidyon_avatarinventory.h"

#include "../shared/evidyon_version.h"

#include <dc/stream>

namespace Evidyon {


static const char KEY_BINDING_VERSION_TEXT[] = "2.8.0"; // change this to invalidate key bindings


EvidyonGUIKeyBoundActionsSentinel::EvidyonGUIKeyBoundActionsSentinel(ActionTargeter* targeter) {
  targeter_ = targeter;
  inventory_ = NULL;
  item_descriptions_ = NULL;
  spells_ = NULL;
}


void EvidyonGUIKeyBoundActionsSentinel::create(const ClientAvatarInventoryItem* inventory,
                                               const ClientItemDescription* item_descriptions,
                                               const ClientSpellDescription* spells) {
  inventory_ = inventory;
  item_descriptions_ = item_descriptions;
  spells_ = spells;
  setDefaultKeyBindings();
}



void EvidyonGUIKeyBoundActionsSentinel::setKey(GUIKey key, const Action* action) {
  if (action == NULL) {
    actions_[key].type = Action::TYPE_INVALID;
  } else {
    actions_[key] = *action;
  }
}


void EvidyonGUIKeyBoundActionsSentinel::setDefaultKeyBindings() {
  for (int i = 0; i < GUIKEY_COUNT; ++i) {
    actions_[i].type = Action::TYPE_INVALID;
  }

  actions_[GUIKEY_I].type = Action::TYPE_USEREVENT;
  actions_[GUIKEY_I].user_event = OPEN_INVENTORY;

  actions_[GUIKEY_U].type = Action::TYPE_USEREVENT;
  actions_[GUIKEY_U].user_event = OPEN_USEITEM;

  actions_[GUIKEY_C].type = Action::TYPE_USEREVENT;
  actions_[GUIKEY_C].user_event = OPEN_CAST;

  actions_[GUIKEY_D].type = Action::TYPE_USEREVENT;
  actions_[GUIKEY_D].user_event = OPEN_DROP;
  
  actions_[GUIKEY_S].type = Action::TYPE_USEREVENT;
  actions_[GUIKEY_S].user_event = OPEN_STAT_DISPLAY;

  actions_[GUIKEY_RETURN].type = Action::TYPE_USEREVENT;
  actions_[GUIKEY_RETURN].user_event = OPEN_CHAT;

  actions_[GUIKEY_F1].type = Action::TYPE_USEREVENT;
  actions_[GUIKEY_F1].user_event = TAKE_SCREENSHOT;

  actions_[GUIKEY_L].type = Action::TYPE_USEREVENT;
  actions_[GUIKEY_L].user_event = TOGGLE_CHAT_LOG;
}

void EvidyonGUIKeyBoundActionsSentinel::saveKeyBindings(dc::dcStreamOut* stream) {
  stream->writeString(std::string(KEY_BINDING_VERSION_TEXT));
  stream->write(actions_, sizeof(actions_));
}

bool EvidyonGUIKeyBoundActionsSentinel::readKeyBindings(dc::dcStreamIn* stream) {
  std::string str;
  if (!stream->readString(&str) || 0 != str.compare(KEY_BINDING_VERSION_TEXT)) {
    setDefaultKeyBindings();
    return false;
  } else {
    stream->read(actions_, sizeof(actions_));
    return true;
  }
}

GUIKey EvidyonGUIKeyBoundActionsSentinel::getKeyForSpell(int spell_type) {
  for (int i = 0; i < GUIKEY_COUNT; ++i) {
    if (actions_[i].type == Action::TYPE_SPELL &&
        actions_[i].spell.spell_type == spell_type) return (GUIKey)i;
  }
  return GUIKEY_NULL;
}

GUIKey EvidyonGUIKeyBoundActionsSentinel::getKeyForUserEvent(UserEventType event_type) {
  for (int i = 0; i < GUIKEY_COUNT; ++i) {
    if (actions_[i].type == Action::TYPE_USEREVENT &&
        actions_[i].user_event == event_type) return (GUIKey)i;
  }
  return GUIKEY_NULL;
}


GUIKey EvidyonGUIKeyBoundActionsSentinel::getKeyForConsumable(int item_type) {
  for (int i = 0; i < GUIKEY_COUNT; ++i) {
    if (actions_[i].type == Action::TYPE_CONSUMABLE &&
        actions_[i].consumable.item_type == item_type) return (GUIKey)i;
  }
  return GUIKEY_NULL;
}


GUIKey EvidyonGUIKeyBoundActionsSentinel::getKeyForAction(const Action* action) {
  Action::Type type = action->type;
  for (int i = 0; i < GUIKEY_COUNT; ++i) {
    if (actions_[i].type != type) continue;
    switch (type) {
      case Action::TYPE_SPELL: if (actions_[i].spell.spell_type == action->spell.spell_type) return (GUIKey)i; break;
      case Action::TYPE_CONSUMABLE: if (actions_[i].consumable.item_type == action->consumable.item_type) return (GUIKey)i; break;
      case Action::TYPE_USEREVENT: if (actions_[i].user_event == action->user_event) return (GUIKey)i; break;
    }
  }
  return GUIKEY_NULL;
}

bool EvidyonGUIKeyBoundActionsSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) { // prevent propagation
    return true;
  }

  switch (actions_[key].type) {
    case Action::TYPE_USEREVENT: targeter_->userEvent(actions_[key].user_event); break;
    case Action::TYPE_SPELL: {
      int type = actions_[key].spell.spell_type;
      targeter_->castSpell(type, spells_[type].needs_target);
    } break;
    case Action::TYPE_CONSUMABLE: {
      int type = actions_[key].consumable.item_type;
      const ClientItemDescription* description = &item_descriptions_[type];
      if (false == description->consumable) break;
      for (int i = 0; i < Evidyon::AVATAR_INVENTORY_SIZE; ++i) {
        if (inventory_[i].type == type) {
          targeter_->consumeItem(i, description->consuming_requires_target);
          break;
        }
      }
    } break;

    default: return false;
  }

  // capture this event
  return true;
}

bool EvidyonGUIKeyBoundActionsSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) {
    targeter_->userEvent(EVENT_ESCAPE);
    return true;
  }

  return false;
}



}