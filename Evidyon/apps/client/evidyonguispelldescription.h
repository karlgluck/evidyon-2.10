#ifndef __EVIDYONGUISPELLDESCRIPTION_H__
#define __EVIDYONGUISPELLDESCRIPTION_H__

#include "../common/gui/guicomponent.h"
#include "../shared/client-editor/clientspelldescription.h"
#include "../shared/evidyon_avatar.h"
#include "../common/gui/guitextlabelelement.h"


namespace Evidyon {

class EvidyonGUISpellDescription : public GUIComponent {
public:
  void setFont(GUIFont* font);
  void setSpell(const ClientSpellDescription* spell,
                GUISize* panel_size);

  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

private:
  GUITextLabelElement description_;
};

}


#endif