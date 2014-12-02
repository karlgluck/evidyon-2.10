#ifndef __MAPLINK_H__
#define __MAPLINK_H__

#include <dc/list>
#include "maplocationspecialfunction.h"
#include "location.h"
#include "atlas.h"

namespace Evidyon {

struct CompiledMapLocationSpecialFunction;

}


class MapLink : public dc::dcResource<MapLink,MapLocationSpecialFunction> {
public:
  MapLink();
  virtual bool compileForServer(Evidyon::CompiledMapLocationSpecialFunction* description);

public:
  static std::string staticTypeName();

private:
  Location source_location_;          // where to jump from
  dc::dcList<Atlas>::Reference dest_map_; // the map to jump to (uses containing map if null)
  Location dest_location_;            // to the location to jump to

private:
  static dc::dcClass<MapLink>::Implements<MapLocationSpecialFunction> implementsSpecialFunction;
};





#endif