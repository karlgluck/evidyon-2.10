#include "evidyonguimodule_inventorylist.h"
#include "evidyonguitooltipbox.h"
#include "evidyonguikeyboundactionssentinel.h"


namespace Evidyon {


EvidyonGUIModule_InventoryList::EvidyonGUIModule_InventoryList() {
  tooltip_ = NULL;
  button_font_ = NULL;
  inventory_ = NULL;
  item_descriptions_ = NULL;
  number_of_item_descriptions_ = 0;
}



void EvidyonGUIModule_InventoryList::create(GUICanvas* source_canvas,
                                            GUIFont* button_font,
                                            const ClientItemDescription* item_descriptions,
                                            size_t number_of_item_descriptions,
                                            const ClientAvatarInventoryItem* inventory,
                                            EvidyonGUITooltipBox* tooltip) {
  inventory_ = inventory;
  button_font_ = button_font;
  item_descriptions_ = item_descriptions;
  number_of_item_descriptions_ = number_of_item_descriptions;
  addComponent(&item_list_);

  item_list_.setLineHeight(30);

  GUICanvas* canvas = item_list_.getCanvas();
  canvas->setSourceCanvas(source_canvas);
  canvas->absoluteWidth(400);
  canvas->absoluteHeight(300);
  canvas->alignXLeft(50);
  canvas->alignYTop(100);
  canvas->absoluteDepth(0.4f);

  tooltip_ = tooltip;

  tooltip_description_.setFont(button_font);
}



void EvidyonGUIModule_InventoryList::destroy() {
  clearComponents();
  button_font_ = NULL;
  hide();
}

GUICanvas* EvidyonGUIModule_InventoryList::getCanvas() {
  return item_list_.getCanvas();
}



void EvidyonGUIModule_InventoryList::sync(DisplayType display_type,
                                          EvidyonGUIKeyBoundActionsSentinel* key_bindings) {
  GUILayer* layer = hide();
  item_list_.clear();
  switch (display_type) {
  case DISPLAY_UNEQUIPPED:
  case DISPLAY_EQUIPPED: {
      for (int i = 0; i < Evidyon::AVATAR_INVENTORY_SIZE; ++i) {
        size_t type = inventory_[i].type;
        int quantity = inventory_[i].quantity;
        bool correct_state = (inventory_[i].equipped != 0) ? (display_type == DISPLAY_EQUIPPED) : (display_type == DISPLAY_UNEQUIPPED);
        if (inventory_[i].inventory_index != i ||
            false == correct_state ||
            type >= number_of_item_descriptions_ ||
            quantity <= 0) continue;
        VolucrisGUIButton* button = item_list_.button(i);
        std::string text;
        if (quantity > 1 || item_descriptions_[type].consumable) {
          char num[64];
          sprintf_s(num, 64, "%i ",quantity );
          text = num;
        }
        text.append(item_descriptions_[type].name);
        button->setText(text.c_str());
        button->getTextLabelElement()->setAlignment(GUIALIGN_LEFT);
        button->setFont(button_font_);
        button->enable();
      }
    } break;
  case DISPLAY_CONSUMABLES: {
      for (int i = 0; i < Evidyon::AVATAR_INVENTORY_SIZE; ++i) {
        size_t type = inventory_[i].type;
        int quantity = inventory_[i].quantity;
        if (inventory_[i].inventory_index != i ||
            type >= number_of_item_descriptions_ ||
            item_descriptions_[type].consumable == false ||
            quantity <= 0) continue;
        VolucrisGUIButton* button = item_list_.button(i);
        std::string text;
        char num[64];
        sprintf_s(num, 64, "%i ", quantity);
        text = num;
        text.append(item_descriptions_[type].name);
        if (key_bindings) {
          GUIKey bound_key = key_bindings->getKeyForConsumable(type);
          if (bound_key != GUIKEY_NULL) {
            text.append(" (");
            text.append(GUIKeyboardKeyName(bound_key));
            text.append(")");
          }
        }
        button->setText(text.c_str());
        button->getTextLabelElement()->setAlignment(GUIALIGN_LEFT);
        button->setFont(button_font_);
        button->enable();
      }
    } break;
  }
  show(layer);
}


bool EvidyonGUIModule_InventoryList::getMouseOverItem(const ClientAvatarInventoryItem** inventory_item,
                                                      const ClientItemDescription** item_description) {
  int id;
  if (item_list_.getMouseOverButton(&id)) {
    if (inventory_item != NULL) *inventory_item = &inventory_[id];
    if (item_description != NULL) *item_description = &item_descriptions_[inventory_[id].type];
    return true;
  } else {
    return false;
  }
}

bool EvidyonGUIModule_InventoryList::getClickedItem(const ClientAvatarInventoryItem** inventory_item,
                                                    const ClientItemDescription** item_description) {
  int id;
  if (item_list_.getClickedButton(&id)) {
    if (inventory_item != NULL) *inventory_item = &inventory_[id];
    if (item_description != NULL) *item_description = &item_descriptions_[inventory_[id].type];
    return true;
  } else {
    return false;
  }
}


void EvidyonGUIModule_InventoryList::updateTooltip() {
  if (!tooltip_ || !isActive()) return;
  const ClientAvatarInventoryItem* inventory_item;
  const ClientItemDescription* item_description;
  if (getMouseOverItem(&inventory_item, &item_description)) {
    GUISize size;
    tooltip_description_.setItem(inventory_item,
                                 item_description,
                                &size);
    tooltip_->setContentSize(size);
    if (tooltip_->becomeOwner(this)) {
      tooltip_->showContentAfter(1.0);
      tooltip_->addContent(&tooltip_description_);
    }
  } else {
    tooltip_->releaseOwnership(this);
  }
}



}