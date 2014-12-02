#ifndef __COMPILEDTREASURE_H__
#define __COMPILEDTREASURE_H__

namespace Evidyon {

struct CompiledTreasure {
  unsigned int item_type; // TODO: quantity, qualities, etc...
};

static const int TREASURE_WEIGHTED_RANDOM_RANGE = 17;
static const int TREASURE_WEIGHTED_RANDOM_CENTER_INDEX = (TREASURE_WEIGHTED_RANDOM_RANGE - 1) / 2;


struct CompiledTreasureClass {
  // this lists are weighted toward the center index
  int number_of_items_to_drop[TREASURE_WEIGHTED_RANDOM_RANGE];

  // the treasure is selected by rand()%[[one of these indices]]
  int treasure_index_range[TREASURE_WEIGHTED_RANDOM_RANGE];

  // how many treasure descriptions are in this class (and follow this header)
  int treasure_item_count;
};

}


#endif