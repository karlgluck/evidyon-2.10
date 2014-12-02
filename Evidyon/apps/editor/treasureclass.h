#ifndef __TREASURECLASS_H__
#define __TREASURECLASS_H__


#include <dc/list>
#include <dc/integer>
#include "treasure.h"
#include "rarityscale.h"


const char* GetRarityValueName(Rarity scale);


class TreasureClass : public dc::dcResource<TreasureClass> {
public:
  TreasureClass();

  // Writes this class into the output stream.  Called by TreasureGeneration
  bool compileForServer(dc::dcStreamOut* stream);
  dc::dcList<Treasure>* getTreasureList() { return &treasure_; }

public:
  static std::string staticTypeName();


private:
  dc::dcList<Treasure> treasure_;

  // Reference to items that match the appropriate rarity on the
  // scale.  These should be selected from the this class's
  // list of treasure.
  dc::dcList<Treasure>::Reference rarity_scale_[RARITY_SCALE_SIZE];

  // How many items to drop, given a randomly selected value.  This is
  // independent of the random value used to select the actual items.
  dc::dcInteger number_of_items_to_drop_[RARITY_SCALE_SIZE];
};




#endif