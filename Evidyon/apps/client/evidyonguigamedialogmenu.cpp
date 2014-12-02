#include "evidyonguigamedialogmenu.h"
#include "evidyonguikeyboundactionssentinel.h"



namespace Evidyon {


EvidyonGUIGameDialogMenu::EvidyonGUIGameDialogMenu() {

  menu_buttons_matrix_.setSourceCanvas(getCanvas());
  menu_buttons_matrix_.alignXCenter(0);
  menu_buttons_matrix_.alignYTop(+16);
  menu_buttons_matrix_.relativeDepth(0.0f);
  menu_buttons_matrix_.absoluteWidth(800 - 40);
  menu_buttons_matrix_.absoluteHeight(35);

  for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
    menu_buttons_[i].getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
    GUICanvas* canvas = menu_buttons_[i].getCanvas();
    canvas->setSourceCanvas(menu_buttons_matrix_.getCellCanvas(0, i));
    canvas->alignXCenter(0);
    canvas->alignYCenter(0);
    canvas->relativeWidth(-8);
    canvas->relativeHeight(0);
    canvas->relativeDepth(0.0f);
  }
  close_menu_.getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);
  {
    GUICanvas* canvas = close_menu_.getCanvas();
    canvas->setSourceCanvas(getCanvas());
    canvas->alignXRight(-16);
    canvas->alignYBottom(-16);
    canvas->relativeDepth(0.0f);
    canvas->absoluteWidth(120);
    canvas->absoluteHeight(35);
    close_menu_.setText("close (ESC)");
  }
  sync(NULL);
  mouse_sentinel_.disable();
}


void EvidyonGUIGameDialogMenu::setFont(GUIFont* font) {
  for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
    menu_buttons_[i].setFont(font);
  }
  close_menu_.setFont(font);
}


void EvidyonGUIGameDialogMenu::registerElements(GUILayer* layer) {
  for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
    menu_buttons_[i].registerElements(layer);
  }
  close_menu_.registerElements(layer);
  mouse_sentinel_.enable();
}




void EvidyonGUIGameDialogMenu::unregisterElements(GUILayer* layer) {
  mouse_sentinel_.disable();
  for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
    menu_buttons_[i].unregisterElements(layer);
  }
  close_menu_.unregisterElements(layer);
}




GUIMouseSentinel* EvidyonGUIGameDialogMenu::getMouseSentinel() {
  return &mouse_sentinel_;
}

void EvidyonGUIGameDialogMenu::sync(EvidyonGUIKeyBoundActionsSentinel* key_bindings) {
  if (key_bindings == NULL) {
    for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
      menu_buttons_[i].setText(getButtonText((Button)i));
    }
  } else {
    for (int i = 0; i < NUMBER_OF_MENU_BUTTONS; ++i) {
      GUIKey bound_key = key_bindings->getKeyForUserEvent(getButtonEventType((Button)i));
      if (bound_key == GUIKEY_NULL) {
        menu_buttons_[i].setText(getButtonText((Button)i));
      } else {
        std::string text = getButtonText((Button)i);
        text.append(" (");
        text.append(GUIKeyboardKeyName(bound_key));
        text.append(")");
        menu_buttons_[i].setText(text.c_str());
      }
    }
  }
}

const char* EvidyonGUIGameDialogMenu::getButtonText(Button button) {
  switch (button) {
    case EvidyonGUIGameDialogMenu::BUTTON_INVENTORY:    return "inventory";
    case EvidyonGUIGameDialogMenu::BUTTON_AVATARSTATS:  return "stats";
    case EvidyonGUIGameDialogMenu::BUTTON_DROP:         return "drop item";
    case EvidyonGUIGameDialogMenu::BUTTON_CAST:         return "cast";
    case EvidyonGUIGameDialogMenu::BUTTON_USEITEM:      return "use item";
  }
  return NULL;
}

UserEventType EvidyonGUIGameDialogMenu::getButtonEventType(Button button) {
  switch (button) {
    case EvidyonGUIGameDialogMenu::BUTTON_INVENTORY:    return OPEN_INVENTORY;
    case EvidyonGUIGameDialogMenu::BUTTON_AVATARSTATS:  return OPEN_STAT_DISPLAY;
    case EvidyonGUIGameDialogMenu::BUTTON_DROP:         return OPEN_DROP;
    case EvidyonGUIGameDialogMenu::BUTTON_CAST:         return OPEN_CAST;
    case EvidyonGUIGameDialogMenu::BUTTON_USEITEM:      return OPEN_USEITEM;
  }
  return INVALID_ACTION;
}


EvidyonGUIGameDialogMenu::Button EvidyonGUIGameDialogMenu::pollSelectionButtons() {
  for (int check = 0; check < NUMBER_OF_MENU_BUTTONS; ++check) {
    if (menu_buttons_[check].wasPushed()) {
      menu_buttons_[check].resetPushed();
      return (Button)check;
    }
  }

  if (close_menu_.wasPushed()) {
    close_menu_.resetPushed();
    return BUTTON_CLOSE_MENU;
  }

  return BUTTON_INVALID;
}


}

