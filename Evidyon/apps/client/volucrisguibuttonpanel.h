//-----------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __VOLUCRISGUIBUTTONPANEL_H__
#define __VOLUCRISGUIBUTTONPANEL_H__

#include "../common/gui/guicanvasmatrix.h"


template <size_t B> class VolucrisGUIButtonPanel {
public:
  VolucrisGUIButtonPanel();
  bool create(GUIFont* font, LPDIRECT3DTEXTURE9 gui_texture, GUIMouseSentinel* mouse_sentinel);
  void destroy();
  void registerElements(GUILayer* layer);
  void unregisterElements(GUILayer* layer);

  inline void buttonsWidth(int width)
      { button_canvases_.absoluteWidth(width); }
  inline void buttonsHeight(int height)
      { button_canvases_.absoluteHeight(height); }
  inline GUICanvas* getCanvas()
      { return panel_.getCanvas(); }
  inline void setTitleText(const char* title)
      { panel_.setText(title); }
  inline VolucrisGUIButton* getButton(int b)
      { if (b<0||b>=B) return NULL; else return &buttons_[b]; }

private:
  GUICanvas client_canvas_;
  VolucrisGUITitledPanel panel_;
  GUICanvasMatrix<B,1> button_canvases_;
  VolucrisGUIButton buttons_[B];
};





//-----------------------------------------------------------------------------
// Inline Implementation
//-----------------------------------------------------------------------------



template <size_t B> bool VolucrisGUIButtonPanel<B>::create(
    GUIFont* font,
    LPDIRECT3DTEXTURE9 gui_texture,
    GUIMouseSentinel* mouse_sentinel) {
  panel_.setFont(font);
  for (size_t i = 0; i < B; ++i) {
    buttons_[i].setFont(font);
    buttons_[i].getMouseSentinel()->setParentMouseSentinel(mouse_sentinel);
  }
  return true;
}

template <size_t B> void VolucrisGUIButtonPanel<B>::destroy() {
  panel_.setFont(NULL);
  panel_.releaseGUITexture();
  for (size_t i = 0; i < B; ++i) {
    buttons_[i].setFont(NULL);
    buttons_[i].releaseGUITexture();
    buttons_[i].getMouseSentinel()->removeFromMouseSentinelChain();
  }
}


template <size_t B> void VolucrisGUIButtonPanel<B>::registerElements(
    GUILayer* layer) {
  panel_.registerElements(layer);
  for (size_t i = 0; i < B; ++i) {
    buttons_[i].registerElements(layer);
  }
}


template <size_t B> void VolucrisGUIButtonPanel<B>::unregisterElements(
    GUILayer* layer) {
  panel_.unregisterElements(layer);
  for (size_t i = 0; i < B; ++i) {
    buttons_[i].unregisterElements(layer);
  }
}


template <size_t B> VolucrisGUIButtonPanel<B>::VolucrisGUIButtonPanel() {
  client_canvas_.setSourceCanvas(panel_.getCanvas());
  client_canvas_.alignXCenter(0);
  client_canvas_.alignYCenter(0);
  client_canvas_.relativeWidth(0);
  client_canvas_.relativeHeight(0);
  client_canvas_.relativeDepth(-0.01f);
  button_canvases_.setSourceCanvas(&client_canvas_);
  button_canvases_.absoluteWidth(300);
  button_canvases_.absoluteHeight(B*40);
  button_canvases_.alignXCenter(0);
  button_canvases_.alignYCenter(+5);
  button_canvases_.relativeDepth(0.0f);
  for (size_t i = 0; i < B; ++i) {
    buttons_[i].getCanvas()->setSourceCanvas(
        button_canvases_.getCellCanvas(i, 0));
    buttons_[i].getCanvas()->relativeDepth(0.0f);
    buttons_[i].getCanvas()->relativeHeight(-4);
    buttons_[i].getCanvas()->relativeWidth(-12);
    buttons_[i].getCanvas()->alignXCenter(0);
    buttons_[i].getCanvas()->alignYCenter(0);
  }
}

#endif