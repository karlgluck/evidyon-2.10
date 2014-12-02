#ifndef __TREASUREGENERATION_H__
#define __TREASUREGENERATION_H__


#include <dc/list>
#include <dc/integer>
#include "treasureclass.h"



class TreasureGeneration : public dc::dcResource<TreasureGeneration> {
public:
  TreasureGeneration();

  // Used by the AI spawners during compilation
  void compileRarityBoost(int level[RARITY_SCALE_SIZE]);

  // Save the treasure classes into the output stream
  bool compileForServer(dc::dcStreamOut* stream);


public:
  static std::string staticTypeName();


private:
  // If the average level of a region is higher than the value in this chart,
  // monsters in that region generate treasure on a scale at minimum of
  // the rarity indicated.
  dc::dcInteger monster_level_treasure_rarity_boost_[RARITY_SCALE_SIZE];

  // The different kinds of treasure that can be generated.  Monsters refer
  // to these structures.
  dc::dcList<TreasureClass> treasure_classes_;
};




#endif
