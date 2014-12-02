#include "treasure.h"
#include "../shared/server-editor/compiledtreasure.h"


Treasure::Treasure() {
  member("Item", &item_);
  member("Treasure Level Modifier", &value_modifier_);
}

void Treasure::compileForServer(Evidyon::CompiledTreasure* description) {
  description->item_type = item_.getReferencedResourceIndex();
}

int Treasure::getTreasureValue() const {
  const dc::dcList<Item>::Element* my_item = item_.getReferencedResource();
  if (my_item == NULL) return 0;
  return my_item->getTreasureLevelValue() + value_modifier_.getValue();
}

int Treasure::compareTo(const dc::dcGenericResource* other) const {
  if (other == 0 || getTypeID() != other->getTypeID())
      return dc::dcGenericResource::compareTo(other);
  else {
    int my_value = getTreasureValue();
    int other_value = ((const Treasure*)other)->getTreasureValue();
    return my_value - other_value;
  }

}

std::string Treasure::staticTypeName() {
  return "Treasure";
}