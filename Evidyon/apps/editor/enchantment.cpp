#include "enchantment.h"
#include "../shared/client-editor/clientenchantmentdescription.h"
#include "../shared/server-editor/magicdescription.h"


Enchantment::Enchantment() {
  member("Description", &description_);
  member("Visual FX", &visual_fx_);
}


void Enchantment::compileForClient(
    Evidyon::ClientEnchantmentDescription* description) const {
  description->visual_fx = visual_fx_.getReferencedResourceIndex();
}


void Enchantment::setProperties(int class_index,
                                unsigned int type_bits,
                                unsigned int inv_class_mask) {
  class_index_ = class_index;
  type_bits_ = type_bits;
  inv_class_mask_ = inv_class_mask;
}


void Enchantment::fillMagicDescriptionStageEffect(
    Evidyon::MagicDescriptionStageEffect* enchantment_effect) {
  enchantment_effect->enchantment_class_index = class_index_;
  enchantment_effect->enchantment_type_bits = type_bits_;
  enchantment_effect->enchantment_inv_class_mask = inv_class_mask_;
}


void Enchantment::reset() {
  description_.setValue("");
  visual_fx_.setGenericReferencedResource(NULL);
}


std::string Enchantment::staticTypeName() {
  return "Enchantment";
}

