#ifndef __ENCHANTMENT_H__
#define __ENCHANTMENT_H__

#include <dc/table>
#include "visualfx.h"

namespace Evidyon {
struct ClientEnchantmentDescription;
struct MagicDescriptionStageEffect;
}

class Enchantment : public dc::dcResource<Enchantment> {
public:
  Enchantment();
  void compileForClient(
      Evidyon::ClientEnchantmentDescription* description) const;
  void setProperties(int class_index,
                     unsigned int type_bits,
                     unsigned int inv_class_mask);

  // Sets the enchantment-specific parameters in the stage effect
  void fillMagicDescriptionStageEffect(
    Evidyon::MagicDescriptionStageEffect* enchantment_effect);

  // Zeroes out this class
  void reset();

  unsigned int getInvClassMask() const { return inv_class_mask_; }
  unsigned int getTypeBits() const { return type_bits_; }

public:
  static std::string staticTypeName();

private:
  dc::dcString description_; // totally internal for the editor
  dc::dcTable<VisualFX>::Reference visual_fx_; // for client

  // for the magic descriptions on the server
  int class_index_;
  unsigned int type_bits_;
  unsigned int inv_class_mask_;
};


#endif