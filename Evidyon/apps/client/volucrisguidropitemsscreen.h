//-----------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __VOLUCRISGUIDROPITEMSSCREEN_H__
#define __VOLUCRISGUIDROPITEMSSCREEN_H__


class UserEventQueue;

/**
 * Screen that shows up when the user wants to drop items
 */
class VolucrisGUIDropItemsScreen {

  class MouseSentinel : public GUIMouseSentinel {
  public:
    MouseSentinel();
    inline void enable(bool enabled) { enabled_ = enabled; }
    virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition,
                             GUIMouseInputSource* mouse);
    virtual bool onMouseButtonAction(GUIMouseButtonAction action,
                                     GUIPoint position,
                                     GUIMouseInputSource* mouse);
  private:
    bool enabled_;
  };

  /**
   * Adds a "drop item" request to the queue when an item is clicked
   * adds a 
   */
  class ItemList : public VolucrisGUIInventoryItemList {
  public:
    ItemList(VolucrisGUIDropItemsScreen* owner);
  protected:
    virtual void onMouseOverListEntryChanged();
    virtual void onSelectionChanged();
  private:
    VolucrisGUIDropItemsScreen* owner_;
  };

public:
  VolucrisGUIDropItemsScreen();
  bool create(const ClientItemDescription* item_descriptions,
              size_t num_item_descriptions,
              GUIFont* font,
              GUIMouseSentinel* parent_mouse_sentinel,
              UserEventQueue* user_action_queue,
              GUILayer* layer);
  void destroy();
  void show(GUILayer* layer);
  void update(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE], int update_indices[AVATAR_INVENTORY_SIZE]);
  inline void hide() { show(NULL); }

  void updateUserEvents();

private:
  GUILayer* layer_;
  MouseSentinel mouse_sentinel_;
  VolucrisGUITitledPanel item_panel_;
  ItemList item_list_;
  VolucrisGUIButton finished_button_;
  UserEventQueue* user_action_queue_;
  VolucrisGUIItemDescriptionPanel item_description_display_;
  const ClientItemDescription* item_description_array_;
  unsigned int num_item_descriptions_;
};

#endif