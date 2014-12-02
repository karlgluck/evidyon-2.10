#ifndef __SHARED_MAPLOCATIONSPECIALFUNCTION_H__
#define __SHARED_MAPLOCATIONSPECIALFUNCTION_H__


namespace Evidyon {

enum MapLocationSpecialFunctionType {
  MAP_LINK,
  HP_POOL,
  MP_POOL,
};


struct CompiledMapLocationSpecialFunction {
  MapLocationSpecialFunctionType type;
  int x, y; // where on the map to place this function

  union {
    struct MapLink {
      int map_index;
      float x, y;
    } map_link;
  };
};

}



#endif