//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"





//------------------------------------------------------------------------------------------------
ChatTextLine::KeyboardSentinel::KeyboardSentinel(ChatTextLine* owner) {
  owner_ = owner;
  watching_ = false;
  enabled_ = false;
}



//------------------------------------------------------------------------------------------------
bool ChatTextLine::KeyboardSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!enabled_) return false;

  if (key == GUIKEY_RETURN) {
  watching_ = true;
  return true;
  }

  // Trap all messages
  return true;
}



//------------------------------------------------------------------------------------------------
bool ChatTextLine::KeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!enabled_) return false;
  switch (key) {
  case GUIKEY_RETURN:
    if (!watching_) break;
    watching_ = false;
    // changed 3-5-09; exit_ is determined if the entire message was sent
    owner_->exit_ = false;//!(keyboard->isKeyDown(GUIKEY_LSHIFT) || keyboard->isKeyDown(GUIKEY_RSHIFT));
    owner_->send_ = true;
    break;  // Don't propogate this message
  case GUIKEY_ESCAPE:
    watching_ = false;
    owner_->exit_ = true;
    owner_->send_ = false;
    break;
  }

  // Trap all messages
  return true;
}



//------------------------------------------------------------------------------------------------
bool ChatTextLine::KeyboardSentinel::onTypedCharacter(
  char character,
  GUIKeyboardInputSource* keyboard) {
  if (enabled_) {
    return GUIKeyboardSentinel::onTypedCharacter(character, keyboard);
  } else {
    return false;
  }
}



//------------------------------------------------------------------------------------------------
ChatTextLine::MouseSentinel::MouseSentinel(ChatTextLine* owner) {
  owner_ = owner;
  enabled_ = false;
}



//------------------------------------------------------------------------------------------------
bool ChatTextLine::MouseSentinel::onMouseButtonAction(GUIMouseButtonAction action,
                            GUIPoint position,
                            GUIMouseInputSource* mouse) {
  if (!enabled_) return false;

  if (GUIMouseSentinel::onMouseButtonAction(action, position, mouse)) return true;
  if (action == GUIMOUSE_CLICK_SINGLE) {
  owner_->exit_ = true;
  owner_->send_ = false;
  return true;
  }
  return false;
}



//------------------------------------------------------------------------------------------------
ChatTextLine::ChatTextLine() : keyboard_sentinel_(this), mouse_sentinel_(this) {
  exit_ = false;
  send_ = false;
  layer_ = NULL;
}



//------------------------------------------------------------------------------------------------
bool ChatTextLine::create(GUIFont* font,
                          GUIMouseSentinel* root_mouse_sentinel,
                          GUIKeyboardSentinel* root_keyboard_sentinel,
                          GUISize hpMpXpSize) {
  destroy();
  setFont(font);
  setTextColor(VolucrisGUIDefs::TEXT_INPUT_COLOR);
  setBackgroundColor(D3DCOLOR_ARGB(128,255,255,255)); // make the background translucent
  keyboard_sentinel_.setParentKeyboardSentinel(root_keyboard_sentinel);
  getKeyboardSentinel()->setParentKeyboardSentinel(&keyboard_sentinel_);
  mouse_sentinel_.setParentMouseSentinel(root_mouse_sentinel);
  getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);

  GUICanvas* canvas = getCanvas();
  canvas->alignXLeft(hpMpXpSize.width + 5);
  canvas->alignYBottom(-32-6);
  canvas->relativeWidth(-(hpMpXpSize.width + 10));
  canvas->absoluteHeight(35);
  canvas->absoluteDepth(0.75f);

  return true;
}



//------------------------------------------------------------------------------------------------
void ChatTextLine::destroy() {
  setFont(NULL);
  getCanvas()->setSourceCanvas(NULL);
  keyboard_sentinel_.removeFromKeyboardSentinelChain();
  mouse_sentinel_.removeFromMouseSentinelChain();
}



//------------------------------------------------------------------------------------------------
void ChatTextLine::show(GUILayer* layer) {
  bool enabled = layer != NULL;
  keyboard_sentinel_.enable(enabled);
  mouse_sentinel_.enable(enabled);

  if (layer_) {
    getCanvas()->setSourceCanvas(NULL);
    unregisterElements(layer_);
  }

  if (layer) {
    getCanvas()->setSourceCanvas(layer->getPrimaryCanvas());
    registerElements(layer);
    getKeyboardSentinel()->obtainKeyboardFocus();
    getMouseSentinel()->obtainMouseFocus();
  }

  layer_ = layer;
}


bool ChatTextLine::update(UserEvent* user_event) {
  if (send_) {
    send_ = false;
    user_event->type_ = SPEAK;
    UserEventSpeak* speak = &user_event->data_.speak_;
    std::string text = getText();
    std::string text_to_send;
    if (text.length() > MAX_SPOKEN_TEXT_CHARACTERS) { // Do we have to cut this message short?
      text_to_send.assign(text.substr(0, MAX_SPOKEN_TEXT_CHARACTERS));
      std::string::size_type last_space = text_to_send.find_last_of(" ");

      // erase after the last space
      if (last_space != text_to_send.npos) text_to_send.erase(last_space);
      text.erase(0,text_to_send.length()+1);
      myCaretLocation = text.length();
    } else {
      text_to_send.assign(text);
      text.erase();
    }

    strncpy_s(
      speak->text_,
      MAX_SPOKEN_TEXT_CHARACTERS+1,
      text_to_send.c_str(),
      MAX_SPOKEN_TEXT_CHARACTERS);

    // set what is left of the text back in the buffer
    if (text.empty()) {
      exit_ = true;
    } else { // add a dash if the user was speaking globally
      if (text_to_send.empty() == false &&
          text_to_send.at(0) == '-') {
         text.insert(0, "-");
      }
    }
    setText(text.c_str());
    return true;
  }
  if (exit_) {
    exit_ = false;
    user_event->type_ = CLOSE_CHAT;
    setText("");
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------------------------
void ChatTextLine::updateActionQueue(UserEventQueue* queue) {
  if (send_) {
    send_ = false;
    UserEventSpeak* speak = queue->addSpeak();
    std::string text = getText();
    std::string text_to_send;
    if (text.length() > MAX_SPOKEN_TEXT_CHARACTERS) { // Do we have to cut this message short?
      text_to_send.assign(text.substr(0, MAX_SPOKEN_TEXT_CHARACTERS));
      std::string::size_type last_space = text_to_send.find_last_of(" ");

      // erase after the last space
      if (last_space != text_to_send.npos) text_to_send.erase(last_space);
      text.erase(0,text_to_send.length()+1);
      myCaretLocation = text.length();
    } else {
      text_to_send.assign(text);
      text.erase();
    }

    strncpy_s(
      speak->text_,
      MAX_SPOKEN_TEXT_CHARACTERS+1,
      text_to_send.c_str(),
      MAX_SPOKEN_TEXT_CHARACTERS);

    // set what is left of the text back in the buffer
    setText(text.c_str());
  }
  if (exit_) {
    exit_ = false;
    queue->addCloseChat();
    setText("");
  }
}