#include "randint32.h"
#include "../common/mtrand/mtrand.h"


unsigned int rand_uint32() {
  static MTRand_int32 generator;
  return generator();
}