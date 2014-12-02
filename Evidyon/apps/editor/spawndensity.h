#ifndef __SPAWNDENSITY_H__
#define __SPAWNDENSITY_H__

#include <dc/integer> // for dc::dcObject

class SpawnDensity : public dc::dcObject<SpawnDensity, int> {
public:
  static std::string staticTypeName() {
    return "Spawn Density";
  }

  //std::string toString() {
  //}
};


#endif