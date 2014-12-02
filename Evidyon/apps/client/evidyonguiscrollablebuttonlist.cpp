#include "evidyonguiscrollablebuttonlist.h"
#include "volucrisguidefs.h"

namespace Evidyon {

EvidyonGUIScrollableButtonList::Button::Button(EvidyonGUIScrollableButtonList* owner, int id) {
  id_ = id;
  owner_ = owner;
}

int EvidyonGUIScrollableButtonList::Button::getID() const {
  return id_;
}

void EvidyonGUIScrollableButtonList::Button::onStateChange(GUIButton::StateChange stateChange) {
  if (stateChange == GUIButton::STATECHANGE_PUSHED) {
    Button* old_button = owner_->mouse_clicked_button_;
    owner_->mouse_clicked_button_ = this;
    if (old_button != NULL)  old_button->resetPushed();
  } else if (stateChange == GUIButton::STATECHANGE_MOUSEOVER) {
    owner_->mouse_over_button_ = this;
  } else if (stateChange == GUIButton::STATECHANGE_MOUSEOUT) {
    if (owner_->mouse_over_button_ == this) {
      owner_->mouse_over_button_ = NULL;
    }
  }

  switch (stateChange) {
      case GUIButton::STATECHANGE_RESET: {
        getMouseSentinel()->releaseMouseFocus();
        myText.setTextColor(VolucrisGUIDefs::LIST_ENTRY_TEXT_COLOR);
        myBackground.setSourceRegion(
           &VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_NEUTRAL,
            VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
            VolucrisGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_DISABLED: {
        myText.setTextColor(VolucrisGUIDefs::LIST_ENTRY_TEXT_SPECIAL_COLOR);
        myBackground.setSourceRegion(
           &VolucrisGUIDefs::BUTTON_BACKGROUND_NEUTRAL,
            VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
            VolucrisGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_ENABLED:
        myText.setTextColor(VolucrisGUIDefs::LIST_ENTRY_TEXT_COLOR);

      case GUIButton::STATECHANGE_MOUSEOUT: {
        myBackground.setSourceRegion(
           &VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_NEUTRAL,
            VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
            VolucrisGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_MOUSEOVER: {
        myBackground.setSourceRegion(&VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_MOUSEOVER,
                                       VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
                                       VolucrisGUIDefs::TEXTURE_SIZE);

      } break;

      case GUIButton::STATECHANGE_MOUSEOVERANDDOWN: {
        myBackground.setSourceRegion(&VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_MOUSEOVERANDDOWN,
                                      VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
                                      VolucrisGUIDefs::TEXTURE_SIZE);

      } break;

      case GUIButton::STATECHANGE_PUSHED: {
        myBackground.setSourceRegion(&VolucrisGUIDefs::LIST_ENTRY_BACKGROUND_SELECTED,
                                      VolucrisGUIDefs::BUTTON_BORDER_PIXELS,
                                      VolucrisGUIDefs::TEXTURE_SIZE);
      } break;
  }

}

EvidyonGUIScrollableButtonList::EvidyonGUIScrollableButtonList() {
  mouse_over_button_ = NULL;
  mouse_clicked_button_ = NULL;

  GUICanvas* list_canvas = list_.getCanvas();
  list_canvas->setSourceCanvas(getCanvas());
  list_canvas->relativeWidth(0);
  list_canvas->relativeHeight(0);
  list_canvas->alignXLeft(0);
  list_canvas->alignYTop(0);
  list_canvas->relativeDepth(0.0f);
}

EvidyonGUIScrollableButtonList::~EvidyonGUIScrollableButtonList() {
  clear();
}

void EvidyonGUIScrollableButtonList::setLineHeight(int pixels) {
  list_.setLineHeight(pixels);
}

VolucrisGUIButton* EvidyonGUIScrollableButtonList::button(int id) {
  GUIComponent* component = list_.getComponent(id);
  if (!component) {
    component = new Button(this, id);
    GUICanvas* canvas = component->getCanvas();
    canvas->alignXLeft(0);
    canvas->alignYTop(0);
    canvas->relativeWidth(0);
    canvas->relativeHeight(0);
    canvas->relativeDepth(0.0f);
    list_.setComponent(id, component);
  }
  return reinterpret_cast<VolucrisGUIButton*>(component);
}

void EvidyonGUIScrollableList_DeleteComponentCallback(GUIComponent* component) {
  delete component;
}

void EvidyonGUIScrollableButtonList::clear() {
  mouse_over_button_ = NULL;
  mouse_clicked_button_ = NULL;
  list_.removeAllComponents(EvidyonGUIScrollableList_DeleteComponentCallback);
}

bool EvidyonGUIScrollableButtonList::getMouseOverButton(int* id) {
  if (NULL == mouse_over_button_) return false;
  *id = mouse_over_button_->getID();
  return true;
}

bool EvidyonGUIScrollableButtonList::getClickedButton(int* id) {
  if (NULL == mouse_clicked_button_) return false;
  mouse_clicked_button_->resetPushed();
  *id = mouse_clicked_button_->getID();
  mouse_clicked_button_ = NULL;
  return true;
}

GUIMouseSentinel* EvidyonGUIScrollableButtonList::getMouseSentinel() {
  return list_.getMouseSentinel();
}

GUIKeyboardSentinel* EvidyonGUIScrollableButtonList::getKeyboardSentinel() {
  return list_.getKeyboardSentinel();
}

void EvidyonGUIScrollableButtonList::registerElements(GUILayer* layer) {
  return list_.registerElements(layer);
}

void EvidyonGUIScrollableButtonList::unregisterElements(GUILayer* layer) {
  return list_.unregisterElements(layer);
}






}



