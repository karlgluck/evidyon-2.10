//-----------------------------------------------------------------------------
// File:    useractionqueue.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "useractionqueue.h"


//-----------------------------------------------------------------------------
UserEventQueue::~UserEventQueue() {
  while (!action_queue_.empty()) {
    UserEvent* action = action_queue_.front();
    SAFE_DELETE(action);
    action_queue_.pop();
  }
}


//-----------------------------------------------------------------------------
void UserEventQueue::addTypeOnlyAction(UserEventType type) {
  UserEvent* action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = type;
  action_queue_.push(action);
}

//-----------------------------------------------------------------------------
UserEventAddAbilityPoint* UserEventQueue::addAttributesAddAbilityPoint() {
  UserEvent* action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = ATTRIBUTES_ADD_ABILITY_POINT;
  action_queue_.push(action);
  return &action->data_.add_ability_point_;
}


//-----------------------------------------------------------------------------
UserEventSpeak* UserEventQueue::addSpeak() {
  UserEvent* action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = SPEAK;
  action_queue_.push(action);
  return &action->data_.speak_;
}


UserEventBazaarBuyItem* UserEventQueue::addBazaarBuyItem() {
  UserEvent* action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = BAZAAR_BUY_ITEM;
  action_queue_.push(action);
  return &action->data_.bazaar_buy_item_;
}

UserEventBazaarListItem* UserEventQueue::addBazaarListItem() {
  UserEvent* action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = BAZAAR_LIST_ITEM;
  action_queue_.push(action);
  return &action->data_.bazaar_list_item_;
}

UserEventBazaarWithdrawItem* UserEventQueue::addBazaarWithdrawItem() {
  UserEvent* action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = BAZAAR_WITHDRAW_ITEM;
  action_queue_.push(action);
  return &action->data_.bazaar_withdraw_item_;
}

UserEventBazaarGetItemsForSale* UserEventQueue::addBazaarGetItemsForSale() {
  UserEvent* action = new UserEvent;
  ZeroMemory(action, sizeof(UserEvent));
  action->type_ = BAZAAR_GET_ITEMS_FOR_SALE;
  action_queue_.push(action);
  return &action->data_.bazaar_get_items_for_sale_;
}


//-----------------------------------------------------------------------------
bool UserEventQueue::popNextAction(UserEvent* action) {
  ASSERT(action) else return false;
  if (action_queue_.empty()) return false;
  UserEvent* queued_action = action_queue_.front();
  action_queue_.pop();
  memcpy(action, queued_action, sizeof(UserEvent));
  SAFE_DELETE(queued_action);
  return true;
}