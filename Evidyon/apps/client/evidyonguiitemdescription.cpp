#include "evidyonguiitemdescription.h"
#include "../common/gui/guicanvas.h"
#include "../common/gui/guifont.h"
#include "../common/gui/guilayer.h"
#include <string>

namespace Evidyon {



void EvidyonGUIItemDescription::setFont(GUIFont* font) {
  description_.setFont(font);
}


void EvidyonGUIItemDescription::setItem(const ItemInStorage* storage_item,
                                        const ClientItemDescription* item_description,
                                        GUISize* panel_size) {
  std::string text;
  text.append(item_description->description);
  {
    GUIFont* font = description_.getFont();
    GUISize size;
    font->getTextExtent(text.c_str(), text.length(), &size);
    size.width += 40;
    size.width = max(size.width, 120);
    const int min_height = font->getCaretSize().height*3;
    size.height = max(min_height, size.height);
    size.width += 16;
    size.height += 16;
    *panel_size = size;
  }
  description_.setText(text.c_str());
  description_.setAlignment(GUIALIGN_LEFT_TOP);
}

void EvidyonGUIItemDescription::setItem(const ClientAvatarInventoryItem* inventory_item,
                                        const ClientItemDescription* item_description,
                                        GUISize* panel_size) {
  std::string text;
  /*int quantity = inventory_item->quantity;
  if (quantity > 1) {
    char num[16];
    sprintf_s(num, 16, "%i ", quantity);
    text.append(num);
  }
  text.append(item_description->name);
  text.append("\n");*/
  text.append(item_description->description);
  {
    GUIFont* font = description_.getFont();
    GUISize size;
    font->getTextExtent(text.c_str(), text.length(), &size);
    size.width += 40;
    size.width = max(size.width, 120);
    const int min_height = font->getCaretSize().height*3;
    size.height = max(min_height, size.height);
    size.width += 16;
    size.height += 16;
    *panel_size = size;
  }
  description_.setText(text.c_str());
  description_.setAlignment(GUIALIGN_LEFT_TOP);

/*
  // parse out the text to make sure it fits within the given region
  std::string description_text;
  int lines = 0;
  while (false == text.empty()) {
    size_t first_whitespace = text.find_first_of("\n");
    size_t length = first_whitespace;
    GUISize size;
    //if (!font->getTextExtent(text.substr(0, length).c_str(), -1, &size)) break;
    //if (size.width > width) {
      //length -= (int)((width-size.width) / (font->getCaretSize().height * 1.0f));
      do {
        if (!font->getTextExtent(text.substr(0, length).c_str(), -1, &size)) break;
      } while (size.width > width && ((length--) > 2));
    //}
    if (length != text.npos) {
      size_t last_length = length;
      while (length > 0 && text.at(length) != ' ') --length;
      if (length <= 1) {
        length = last_length;
      } else {
        first_whitespace = length;
      }
    }
    description_text.append(text.substr(0, length));
    description_text.append("\n");
    if (length == first_whitespace && length != text.npos) {
      ++length;
      while (text.size() > length && text.at(length) == ' ') ++length;
    }
    text.erase(0, length);
    ++lines;
  }*/

  //description_.setText(description_text.c_str());
}

void EvidyonGUIItemDescription::registerElements(GUILayer* layer) {
  GUICanvas* canvas = getCanvas();
  canvas->alignXCenter(0);
  canvas->alignYCenter(0);
  canvas->relativeWidth(-16);
  canvas->relativeHeight(-16);
  canvas->relativeDepth(-0.01f);
  description_.setSourceCanvas(canvas);
  layer->registerElement(&description_);
}

void EvidyonGUIItemDescription::unregisterElements(GUILayer* layer) {
  layer->unregisterElement(&description_);
}





}

