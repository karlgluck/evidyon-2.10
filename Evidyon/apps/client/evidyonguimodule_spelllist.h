#ifndef __EVIDYONGUIMODULE_SPELLLIST_H__
#define __EVIDYONGUIMODUEL_SPELLLIST_H__



#include "evidyonguiscrollablebuttonlist.h"
#include "../common/gui/guimodule.h"
#include "../shared/client-editor/clientspelldescription.h"
#include "../shared/evidyon_avatar.h"
#include "evidyonguispelldescription.h"


class GUIFont;


namespace Evidyon {

class EvidyonGUITooltipBox;
class EvidyonGUIKeyBoundActionsSentinel;

class EvidyonGUIModule_SpellList : public GUIModule {
public:
  EvidyonGUIModule_SpellList();
  void create(GUICanvas* source_canvas,
              GUIFont* button_font,
              const ClientSpellDescription* spells,
              size_t number_of_spells,
              EvidyonGUITooltipBox* tooltip);
  void destroy();
  void sync(EvidyonGUIKeyBoundActionsSentinel* key_bindings, const SpellAvailabilityMask* spell_mask);
  bool getMouseOverSpell(const ClientSpellDescription** spell);
  bool getClickedSpell(int* spell_type,
                       const ClientSpellDescription** spell);
  void updateTooltip();

  GUICanvas* getCanvas();

private:
  GUIFont* button_font_;
  EvidyonGUIScrollableButtonList spell_list_;
  const ClientSpellDescription* spell_descriptions_;
  size_t number_of_spell_descriptions_;

  EvidyonGUITooltipBox* tooltip_;
  EvidyonGUISpellDescription tooltip_description_;
};

}


#endif