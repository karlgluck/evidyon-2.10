#ifndef __ENCHANTMENTCLASSIMPL_H__
#define __ENCHANTMENTCLASSIMPL_H__

#include "enchantment.h"
#include "enchantmentclass.h"
#include "../shared/client-editor/clientenchantmentdescription.h"

template <const int BITS> class EnchantmentClassImpl :
public dc::dcResource<EnchantmentClassImpl<BITS>, EnchantmentClass> {
public:
  static const int TYPES_IN_CLASS = (1<<BITS)-1;

public:
  EnchantmentClassImpl() {
    // this constructor should not normally be used; however,
    // it is necessary that it be implemented for importing.
    for (int i = 0; i < TYPES_IN_CLASS; ++i) {
      member(EnchantmentClass::stringOfEnchantmentClassBits(BITS) + std::string(" enchantment #") +
             EnchantmentClass::stringOfNumber(i),
            &enchantments_[i]);
    }
  }

  EnchantmentClassImpl(const std::string& class_name,
                       int first_enchantment_index,
                       int first_bit, bool visible) {
    first_enchantment_index_ = first_enchantment_index;
    first_bit_ = first_bit;
    for (int i = 0; i < TYPES_IN_CLASS; ++i) {
      member(EnchantmentClass::stringOfEnchantmentClassBits(BITS) + std::string(" enchantment #") +
             EnchantmentClass::stringOfNumber(i),
            &enchantments_[i]);
      unsigned int inv_class_mask, type_bits;
      if (visible) {
        inv_class_mask = ~(((~(unsigned)0)>>((sizeof(unsigned int)*8)-BITS))<<first_bit);
        type_bits = ((unsigned)(i+1))<<first_bit;
      } else {
        inv_class_mask = ~(unsigned)0;
        type_bits = 0;
      }
      enchantments_[i].setProperties(first_enchantment_index + i,
                                     type_bits, inv_class_mask);
    }
  }

  virtual int getEnchantmentTypesInClass() const {
    return TYPES_IN_CLASS;
  }

  virtual int getBitFieldSize() const {
    return BITS;
  }

  virtual void compileForClient(Evidyon::ClientEnchantmentClassDescription* description) {
    description->mask.first_bit = first_bit_;
    description->mask.type_mask = ~((~(unsigned)0L)<<BITS); // create 'BITS' 1s starting at LSB
    description->first_enchantment_index = first_enchantment_index_;
  }

  virtual Enchantment* getEnchantment(unsigned int index) {
    return index < TYPES_IN_CLASS ? &enchantments_[index] : NULL;
  }

public:
  static std::string staticTypeName() {
    return std::string("EnchantmentVisualizationClass") +
           EnchantmentClass::stringOfEnchantmentClassBits(BITS);
  } 

private:
  int first_enchantment_index_, first_bit_;
  Enchantment enchantments_[TYPES_IN_CLASS];
};


#endif