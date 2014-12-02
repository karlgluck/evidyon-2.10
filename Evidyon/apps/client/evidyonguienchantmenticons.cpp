#include "evidyonguienchantmenticons.h"
#include "volucrisguidefs.h"
#include "../common/gui/guilayer.h"

namespace Evidyon {

EvidyonGUIEnchantmentIcons::EvidyonGUIEnchantmentIcons() {
}


void EvidyonGUIEnchantmentIcons::init(GUICanvas* primary_canvas, GUISize hp_mp_xp_size) {
  GUICanvas* canvas = getCanvas();
  canvas->setSourceCanvas(primary_canvas);
  canvas->relativeWidth(0);
  canvas->relativeHeight(0);
  canvas->alignXLeft(0);
  canvas->alignYTop(0);
  canvas->absoluteDepth(0.9f);

  canvas_matrix_.setSourceCanvas(canvas);
  canvas_matrix_.relativeDepth(0.0f);
  canvas_matrix_.alignXLeft(8 + hp_mp_xp_size.width);
  canvas_matrix_.alignYBottom(-4);
  canvas_matrix_.absoluteWidth(Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES * (32+4));
  canvas_matrix_.absoluteHeight(32);

  for (int i = 0; i < Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES; ++i) {
    icons_[i].setSourceCanvas(canvas_matrix_.getCellCanvas(0, i));
    icons_[i].setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
  }

  update(NULL);
}


void EvidyonGUIEnchantmentIcons::update(bool enchantment_active[Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES]) {
  int icon_index = 0;

  if (enchantment_active != NULL) {
    for (int i = 0; i < Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES; ++i) {
      if (enchantment_active[i]) {
        GUIRect area;
        getIconSrcRect(i, &area);
        icons_[icon_index].setSourceRegion(&area, VolucrisGUIDefs::TEXTURE_SIZE);
        ++icon_index;
      }
    }
  }

  GUIRect area = { 0, 0, 0, 0 };
  while (icon_index < Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES) {
    icons_[icon_index++].setSourceRegion(&area, VolucrisGUIDefs::TEXTURE_SIZE);
  }
}


void EvidyonGUIEnchantmentIcons::registerElements(GUILayer* layer) {
  for (int i = 0; i < Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES; ++i) {
    layer->registerElement(&icons_[i]);
  }
}


void EvidyonGUIEnchantmentIcons::unregisterElements(GUILayer* layer) {
  for (int i = 0; i < Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES; ++i) {
    layer->unregisterElement(&icons_[i]);
  }
}


void EvidyonGUIEnchantmentIcons::getIconSrcRect(int enchantment_index, GUIRect* rect) {
  GUIPoint pt;
  switch (enchantment_index) {
    default:
    case 0:
      pt.x = 2*32;
      pt.y = 7*32;
      break;
    case 1:
      pt.x = 2*32;
      pt.y = 5*32;
      break;
    case 2:
      pt.x = 2*32;
      pt.y = 3*32;
      break;
    case 3:
      pt.x = 2*32;
      pt.y = 4*32;
      break;
  }

  GUISize size = { 32, 32 };
  rect->set(pt, size);
}




}