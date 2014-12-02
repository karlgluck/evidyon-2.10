#include "enchantmentclass.h"
#include <stdio.h>

std::string EnchantmentClass::staticTypeName() {
  return "EnchantmentClass";
}

std::string EnchantmentClass::stringOfNumber(int type) {
  char buffer[64];
  sprintf_s(buffer, sizeof(buffer), "%i", type);
  return std::string(buffer);
}

std::string EnchantmentClass::stringOfEnchantmentClassBits(int bits) {
  char buffer[64];
  sprintf_s(buffer, sizeof(buffer), " %i-Bit%s", bits, bits == 1 ? "" : "s");
  return std::string(buffer);
}