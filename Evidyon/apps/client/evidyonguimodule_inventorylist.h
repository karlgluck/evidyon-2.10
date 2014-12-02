#ifndef __EVIDYONGUIMODULE_INVENTORYLIST_H__
#define __EVIDYONGUIMODULE_INVENTORYLIST_H__


#include "evidyonguiscrollablebuttonlist.h"
#include "../common/gui/guimodule.h"
#include "../shared/client-editor/client_gamefile.h"
#include "../shared/evidyon_avatarinventory.h"
#include "evidyonguiitemdescription.h"


namespace Evidyon {

class EvidyonGUITooltipBox;
class EvidyonGUIKeyBoundActionsSentinel;

class EvidyonGUIModule_InventoryList : public GUIModule {
public:
  // Defines which of the inventory items this list displays.
  enum DisplayType {
    DISPLAY_CONSUMABLES,
    DISPLAY_EQUIPPED,
    DISPLAY_UNEQUIPPED,
  };

public:
  EvidyonGUIModule_InventoryList();

  void create(GUICanvas* source_canvas,
              GUIFont* button_font,
              const ClientItemDescription* item_descriptions,
              size_t number_of_item_descriptions,
              const ClientAvatarInventoryItem* inventory,
              EvidyonGUITooltipBox* tooltip);
  void destroy();

  GUICanvas* getCanvas();

  // Updates the list with the current state of the inventory
  void sync(DisplayType display_type,
            EvidyonGUIKeyBoundActionsSentinel* key_bindings);

  bool getMouseOverItem(const ClientAvatarInventoryItem** inventory_item,
                        const ClientItemDescription** item_description);
  bool getClickedItem(const ClientAvatarInventoryItem** inventory_item,
                      const ClientItemDescription** item_description);

  void updateTooltip();


private:
  GUIFont* button_font_;
  EvidyonGUIScrollableButtonList item_list_;
  const ClientAvatarInventoryItem* inventory_;
  const ClientItemDescription* item_descriptions_;
  size_t number_of_item_descriptions_;

  EvidyonGUITooltipBox* tooltip_;
  EvidyonGUIItemDescription tooltip_description_;
};

}



#endif