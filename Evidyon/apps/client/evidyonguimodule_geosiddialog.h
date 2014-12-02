#ifndef __EVIDYONGUIMODULE_GEOSIDDIALOG_H__
#define __EVIDYONGUIMODULE_GEOSIDDIALOG_H__



#include "evidyonguiscrollablebuttonlist.h"
#include "../common/gui/guimodule.h"
#include "../shared/client-editor/client_gamefile.h"
#include "../shared/evidyon_avatarinventory.h"
#include "evidyonguiitemdescription.h"


namespace Evidyon {

class EvidyonGUITooltipBox;
class EvidyonGUIKeyBoundActionsSentinel;

class EvidyonGUIModule_GeosidDialog : public GUIModule {
public:
  EvidyonGUIModule_GeosidDialog();
  void create(GUICanvas* source_canvas,
              GUIFont* button_font,
              const ClientItemDescription* item_descriptions,
              size_t number_of_item_descriptions,
              const ClientAvatarInventoryItem* inventory,
              EvidyonGUITooltipBox* tooltip);
  void destroy();

  GUICanvas* getCanvas();

  // Updates the list with the current state of the inventory
  void sync();

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