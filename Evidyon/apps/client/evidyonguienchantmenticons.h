#ifndef __EVIDYONGUIENCHANTMENTICONS_H__
#define __EVIDYONGUIENCHANTMENTICONS_H__

#include "../shared/evidyon_enchantments.h"
#include "../common/gui/guicomponent.h"
#include "../common/gui/guicanvasmatrix.h"
#include "../common/gui/guipictureelement.h"



namespace Evidyon {

// Displays icons for the various enchantments
// It might be cool to add tooltips for the pictures so that
// players can see what they do
class EvidyonGUIEnchantmentIcons : public GUIComponent {
public:
  EvidyonGUIEnchantmentIcons();

  // Sets up the location of the canvases for this display.  The
  // icons will show up just underneath where the chat bar
  // is rendered.
  void init(GUICanvas* primary_canvas, GUISize hp_mp_xp_size);

  // Syncs the icon display to which enchantments are
  // currently active on the client's avatar.  This should
  // be called when no elements are registered.
  void update(bool enchantment_active[Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES]);

public:
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

private:
  // Internal method to get the source area of the GUI texture
  // that contains the icon for the given enchantment type.
  void getIconSrcRect(int enchantment_index, GUIRect* rect);

private:
  GUICanvasMatrix<1, Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES> canvas_matrix_;
  GUIPictureElement icons_[Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES];
};

}

#endif