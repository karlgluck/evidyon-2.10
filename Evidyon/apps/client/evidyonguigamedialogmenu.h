#ifndef __EVIDYONGUIGAMEDIALOGMENU_H__
#define __EVIDYONGUIGAMEDIALOGMENU_H__

#include "../common/gui/guicomponent.h"
#include "volucrisguibutton.h"
#include "../common/gui/guicanvasmatrix.h"
#include "useractionqueue.h"

namespace Evidyon {

class EvidyonGUIKeyBoundActionsSentinel;

class EvidyonGUIGameDialogMenu : public GUIComponent {
public:
  enum Button {
    BUTTON_INVENTORY,
    BUTTON_AVATARSTATS,
    BUTTON_DROP,
    BUTTON_CAST,
    BUTTON_USEITEM,
    NUMBER_OF_MENU_BUTTONS,
    BUTTON_CLOSE_MENU,
    BUTTON_CHANGE_VIEWMODE,
    BUTTON_INVALID,
  };

public:
  EvidyonGUIGameDialogMenu();
  void setFont(GUIFont* font);
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);
  virtual GUIMouseSentinel* getMouseSentinel();

  void sync(EvidyonGUIKeyBoundActionsSentinel* key_bindings);

  static const char* getButtonText(Button button);
  static UserEventType getButtonEventType(Button button);

  // Checks all of the selection buttons to see if one has been pressed
  Button pollSelectionButtons();

private:
  GUIMouseSentinel mouse_sentinel_;
  GUICanvasMatrix<1,NUMBER_OF_MENU_BUTTONS> menu_buttons_matrix_;
  VolucrisGUIButton menu_buttons_[NUMBER_OF_MENU_BUTTONS];
  VolucrisGUIButton close_menu_;
};



}



#endif