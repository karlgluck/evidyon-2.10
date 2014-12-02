#include "evidyonguiactionbindingdescription.h"
#include "../common/gui/guicanvas.h"
#include "../common/gui/guifont.h"
#include "../common/gui/guilayer.h"


namespace Evidyon {



void EvidyonGUIActionBindingDescription::setFont(GUIFont* font) {
  description_.setFont(font);
}

void EvidyonGUIActionBindingDescription::set(const std::string& text, GUISize* panel_size) {
  {
    GUIFont* font = description_.getFont();
    GUISize size;
    font->getTextExtent(text.c_str(), text.length(), &size);
    size.width += 16;
    size.height += 16;
    *panel_size = size;
  }
  description_.setText(text.c_str());
  description_.setAlignment(GUIALIGN_LEFT_TOP);
}

void EvidyonGUIActionBindingDescription::registerElements(GUILayer* layer) {
  GUICanvas* canvas = getCanvas();
  canvas->alignXCenter(0);
  canvas->alignYCenter(0);
  canvas->relativeWidth(-16);
  canvas->relativeHeight(-16);
  canvas->relativeDepth(-0.01f);
  description_.setSourceCanvas(canvas);
  layer->registerElement(&description_);
}

void EvidyonGUIActionBindingDescription::unregisterElements(GUILayer* layer) {
  layer->unregisterElement(&description_);
}



}