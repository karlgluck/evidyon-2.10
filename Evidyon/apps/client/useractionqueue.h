//-----------------------------------------------------------------------------
// File:  useractionqueue.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __USERACTIONQUEUE_H__
#define __USERACTIONQUEUE_H__


#include <queue>
#include "../shared/evidyon_avatar.h"


enum UserEventType {
  LOG_OUT_OF_WORLD,
  TOGGLE_VIEW_ANGLE,
  TOGGLE_SHOW_NAMES,
  SAY_AGAIN,
  STOP_ACTION, // unimplemented
  OPEN_INVENTORY,
  OPEN_ATTRIBUTES,
  OPEN_KEY_BINDINGS,
  OPEN_CHAT_AND_ACTIONS,
  OPEN_STAT_DISPLAY,
  OPEN_CHAT,
  OPEN_DROP,
  OPEN_BAZAAR,
  TOGGLE_CHAT_LOG,
  CLOSE_CHAT_AND_ACTIONS,
  CLOSE_INVENTORY,
  CLOSE_KEY_BINDINGS,
  CLOSE_STAT_DISPLAY,
  CLOSE_CHAT,
  CLOSE_DROP,
  CLOSE_BAZAAR,
  INVENTORY_DROP_ITEMS,
  INVENTORY_EQUIP_ITEMS,
  INVENTORY_UNEQUIP_ITEMS,
  ATTRIBUTES_ADD_ABILITY_POINT,
  SPEAK,
  DROP_ITEM,
  EQUIP_ITEM,
  UNEQUIP_ITEM,
  BAZAAR_BUY_ITEM,
  BAZAAR_LIST_ITEM,
  BAZAAR_WITHDRAW_ITEM,
  BAZAAR_GET_SELLER_ITEMS,
  BAZAAR_GET_ITEMS_FOR_SALE,
  BAZAAR_SELLER_REDEEM,
  TAKE_SCREENSHOT,
  OPEN_CAST,
  OPEN_USEITEM,
  EVENT_ESCAPE,
  STORAGE_WITHDRAW_ITEM,
  STORAGE_DEPOSIT_ITEM,
  GEOSID_SACRIFICE_ITEM,
  INVALID_ACTION,
};

struct UserEventSpeak {
  char text_[Evidyon::MAX_SPOKEN_TEXT_CHARACTERS+1];
};

struct UserEventAddAbilityPoint {
  int ability;
};

struct UserEventOnItem {
  int inventory_index_;
};

struct UserEventBazaarBuyItem {
  unsigned int listing_id_;
};

struct UserEventBazaarListItem {
  unsigned char inventory_index_;
  unsigned int price_;
};

struct UserEventBazaarWithdrawItem {
  unsigned int listing_id_;
};

struct UserEventBazaarGetItemsForSale {
  unsigned int minimum_listing_id_;
};


struct UserEvent {
  UserEventType type_;
  union {
  UserEventSpeak speak_;
  UserEventAddAbilityPoint add_ability_point_;
  UserEventOnItem item_event_;
  UserEventBazaarBuyItem bazaar_buy_item_;
  UserEventBazaarListItem bazaar_list_item_;
  UserEventBazaarWithdrawItem bazaar_withdraw_item_;
  UserEventBazaarGetItemsForSale bazaar_get_items_for_sale_;
  } data_;
};

class UserEventQueue {
public:
  ~UserEventQueue();

  void addLogOutOfWorld() { addTypeOnlyAction(LOG_OUT_OF_WORLD); }
  void addOpenChatAndActions() { addTypeOnlyAction(OPEN_CHAT_AND_ACTIONS); }
  void addOpenInventory() { addTypeOnlyAction(OPEN_INVENTORY); }
  void addOpenAttributes() { addTypeOnlyAction(OPEN_ATTRIBUTES); }
  void addOpenKeyBindings() { addTypeOnlyAction(OPEN_KEY_BINDINGS); }
  void addCloseChatAndActions() { addTypeOnlyAction(CLOSE_CHAT_AND_ACTIONS); }
  void addInventoryDropItems() { addTypeOnlyAction(INVENTORY_DROP_ITEMS); }
  void addInventoryEquipItems() { addTypeOnlyAction(INVENTORY_EQUIP_ITEMS); }
  void addInventoryUnequipItems() { addTypeOnlyAction(INVENTORY_UNEQUIP_ITEMS); }
  void addCloseInventory() { addTypeOnlyAction(CLOSE_INVENTORY); }
  void addCloseKeyBindings() { addTypeOnlyAction(CLOSE_KEY_BINDINGS); }
  void addCloseStatDisplay() { addTypeOnlyAction(CLOSE_STAT_DISPLAY); }
  void addToggleViewAngle() { addTypeOnlyAction(TOGGLE_VIEW_ANGLE); }
  void addToggleShowNames() { addTypeOnlyAction(TOGGLE_SHOW_NAMES); }
  void addOpenChat() { addTypeOnlyAction(OPEN_CHAT); }
  void addCloseChat() { addTypeOnlyAction(CLOSE_CHAT); }
  void addOpenDrop() { addTypeOnlyAction(OPEN_DROP); }
  void addCloseDrop() { addTypeOnlyAction(CLOSE_DROP); }
  UserEventAddAbilityPoint* addAttributesAddAbilityPoint();
  UserEventSpeak* addSpeak();
  UserEventBazaarBuyItem* addBazaarBuyItem();
  UserEventBazaarListItem* addBazaarListItem();
  UserEventBazaarWithdrawItem* addBazaarWithdrawItem();
  UserEventBazaarGetItemsForSale* addBazaarGetItemsForSale();
  bool popNextAction(UserEvent* action);

  public:
  void addTypeOnlyAction(UserEventType type);

private:
  typedef std::queue<UserEvent*> ActionQueue;
  ActionQueue action_queue_;
};



#endif
