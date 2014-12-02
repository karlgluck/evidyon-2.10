#include "maplink.h"
#include "../shared/server-editor/maplocationspecialfunction.h"


dc::dcClass<MapLink>::Implements<MapLocationSpecialFunction> MapLink::implementsSpecialFunction;

MapLink::MapLink() {
  member("Source Location", &source_location_);
  member("Destination Map", &dest_map_);
  member("Dest Location", &dest_location_);
}

bool MapLink::compileForServer(Evidyon::CompiledMapLocationSpecialFunction* description) {
  description->type = Evidyon::MAP_LINK;
  description->x = source_location_.getX();
  description->y = source_location_.getY();
  description->map_link.x = dest_location_.getX()+0.5f;
  description->map_link.y = dest_location_.getY()+0.5f;
  description->map_link.map_index = dest_map_.getReferencedResourceIndex();
  return true;
}


std::string MapLink::staticTypeName() {
  return "MapLink";
}

