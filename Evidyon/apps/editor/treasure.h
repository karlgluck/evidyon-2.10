#ifndef __TREASURE_H__
#define __TREASURE_H__

#include <dc/list>
#include <dc/integer>
#include "item.h"


namespace Evidyon {
struct CompiledTreasure;
}


class Treasure : public dc::dcResource<Treasure> {
public:
  Treasure();

  // Saves the description of this treasure item into the structure
  void compileForServer(Evidyon::CompiledTreasure* description);

  void setItemReference(dc::dcList<Item>::Element* element) { item_.setReferencedResource(element); }
  int getTreasureValue() const;

public:

  // sorts by item value
  virtual int compareTo(const dc::dcGenericResource* other) const;


public:
  static std::string staticTypeName();

private:
  dc::dcList<Item>::Reference item_;  // the item this treasure represents
  dc::dcInteger value_modifier_;      // added to the item's innate value when sorting
};










#endif