#ifndef __WORLDREGION_H__
#error must be included by worldregion.h
#else
#ifndef __WORLDREGION_INLINE_H__
#define __WORLDREGION_INLINE_H__


WorldRegion::ActorSet::iterator WorldRegion::localActorsBegin()    { return local_actors_.begin(); }
WorldRegion::ActorSet::iterator WorldRegion::localActorsEnd()      { return local_actors_.end(); }
WorldRegion::ActorSet::iterator WorldRegion::nearbyActorsBegin()   { return nearby_actors_.begin(); }
WorldRegion::ActorSet::iterator WorldRegion::nearbyActorsEnd()     { return nearby_actors_.end(); }
WorldRegion::ClientSet::iterator WorldRegion::localClientsBegin()  { return local_clients_.begin(); }
WorldRegion::ClientSet::iterator WorldRegion::localClientsEnd()    { return local_clients_.end(); }
WorldRegion::ClientSet::iterator WorldRegion::nearbyClientsBegin() { return nearby_clients_.begin(); }
WorldRegion::ClientSet::iterator WorldRegion::nearbyClientsEnd()   { return nearby_clients_.end(); }

bool WorldRegion::neighboringRegionsHaveNoNearbyClients() const {
  bool all_empty = true;
  switch (number_of_neighbors_) { // unroll the loop...dunno if it's helpful though
    case 8: all_empty  = neighbors_[7]->nearby_clients_.empty();
    case 7: all_empty &= neighbors_[6]->nearby_clients_.empty();
    case 6: all_empty &= neighbors_[5]->nearby_clients_.empty();
    case 5: all_empty &= neighbors_[4]->nearby_clients_.empty();
    case 4: all_empty &= neighbors_[3]->nearby_clients_.empty();
    case 3: all_empty &= neighbors_[2]->nearby_clients_.empty();
    case 2: all_empty &= neighbors_[1]->nearby_clients_.empty();
    case 1: all_empty &= neighbors_[0]->nearby_clients_.empty();
      return all_empty;
    default: return nearby_clients_.empty();
  }
}

size_t WorldRegion::numberOfNearbyClients() const { return nearby_clients_.size(); }

bool WorldRegion::containsPoint(float x, float y) const {
  float dx = x-center_x_, dy = y-center_y_;
  return dx*dx+dy*dy < Evidyon::World::REGION_RADIUS_SQ;
}

inline bool WorldRegion::strictlyContainsPoint(int x, int y) const {
  return (x >= left_) &&
         (y >= top_) &&
         (x < (left_ + World::REGION_SIZE)) &&
         (y < (top_ + World::REGION_SIZE));
}

void WorldRegion::addReference() {
  ++references_;
}

void WorldRegion::removeReference() {
  --references_;
}

World::RegionID WorldRegion::getRegionID() const {
  return region_id_;
}

int WorldRegion::getRegionX() const { return region_x_; }
int WorldRegion::getRegionY() const { return region_y_; }

bool WorldRegion::uniquelyContainsArea(float x, float y, float radius_sq) const { return false; }

Map* WorldRegion::getMap() { return map_; }

}



#endif
#endif