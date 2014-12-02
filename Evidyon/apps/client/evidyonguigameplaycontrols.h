#ifndef __EVIDYONGUIGAMEPLAYCONTROLS_H__
#define __EVIDYONGUIGAMEPLAYCONTROLS_H__


#include "evidyonguimodule_inventorylist.h"
#include "evidyonguimodule_spelllist.h"
#include "evidyonguimodule_storagelist.h"
#include "evidyonguigamedialogmenu.h"
#include "evidyonguistatusbars.h"
#include "evidyonguitooltipbox.h"
#include "actiontargeter.h"
#include "evidyonguiactionbindingdescription.h"
//#include "evidyonguimodule_geosiddialog.h"

#include "../common/gui/guikeyboard.h"
#include "evidyonguikeyboundactionssentinel.h"

#include "chattextline.h"

#include "volucrisguistatdisplay.h"



namespace Evidyon {

class GlobalActorManager;

struct KeyBinding {
  Action action;
};



class EvidyonGUIGameplayControls {
  class KeyboardSentinel : public GUIKeyboardSentinel {
  public:
    KeyboardSentinel(EvidyonGUIGameplayControls* owner);
  public:
    virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  private:
    EvidyonGUIGameplayControls* owner_;
  };

  class MouseSentinel : public GUIMouseSentinel {
  public:
    MouseSentinel(EvidyonGUIGameplayControls* owner);

    // This method watches for the key binding shortcut: left + right click
    virtual bool onMouseButtonAction(GUIMouseButtonAction action,
                                     GUIPoint position,
                                     GUIMouseInputSource* mouse);
    // If you move the mouse more than a certain amount, the key binding
    // effect is canceled.
    virtual bool onMouseMove(GUIPoint position,
                             GUIPoint oldPosition,
                             GUIMouseInputSource* mouse);
  private:
    GUIPoint started_binding_position_;
    EvidyonGUIGameplayControls* owner_;
  };

public:
  enum Dialog {
    DIALOG_STAT_DISPLAY,
    DIALOG_INVENTORY,
    DIALOG_DROP_ITEMS,
    DIALOG_USE_ITEM,
    DIALOG_CAST_SPELL,
    DIALOG_CHAT,
    DIALOG_KEY_BINDINGS,
    DIALOG_STORAGE,
    DIALOG_GEOSID,
    DIALOG_NONE,
  };

public:
  EvidyonGUIGameplayControls();
  void create(const std::string& keys_file,
              GUIFont* font,
              GUILayer* primary_layer,
              GUIMouseSentinel* root_mouse_sentinel,
              GUIKeyboardSentinel* root_keyboard_sentinel,
              GUIGenericCanvas* mouse_cursor_canvas,
              const ClientItemDescription* item_descriptions,
              size_t number_of_item_descriptions,
              const ClientAvatarInventoryItem* inventory,
              const ClientSpellDescription* spells,
              size_t number_of_spells,
              SpellAvailabilityMask* spell_availability_mask,
              GUISize hpMpXpSize,
              IsometricCamera* camera,
              GlobalActorManager* global_actor_manager,
              const ItemInStorage* storage);

  // Frees all internal references
  void destroy();

  // Polls for user input changes and returns the latest action the
  // user wants to perform.
  // If the change_cursor_state variable is set to 'true', then the
  // cursor icon should be updated based on the return value from getCursorState
  Action::Type update(double time,
                      const ActionInstance** action_instance,
                      bool* change_cursor_state);

  // Returns '1' if the cursor should be in the the targeting state, or
  // '0' if it should be in the normal state.
  int getCursorState() const;

  // Rebuilds the inventory panels' contents.  This should be called whenever
  // the inventory is edited.
  void syncInventory();
  void syncSpells();
  void syncStorageItems();

//----------------
  // this is a super hackity hacky hack
  inline VolucrisGUIStatDisplay* syncAvatarStats() { return &stat_display_; }
//----------------


  void showDialog(Dialog dialog);

  // Shows a dialog if it isn't visible, hides all dialogs if it is.
  void toggleDialog(Dialog dialog);

  // This method returns 'false' if no menu was being shown.  This is so that
  // the escape key can be used to close any menu, and when no menu is being
  // displayed it can log the character out of the world.
  bool hideDialogs();


private:
  void setSplitInventoryCanvases();
  void setSingleInventoryCanvas();
  void setSpellCanvas();
  void setStorageCanvases();

private:
  void startBinding();
  void cancelBinding();
  bool isBinding();
  void bind(GUIKey key);

private:
  void saveKeyBindings();

private:
  std::string keys_file_;
  GUILayer* primary_layer_;
  EvidyonGUIGameDialogMenu dialog_menu_;
  ActionTargeter targeter_;
  EvidyonGUIModule_SpellList spells_;
  EvidyonGUIModule_InventoryList equipped_, unequipped_;
  EvidyonGUIModule_StorageList storage_;
  EvidyonGUITooltipBox tooltip_box_;
  SpellAvailabilityMask* spell_availability_mask_;
  ChatTextLine chat_text_line_;

  VolucrisGUIStatDisplay stat_display_;

  Dialog current_dialog_;

  // Whether or not the cursor was targeting something as of the
  // last call to update()
  bool last_update_cursor_targeting_;

private:
  KeyboardSentinel keyboard_sentinel_;
  MouseSentinel mouse_sentinel_;

  EvidyonGUIActionBindingDescription binding_description_;

  KeyBinding current_binding_;

  EvidyonGUIKeyBoundActionsSentinel key_bound_actions_;

};

}




#endif