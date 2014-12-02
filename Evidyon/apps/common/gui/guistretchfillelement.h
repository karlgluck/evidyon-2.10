//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//
// GUI element to renders a quad that stretches to fill the entire region of its source canvas
//------------------------------------------------------------------------------------------------
#ifndef __GUISTRETCHFILLELEMENT_H__
#define __GUISTRETCHFILLELEMENT_H__


class GUIStretchFillElement : public GUIElement {
public:
  GUIStretchFillElement();
  void setTexture(int texture);
  void setSourceTextureRegion(const GUIRect* sourceArea, int texture, GUILayer* layer);
  void setSourceRegion(const GUIRect* sourceArea, GUISize textureDimensions);

public:
  bool render(GUILayer* layer);
  int getTexture();

protected:
  int texture_;
  float texcoords_[2][2][2];
  D3DCOLOR diffuse_;
};




#endif