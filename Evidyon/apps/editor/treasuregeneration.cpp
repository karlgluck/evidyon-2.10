#include "treasuregeneration.h"

TreasureGeneration::TreasureGeneration() {
  for (int i = 0; i < (int)RARITY_SCALE_SIZE; ++i) {
    char name[128];
    sprintf_s(name, "Monster Level to Boost Rarity %i Level%s", i + 1, i == 0 ? "" : "s");
    member(name, &monster_level_treasure_rarity_boost_[i]);
  }

  member("Treasure Classes", &treasure_classes_);
}


void TreasureGeneration::compileRarityBoost(int level[RARITY_SCALE_SIZE]) {
  for (int i = 0; i < RARITY_SCALE_SIZE; ++i) {
    level[i] = monster_level_treasure_rarity_boost_[i].getValue();
  }
}


bool TreasureGeneration::compileForServer(dc::dcStreamOut* stream) {
  size_t classes = treasure_classes_.getMembers().size();
  CONFIRM(stream->write(&classes, sizeof(classes))) else return false;

  const dc::dcGenericResource::Array& elements = treasure_classes_.getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = elements.begin();
       i != elements.end(); ++i) {
    dc::dcList<TreasureClass>::Element* element = 
      (dc::dcList<TreasureClass>::Element*)(*i);
    CONFIRM(element->compileForServer(stream)) else return false;
  }

  return true;
}


std::string TreasureGeneration::staticTypeName() {
  return "TreasureGeneration";
}