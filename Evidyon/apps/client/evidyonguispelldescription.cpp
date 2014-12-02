#include <d3dx9.h>
#include "evidyonguispelldescription.h"
#include "../common/gui/guicanvas.h"
#include "../common/gui/guifont.h"
#include "../common/gui/guilayer.h"
#include <string>



namespace Evidyon {


void EvidyonGUISpellDescription::setFont(GUIFont* font) {
    description_.setFont(font);
}

void EvidyonGUISpellDescription::setSpell(const ClientSpellDescription* spell,
                                          GUISize* panel_size) {
  std::string text;
  text.append(spell->description);
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


void EvidyonGUISpellDescription::registerElements(GUILayer* layer) {
  GUICanvas* canvas = getCanvas();
  canvas->alignXCenter(0);
  canvas->alignYCenter(0);
  canvas->relativeWidth(-16);
  canvas->relativeHeight(-16);
  canvas->relativeDepth(-0.01f);
  description_.setSourceCanvas(canvas);
  layer->registerElement(&description_);
}

void EvidyonGUISpellDescription::unregisterElements(GUILayer* layer)  {
  layer->unregisterElement(&description_);
}



}

