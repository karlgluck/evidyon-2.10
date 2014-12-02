#ifndef __EVIDYONGUIMODULE_STORAGELIST_H__
#define __EVIDYONGUIMODULE_STORAGELIST_H__


#include "evidyonguiscrollablebuttonlist.h"
#include "../common/gui/guimodule.h"
#include "../shared/client-editor/client_gamefile.h"
#include "evidyonguiitemdescription.h"
#include "../shared/evidyon_storage.h"


namespace Evidyon {

class EvidyonGUITooltipBox;
class EvidyonGUIKeyBoundActionsSentinel;

class EvidyonGUIModule_StorageList : public GUIModule {
public:
  EvidyonGUIModule_StorageList();

  void create(GUICanvas* source_canvas,
              GUIFont* button_font,
              const ClientItemDescription* item_descriptions,
              size_t number_of_item_descriptions,
              const ItemInStorage* storage,
              EvidyonGUITooltipBox* tooltip);
  void destroy();

  void resetStorageContent();

  GUICanvas* getCanvas();

  // Updates the list with the current state of the storage
  void sync();

  bool getMouseOverItem(const ItemInStorage** storage_item,
                        const ClientItemDescription** item_description);
  bool getClickedItem(const ItemInStorage** storage_item,
                      const ClientItemDescription** item_description);

  void updateTooltip();


private:
  GUIFont* button_font_;
  const ItemInStorage* storage_;
  EvidyonGUIScrollableButtonList item_list_;
  const ClientItemDescription* item_descriptions_;
  size_t number_of_item_descriptions_;

  EvidyonGUITooltipBox* tooltip_;
  EvidyonGUIItemDescription tooltip_description_;
};

}



#endif