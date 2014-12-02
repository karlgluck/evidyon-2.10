//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIUSERACTIONKEYBINDINGLIST_H__
#define __VOLUCRISGUIUSERACTIONKEYBINDINGLIST_H__


namespace UserEvents
{

static const UserEventType KEY_BOUND_ACTION_LIST[] = {
  SAY_AGAIN,
  OPEN_INVENTORY,
  OPEN_KEY_BINDINGS,
  OPEN_CHAT,
  OPEN_DROP,
  OPEN_STAT_DISPLAY,
  TOGGLE_CHAT_LOG,
  TOGGLE_VIEW_ANGLE,
  TAKE_SCREENSHOT,
  //TOGGLE_SHOW_NAMES,
  //OPEN_BAZAAR,
};

static const GUIKey DEFAULT_ACTION_KEYS[] = {
  GUIKEY_A,
  GUIKEY_I,
  GUIKEY_K,
  GUIKEY_RETURN,
  GUIKEY_D,
  GUIKEY_S,
  GUIKEY_L,
  GUIKEY_V,
  GUIKEY_F1,
  //GUIKEY_TAB,
  //GUIKEY_B,
};

static const int NUMBER_OF_BOUND_ACTION_KEYS = sizeof(KEY_BOUND_ACTION_LIST)/sizeof(UserEventType);

// increment this value when KEY_BOUND_ACTION_LIST changes so that bound keys get rewritten
static const int USER_ACTION_VERSION = 4;

}


//-----------------------------------------------------------------------------
class VolucrisGUIUserEventKeyBindingList : public VolucrisGUISelectableTextLineList {

  //---------------------------------------------------------------------------
  class ActionKeyBindingListEntry : public VolucrisListEntry {
  public:
    ActionKeyBindingListEntry();
    void initializeListEntry(VolucrisGUIUserEventKeyBindingList* owner,
                             GUIKey current_key, UserEventType action_type);

    inline UserEventType getActionType() const { return action_type_; }
    inline GUIKey getBoundKey() const { return bound_key_; }
    void setBoundKey(GUIKey key);

  protected:
    virtual void onStateChange(StateChange state_change);

  private:
    VolucrisGUIUserEventKeyBindingList* owner_;
    GUIKey bound_key_;
    UserEventType action_type_;
  };

  //---------------------------------------------------------------------------
  class KeyboardSentinel : public GUIKeyboardSentinel {
  public:
    KeyboardSentinel(VolucrisGUIUserEventKeyBindingList* owner);
    void enable(bool enabled) { enabled_ = enabled; }
    virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);
    void watchForBinding(ActionKeyBindingListEntry* entry);

  private:
    bool enabled_;
    VolucrisGUIUserEventKeyBindingList* owner_;
    ActionKeyBindingListEntry* next_entry_to_bind_;
  };


  //---------------------------------------------------------------------------
  class MouseSentinel : public GUIMouseSentinel {
  public:
    MouseSentinel(VolucrisGUIUserEventKeyBindingList* owner);
    inline void enable(bool enabled) { enabled_ = enabled; }
    virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
    virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);
  private:
    bool enabled_;
    VolucrisGUIUserEventKeyBindingList* owner_;
  };

public:
  static const char* userActionTypeString(UserEventType action_type);

public:
  VolucrisGUIUserEventKeyBindingList();
  bool create(GUIFont* font,
              GUIMouseSentinel* parent_mouse_sentinel,
              GUIKeyboardSentinel* parent_keyboard_sentinel);
  void destroy();
  void show(GUILayer* layer);
  inline void hide() { show(NULL); }

  // constructs a list of keys that map 1:1 to entries in BOUND_ACTION_KEYS.
  // a value of GUIKEY_NULL means no key was bound.
  void setKeyToActionBindings(GUIKey keys[UserEvents::NUMBER_OF_BOUND_ACTION_KEYS]);
  void buildKeyToActionBindings(GUIKey keys[UserEvents::NUMBER_OF_BOUND_ACTION_KEYS]);

protected:
  void entryWantsKeyBinding(ActionKeyBindingListEntry* entry);

protected:
  virtual size_t getNumberOfLines() const;
  virtual ListEntry* getListEntry(size_t index);

private:
  GUILayer* layer_;
  KeyboardSentinel keyboard_sentinel_;
  MouseSentinel mouse_sentinel_;
  ActionKeyBindingListEntry entries_[UserEvents::NUMBER_OF_BOUND_ACTION_KEYS];
};


#endif