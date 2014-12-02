#include "evidyonguimodule_geosiddialog.h"
#include "evidyonguitooltipbox.h"

namespace Evidyon {



EvidyonGUIModule_GeosidDialog::EvidyonGUIModule_GeosidDialog() {
  tooltip_ = NULL;
  button_font_ = NULL;
  inventory_ = NULL;
  item_descriptions_ = NULL;
  number_of_item_descriptions_ = 0;
}



void EvidyonGUIModule_GeosidDialog::create(GUICanvas* source_canvas,
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



void EvidyonGUIModule_GeosidDialog::destroy() {
  clearComponents();
  button_font_ = NULL;
  hide();
}

GUICanvas* EvidyonGUIModule_GeosidDialog::getCanvas() {
  return item_list_.getCanvas();
}



void EvidyonGUIModule_GeosidDialog::sync() {
  GUILayer* layer = hide();
  item_list_.clear();
  for (int i = 0; i < Evidyon::AVATAR_INVENTORY_SIZE; ++i) {
    size_t type = inventory_[i].type;
    int quantity = inventory_[i].quantity;
    bool correct_state = inventory_[i].equipped == 0;
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
  show(layer);
}


bool EvidyonGUIModule_GeosidDialog::getMouseOverItem(const ClientAvatarInventoryItem** inventory_item,
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

bool EvidyonGUIModule_GeosidDialog::getClickedItem(const ClientAvatarInventoryItem** inventory_item,
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


void EvidyonGUIModule_GeosidDialog::updateTooltip() {
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
