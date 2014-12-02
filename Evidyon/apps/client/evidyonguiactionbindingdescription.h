#ifndef __EVIDYONGUIACTIONBINDINGDESCRPITION_H__
#define __EVIDYONGUIACTIONBINDINGDESCRPITION_H__



#include "../common/gui/guicomponent.h"
#include "../shared/client-editor/client_gamefile.h"
#include "../shared/evidyon_avatarinventory.h"
#include "../common/gui/guitextlabelelement.h"
#include <string>


namespace Evidyon {


class EvidyonGUIActionBindingDescription : public GUIComponent {
public:
  void setFont(GUIFont* font);

  void set(const std::string& text, GUISize* panel_size);

  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

private:
  GUITextLabelElement description_;
};

}



#endif