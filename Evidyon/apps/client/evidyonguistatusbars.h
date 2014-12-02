#ifndef __EVIDYONGUISTATUSBARS_H__
#define __EVIDYONGUISTATUSBARS_H__


#include "../common/gui/guicomponent.h"
#include "../common/gui/guiborderedfillelement.h"
#include "../common/gui/guistretchfillelement.h"


namespace Evidyon {


class EvidyonGUIStatusBars : public GUIComponent {
public:
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

  void init(int screen_width, int screen_height);
  void set(float hp_percent, float mp_percent, float xp_percent);
  GUISize getSize();

private:
  GUICanvas hpBarCanvas, mpBarCanvas;
  GUICanvas barBackgroundCanvas;
  GUIBorderedFillElement barBackground;
  GUIStretchFillElement hpBar, mpBar;
  GUICanvas expBarCanvas;
  GUIStretchFillElement expBar;
};

}


#endif