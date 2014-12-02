//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __CHATTEXTLINE_H__
#define __CHATTEXTLINE_H__

#include "volucrisguieditabletextline.h"

class ChatTextLine : public VolucrisGUIEditableTextLine {
  class KeyboardSentinel : public GUIKeyboardSentinel {
  public:
  KeyboardSentinel(ChatTextLine* owner);
  void enable(bool enabled) { enabled_ = enabled; }
  virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onTypedCharacter(char character, GUIKeyboardInputSource* keyboard);

  private:
  bool enabled_;
  bool watching_;
  ChatTextLine* owner_;
  };

  class MouseSentinel : public GUIMouseSentinel {
  public:
  MouseSentinel(ChatTextLine* owner);
  void enable(bool enabled) { enabled_ = enabled; }
  virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);
  private:
  bool enabled_;
  ChatTextLine* owner_;
  };

public:
  ChatTextLine();
  bool create(GUIFont* font,
        GUIMouseSentinel* root_mouse_sentinel,
        GUIKeyboardSentinel* root_keyboard_sentinel,
        GUISize hpMpXpSize);
  void destroy();
  void show(GUILayer* layer);
  void updateActionQueue(UserEventQueue* queue);

  bool update(UserEvent* user_event);

protected:
  KeyboardSentinel keyboard_sentinel_;
  MouseSentinel mouse_sentinel_;
  bool exit_;
  bool send_;
  GUILayer* layer_;
};




#endif
