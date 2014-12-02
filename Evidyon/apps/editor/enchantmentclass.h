#ifndef __ENCHANTMENTCLASS_H__
#define __ENCHANTMENTCLASS_H__

namespace Evidyon {
struct ClientEnchantmentClassDescription;
};

class Enchantment;

#include <dc/string> // for dc::dcGenericResource


class EnchantmentClass : public dc::dcGenericResource {
public:
  virtual Enchantment* getEnchantment(unsigned int index) = 0;
  virtual int getEnchantmentTypesInClass() const = 0;
  virtual int getBitFieldSize() const = 0;
  virtual void compileForClient(Evidyon::ClientEnchantmentClassDescription* description) = 0;

public:
  static std::string staticTypeName();
  static std::string stringOfNumber(int type);
  static std::string stringOfEnchantmentClassBits(int bits);
};




#endif