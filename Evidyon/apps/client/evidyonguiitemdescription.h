#ifndef __EVIDYONGUIITEMDESCRPITION_H__
#define __EVIDYONGUIITEMDESCRPITION_H__



#include "../common/gui/guicomponent.h"
#include "../shared/client-editor/client_gamefile.h"
#include "../shared/evidyon_avatarinventory.h"
#include "../common/gui/guitextlabelelement.h"


namespace Evidyon {

struct ItemInStorage;

class EvidyonGUIItemDescription : public GUIComponent {
public:
  void setFont(GUIFont* font);
  void setItem(const ClientAvatarInventoryItem* inventory_item,
               const ClientItemDescription* item_description,
               GUISize* panel_size);
  void setItem(const ItemInStorage* storage_item,
               const ClientItemDescription* item_description,
               GUISize* panel_size);

  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

private:
  GUITextLabelElement description_;
};

}



#endif