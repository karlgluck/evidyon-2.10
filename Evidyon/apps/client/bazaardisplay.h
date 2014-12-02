//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __BAZAARDISPLAY_H__
#define __BAZAARDISPLAY_H__





enum BazaarDisplayMode {
  BAZAAR_MODE_BROWSE,
  BAZAAR_MODE_VIEWSALES,
  BAZAAR_MODE_LISTFORSALE,
  BAZAAR_MODE_LISTFORSALE_INPUTITEMPRICE,
};

class BazaarDisplay {

  class KeyboardSentinel : public GUIKeyboardSentinel {
  public:
  KeyboardSentinel(BazaarDisplay* owner);
  void enable(bool enabled);
  bool isEnabled() { return enabled_; }
  virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);

  private:
  BazaarDisplay* owner_;
  bool enabled_;
  };

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

  // Shows this client's inventory
  class InventoryItemList : public VolucrisGUIInventoryItemList {
  public:
  InventoryItemList(BazaarDisplay* owner);
  protected:
  virtual void onMouseOverListEntryChanged();
  virtual void onSelectionChanged();
  private:
  BazaarDisplay* owner_;
  };

  class BazaarItemList : public VolucrisGUIItemList {
  class BazaarItemListEntry : public VolucrisGUIItemList::ItemListEntry {
  public:
    BazaarItemListEntry() : price_text_(this) { owner_ = NULL; }
    void initialize(BazaarItemList* owner, GUIFont* font);
    void copyDisplay(BazaarItemListEntry* other);
    void displayBazaarItem(const ItemForTrade* item, bool differentiate_for_owner); // differentiate... makes this item look different
    inline unsigned int getListingID() const { return listing_id_; }
    inline unsigned int getPrice() const { return price_value_; }
  public:
    virtual void registerElements(GUILayer* layer);
    virtual void unregisterElements(GUILayer* layer);
    virtual void refresh();
    virtual void onMouseAction(MouseAction action);
  private:
    BazaarItemList* owner_;
    bool owned_item_;
    unsigned int listing_id_;
    unsigned int price_value_;
    VolucrisGUIListEntryText price_text_;
  };
  public:
    BazaarItemList(BazaarDisplay* owner);

    void create(GUIFont* font);
    void destroy();
    void show(GUILayer* layer);
    void hide(GUILayer* layer);

    // If layer != null, this item list unregisters the current elements first (to get rid of old items)
    // then re-registers the new ones with this layer.  This method calls refresh().
    void updateItemList(GUILayer* layer, bool reregister_elements,
              bool display_seller_status, ItemForTrade* items, int number_of_items);
    void clear(GUILayer* layer, bool reregister_elements);
    void removeListing(GUILayer* layer, bool reregister_elements, unsigned int listing_id);
    inline GUICanvas* getCanvas() { return &list_canvas_; }
    inline GUIMouseSentinel* getMouseSentinel() { return &mouse_sentinel_; }
    void syncSlider() { if (getNumberOfLines() > 0) slider_.syncToList(); }
  protected:
    void updateItemList(bool display_seller_status, ItemForTrade* items, int number_of_items);
    virtual void onChangeMouseOverEntry(ItemListEntry* entry);
    virtual GUIList::ListEntry* getListEntry(unsigned int index) { return index < Economy::MAX_ITEMS_PER_BAZAAR_PAGE ? &entries_[index] : NULL; }
    virtual unsigned int getNumberOfLines() const { return number_of_entries_; }
  private:
    GUIMouseSentinel mouse_sentinel_;
    GUICanvas list_canvas_;
    GUICanvas background_canvas_;
    ListSlider slider_;
    BazaarDisplay* owner_;
    BazaarItemListEntry entries_[Economy::MAX_ITEMS_PER_BAZAAR_PAGE];
    unsigned int number_of_entries_;
    GUIBorderedFillElement background_;
  };

  public:
  BazaarDisplay();
  ~BazaarDisplay();
  bool create(const ClientItemDescription* item_descriptions,
        size_t num_item_descriptions,
        GUIFont* font,
        GUIMouseSentinel* parent_mouse_sentinel,
        GUIKeyboardSentinel* parent_keyboard_sentinel,
        UserEventQueue* user_action_queue,
        GUILayer* layer);
  void destroy();
  void show(GUILayer* layer);
  void updateInventory(ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE], int update_indices[AVATAR_INVENTORY_SIZE]);
  void updateBazaar(ItemForTrade* items_to_display, int number_of_items);
  void updateSellerStatus(ItemForTrade* items_being_sold, int number_of_items);
  void updateMoney(unsigned int gold);
  void removeListing(unsigned int listing_id);
  inline void hide() { listings_.clear(); show(NULL); changeMode(BAZAAR_MODE_BROWSE); }

  void updateUserEvents();

protected:
  void changeMode(BazaarDisplayMode mode);
  void listForSale(unsigned char inventory_index);
  void buy(unsigned int price, unsigned int listing_id);
  void withdraw(unsigned int listing_id);
  void redeem();

private:
  GUILayer* layer_;
  MouseSentinel mouse_sentinel_;
  KeyboardSentinel keyboard_sentinel_;
  InventoryItemList inventory_items_;
  VolucrisGUITitledPanel bazaar_items_panel_;
  BazaarItemList bazaar_items_; // displays shopping status

  UserEventQueue* user_action_queue_;

  unsigned int available_money_;
  GUICanvas money_text_canvas_;
  GUITextLabelElement money_text_;

  GUICanvas item_description_background_canvas_;
  GUIBorderedFillElement item_description_background_;
  GUICanvas item_description_canvas_;
  GUITextLabelElement item_description_;

  GUICanvas listing_prompt_canvas_;
  GUITextLabelElement listing_prompt_;
  VolucrisGUIEditableTextLine listing_price_;
  VolucrisGUIButton listing_confirm_;
  VolucrisGUIButton listing_cancel_;
  unsigned char listing_selected_item_inventory_index_;

  VolucrisGUIButton page_forward_;
  VolucrisGUIButton page_backward_;
  VolucrisGUIButton page_refresh_;
  //typedef std::map<unsigned int,int> PaginationList;
  //PaginationList pagination_list_;
  typedef std::set<unsigned int> Listings;
  Listings listings_;
  int current_page_index_;
/*
  unsigned int getPageListingID() {
  PaginationList::iterator i = pagination_list_.begin();
  for (int p=0; p < current_page_index_ && i != pagination_list_.end(); ++p, ++i);
  if (i != pagination_list_.end()) return i->first;
  return 0;
  }*/
  unsigned int getPageListingID() {
  int page = 0, index = 0;
  unsigned int current_page_last_listing = 0;
  for (Listings::iterator i = listings_.begin(); i != listings_.end(); ++i, ++index) {
    current_page_last_listing = *i;
    if (page == current_page_index_) return *i;
    if (index == Economy::MAX_ITEMS_PER_BAZAAR_PAGE) {
    page++;
    }
  }
  return current_page_last_listing;
  }

  VolucrisGUIButton buy_sell_toggle_button_; // changes text depending on current mode
  VolucrisGUIButton finished_button_;
  VolucrisGUIButton list_for_sale_; // shows up in seller mode to allow user to select items to sell

  BazaarDisplayMode display_mode_;

  const ClientItemDescription* item_description_array_;
  unsigned int num_item_descriptions_;
};



#endif