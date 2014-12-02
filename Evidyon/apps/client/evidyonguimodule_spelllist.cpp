#include "evidyonguimodule_spelllist.h"
#include "evidyonguitooltipbox.h"
#include "evidyonguikeyboundactionssentinel.h"



namespace Evidyon {


EvidyonGUIModule_SpellList::EvidyonGUIModule_SpellList() {
  tooltip_ = NULL;
  button_font_ = NULL;
  spell_descriptions_ = NULL;
  number_of_spell_descriptions_ = 0;
}


void EvidyonGUIModule_SpellList::create(GUICanvas* source_canvas,
                                        GUIFont* button_font,
                                        const ClientSpellDescription* spells,
                                        size_t number_of_spells,
                                        EvidyonGUITooltipBox* tooltip) {
  tooltip_ = tooltip;
  button_font_ = button_font;
  spell_descriptions_ = spells;
  number_of_spell_descriptions_ = number_of_spells;
  addComponent(&spell_list_);

  spell_list_.setLineHeight(30);

  GUICanvas* canvas = spell_list_.getCanvas();
  canvas->setSourceCanvas(source_canvas);
  canvas->absoluteWidth(400);
  canvas->absoluteHeight(300);
  canvas->alignXLeft(50);
  canvas->alignYTop(100);
  canvas->absoluteDepth(0.4f);

  tooltip_description_.setFont(button_font);
}

void EvidyonGUIModule_SpellList::destroy() {
  clearComponents();
  button_font_ = NULL;
  hide();
}

void EvidyonGUIModule_SpellList::sync(EvidyonGUIKeyBoundActionsSentinel* key_bindings,
                                      const SpellAvailabilityMask* spell_mask) {
  GUILayer* layer = hide();
  spell_list_.clear();
  for (int i = 0; i < number_of_spell_descriptions_; ++i) {
    if (spell_mask->isBitSet(i)) {
      VolucrisGUIButton* button = spell_list_.button(i);
      button->enable();
      button->setFont(button_font_);
      GUIKey bound_key = key_bindings->getKeyForSpell(i);
      if (bound_key != GUIKEY_NULL) {
        std::string text = spell_descriptions_[i].name;
        text.append(" (");
        text.append(GUIKeyboardKeyName(bound_key));
        text.append(")");
        button->setText(text.c_str());
      } else {
        button->setText(spell_descriptions_[i].name);
      }
      button->getTextLabelElement()->setAlignment(GUIALIGN_LEFT);
    }
  }
  show(layer);
}



bool EvidyonGUIModule_SpellList::getMouseOverSpell(const ClientSpellDescription** spell) {
  int id;
  if (spell_list_.getMouseOverButton(&id)) {
    if (spell != NULL) *spell = &spell_descriptions_[id];
    return true;
  } else {
    return false;
  }
}

bool EvidyonGUIModule_SpellList::getClickedSpell(int* spell_type,
                                                 const ClientSpellDescription** spell) {
  int id;
  if (spell_list_.getClickedButton(&id)) {
    if (spell_type != NULL) *spell_type = id;
    if (spell != NULL) *spell = &spell_descriptions_[id];
    return true;
  } else {
    return false;
  }
}



void EvidyonGUIModule_SpellList::updateTooltip() {
  if (!tooltip_ || !isActive()) return;
  const ClientSpellDescription* spell;
  if (getMouseOverSpell(&spell)) {
    GUISize size;
    tooltip_description_.setSpell(spell,
                                 &size);
    tooltip_->setContentSize(size);
    if (tooltip_->becomeOwner(this)) {
      tooltip_->showContentAfter(1.0);
      tooltip_->addContent(&tooltip_description_);
    }
  } else {
    tooltip_->releaseOwnership(this);
  }
}



GUICanvas* EvidyonGUIModule_SpellList::getCanvas() {
  return spell_list_.getCanvas();
}



}