#ifndef __WORLDREGION_ITEMS_H__
#define __WORLDREGION_ITEMS_H__

#include <map>

namespace Evidyon {

class Item;
typedef std::pair<int,int> WorldRegion_Sublocation;
typedef std::multimap<WorldRegion_Sublocation, Item*> WorldRegion_LocalItems;


}

#endif