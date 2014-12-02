#include "clientenchantmentcontroller.h"
#include <memory.h>
#include <limits>
#include "../shared/server-editor/magicdescription.h"

#include <dc/debug>

// for mutate to gas form
//#include "client.h"

namespace Evidyon {

ClientEnchantmentController::ClientEnchantmentController() {
  reset(NULL);
}


void ClientEnchantmentController::reset(Client* owner) {
  owner_ = NULL;
  memset(enchantments_, 0, sizeof(enchantments_));
  needs_updating_ = false;
  next_enchantment_expiration_time_ = std::numeric_limits<double>::max();
  description_bits_ = 0;
}

void ClientEnchantmentController::clear() {
  memset(enchantments_, 0, sizeof(enchantments_));
  needs_updating_ = true;
  next_enchantment_expiration_time_ = std::numeric_limits<double>::max();
  // don't reset description bits--this is done by apply() and is used
  // to trigger broadcasting an actor description update
  //description_bits_ = 0;
}


bool ClientEnchantmentController::update(double time) {
  needs_updating_ = needs_updating_ || next_enchantment_expiration_time_ < time;
  return needs_updating_;
}


void ClientEnchantmentController::addEnchantment(
    double expiration_time,
    const MagicDescriptionStageEffect* effect,
    const MagicDescriptionStageEffect::Parameters* parameters) {

  int class_index = effect->enchantment_class_index;
  CONFIRM(class_index >= 0 &&
          class_index < Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES) else return;

  enchantments_[class_index].effect = effect;
  enchantments_[class_index].expiration_time = expiration_time;
  memcpy(&enchantments_[class_index].parameters,
         parameters, sizeof(enchantments_[class_index].parameters));

  if (expiration_time < next_enchantment_expiration_time_) {
    next_enchantment_expiration_time_ = expiration_time;
  }
  
  needs_updating_ = true;
}


bool ClientEnchantmentController::apply(double time, AvatarAttributes::Enchantable* attributes, CorporealState* actor_state) {
  needs_updating_ = false;
  unsigned int old_description_bits = description_bits_;
  unsigned int new_description_bits = 0;
  double next_enchantment_expiration_time = std::numeric_limits<double>::max();
  for (int i = 0; i < Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES; ++i) {
    const MagicDescriptionStageEffect* effect = enchantments_[i].effect;
    const MagicDescriptionStageEffect::Parameters* params = &enchantments_[i].parameters;
    if (effect == NULL) continue;
    if (time > enchantments_[i].expiration_time) {
      enchantments_[i].effect = NULL;
      continue;
    }
    new_description_bits &= effect->enchantment_inv_class_mask;
    new_description_bits |= effect->enchantment_type_bits;

    switch (effect->type) {
      case MagicDescriptionStageEffect::ENCHANT_HP: attributes->hp_regen_rate += params->int_param; break;
      case MagicDescriptionStageEffect::ENCHANT_MP: attributes->mp_regen_rate += params->int_param; break;
      case MagicDescriptionStageEffect::ENCHANT_MUTATE_COPY: // int_param is set by the casting class
      case MagicDescriptionStageEffect::ENCHANT_MUTATE: attributes->actor_type = params->int_param;break;
      case MagicDescriptionStageEffect::ENCHANT_MUTATE_GASFORM: {
        if (*actor_state == NORMAL) {
          *actor_state = SPIRIT_FORM;
        }
      } break;
      case MagicDescriptionStageEffect::ENCHANT_ATTACK_RUN: break;
      case MagicDescriptionStageEffect::ENCHANT_MAXHP: attributes->max_hp *= params->float_param; break;
      case MagicDescriptionStageEffect::ENCHANT_MAXMP: attributes->max_mp *= params->float_param; break;
      case MagicDescriptionStageEffect::ENCHANT_PHYSICAL_DAMAGE: attributes->random_attack_damage += params->int_param; break;
      case MagicDescriptionStageEffect::ENCHANT_PHYSICAL_DEFENSE: attributes->physical_defense += params->int_param; break;
      case MagicDescriptionStageEffect::ENCHANT_MAGICAL_DEFENSE: attributes->magical_defense  += params->int_param; break;
      case MagicDescriptionStageEffect::ENCHANT_MOVEMENT_SPEED: attributes->speed += params->float_param; break;
      case MagicDescriptionStageEffect::ENCHANT_ATTACK_SPEED:
        attributes->attack_delay_time *= params->float_param;
        attributes->casting_delay_percent *= params->float_param; break;
    }

    double expiration_time = enchantments_[i].expiration_time;
    if (expiration_time < next_enchantment_expiration_time) {
      next_enchantment_expiration_time = expiration_time;
    }
  }
  description_bits_ = new_description_bits;
  next_enchantment_expiration_time_ = next_enchantment_expiration_time;
  return new_description_bits != old_description_bits;
}

unsigned int ClientEnchantmentController::getDescriptionBits() const {
  return description_bits_;
}



}