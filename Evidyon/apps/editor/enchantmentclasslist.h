#ifndef __ENCHANTMENTCLASSLIST_H__
#define __ENCHANTMENTCLASSLIST_H__

#include <dc/string> // for dc::dcResource/dcStream
//class EnchantmentClass;
#include "../shared/evidyon_enchantments.h"
#include "enchantment.h"

//static const int NUMBER_OF_ENCHANTMENT_CLASSES = 10;

class EnchantmentClassList : public dc::dcResource<EnchantmentClassList> {
  //static const int TWO_BIT_CLASSES = 4;   // = 4*2 = 8 bits
  //static const int THREE_BIT_CLASSES = 2; // = 2*3 = 6 bits
  //static const int TOTAL_CLASS_BITS = 16;
  //static const int VISIBLE_ONE_BIT_CLASSES = 2;//TOTAL_CLASS_BITS - (TWO_BIT_CLASSES*2 + THREE_BIT_CLASSES*3);
  //static const int TOTAL_CLASSES = 10;
  //static const int INVISIBLE_ONE_BIT_CLASSES = 2;//TOTAL_CLASSES - (VISIBLE_ONE_BIT_CLASSES +
  //                                               //                 TWO_BIT_CLASSES +
  //                                               //                 THREE_BIT_CLASSES);
public:
  EnchantmentClassList();
  ~EnchantmentClassList();

  bool compileForClient(dc::dcStreamOut* stream);

  void reset();

public:
  static std::string staticTypeName();

private:
  //EnchantmentClass* classes_[TOTAL_CLASSES];
  Enchantment enchantments_[Evidyon::Enchantments::TOTAL_ENCHANTMENT_TYPES];
};



#endif