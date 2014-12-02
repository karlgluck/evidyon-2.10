#include "worldregion.h"
#include <dc/debug>
#include "../common/null.h"
#include "globalworldmanager.h"
#include "actor.h"
#include "globalnetworkmanager.h"
#include "./packets/tellactorfulldescriptions-encoder.h"
#include "../shared/client-server/actordescription.h"
#include "./packets/tellactorfulldescriptions-encoder.h"
#include "../shared/client-server/actordescription.h"
#include "globalactormanager.h"
#include "encodepackets.h"
#include "item.h"
#include "globalitemmanager.h"

namespace Evidyon {

static const int MAX_ITEMS_PICKED_UP_AT_ONCE = 6;


void WorldRegion::acquire(Map* map,
                          WorldRegion* neighbors[8],
                          short region_x,
                          short region_y,
                          World::RegionID region_id) {
  map_ = map;
  region_x_ = region_x;
  region_y_ = region_y;
  region_id_ = region_id;
  center_x_ = region_x * World::REGION_SIZE + 0.5f * World::REGION_SIZE;
  center_y_ = region_y * World::REGION_SIZE + 0.5f * World::REGION_SIZE;
  left_ = region_x * World::REGION_SIZE;
  top_  = region_y * World::REGION_SIZE;

  // add all of the valid neighbors
  references_ = 0;
  number_of_neighbors_ = 0;
  for (int i = 0; i < 8; ++i) {
    if (neighbors[i] == NULL) continue;
    linkNewNeighbor(neighbors[i]);
  }

  for (int i = 0; i < NUMBER_OF_CLEAR_SETS; ++i) {
    locations_with_items_to_clear_[i].clearAll();
  }
}


void WorldRegion::release() {
  CONFIRM(local_actors_.empty() && local_clients_.empty()) else {
    local_actors_.clear();
    local_clients_.clear();
  }

  { // Erase items from this region
    GlobalItemManager* item_manager = GlobalItemManager::singleton();
    item_manager->releaseWorldRegionItems(&local_items_);
  }

  // Remove neighbor links
  const int number_of_neighbors = number_of_neighbors_;
  for (int i = 0; i < number_of_neighbors; ++i) {
    WorldRegion* neighbor = neighbors_[i];
    neighbors_[i] = NULL;
    CONFIRM(neighbor->removeNeighbor(this) == 1) else continue;
    neighbor->removeReference();
  }

  nearby_actors_.clear();
  nearby_clients_.clear();

  number_of_neighbors_ = 0;
}



void WorldRegion::enterWorld(Actor* actor) {
  local_actors_.insert(actor);
  nearby_actors_.insert(actor);

  const int number_of_neighbors = number_of_neighbors_;
  for (int i = 0; i < number_of_neighbors; ++i) {
    neighbors_[i]->nearby_actors_.insert(actor);
  }
}



void WorldRegion::enterWorld(Actor* actor,
                             WorldRegion** regions_entered,
                             int* number_of_regions_entered) {
  local_actors_.insert(actor);
  nearby_actors_.insert(actor);

  const int number_of_neighbors = number_of_neighbors_;
  *number_of_regions_entered = 1 + number_of_neighbors;
  for (int i = 0; i < number_of_neighbors; ++i) {
    WorldRegion* neighbor = neighbors_[i];
    regions_entered[i] = neighbor;
    neighbor->nearby_actors_.insert(actor);
  }
  regions_entered[number_of_neighbors] = this;
}



void WorldRegion::enterWorld(Client* client) {
  local_clients_.insert(client);
  nearby_clients_.insert(client);

  const int number_of_neighbors = number_of_neighbors_;
  for (int i = 0; i < number_of_neighbors; ++i) {
    WorldRegion* neighbor = neighbors_[i];
    neighbor->nearby_clients_.insert(client);
  }
}



void WorldRegion::enterWorld(Client* client,
                             WorldRegion** regions_entered,
                             int* number_of_regions_entered) {
  local_clients_.insert(client);
  nearby_clients_.insert(client);

  const int number_of_neighbors = number_of_neighbors_;
  *number_of_regions_entered = number_of_neighbors + 1;
  for (int i = 0; i < number_of_neighbors; ++i) {
    WorldRegion* neighbor = neighbors_[i];
    regions_entered[i] = neighbor;
    neighbor->nearby_clients_.insert(client);
  }
  regions_entered[number_of_neighbors] = this;
}

void WorldRegion::leaveWorld(Actor* actor) {
  local_actors_.erase(actor);
  nearby_actors_.erase(actor);

  const int number_of_neighbors = number_of_neighbors_;
  for (int i = 0; i < number_of_neighbors; ++i) {
    neighbors_[i]->nearby_actors_.erase(actor);
  }
}

void WorldRegion::leaveWorld(Actor* actor,
                             WorldRegion** regions_left,
                             int* number_of_regions_left) {
  local_actors_.erase(actor);
  nearby_actors_.erase(actor);

  const int number_of_neighbors = number_of_neighbors_;
  *number_of_regions_left = 1 + number_of_neighbors;
  for (int i = 0; i < number_of_neighbors; ++i) {
    WorldRegion* neighbor = neighbors_[i];
    regions_left[i] = neighbor;
    neighbor->nearby_actors_.erase(actor);
  }
  regions_left[number_of_neighbors] = this;
}


void WorldRegion::leaveWorld(Client* client) {
  local_clients_.erase(client);
  nearby_clients_.erase(client);

  const int number_of_neighbors = number_of_neighbors_;
  for (int i = 0; i < number_of_neighbors; ++i) {
    neighbors_[i]->nearby_clients_.erase(client);
  }
}



void WorldRegion::moveToRegion(Actor* actor, WorldRegion* new_region,
          WorldRegion** regions_left, int* number_of_regions_left,
          WorldRegion** regions_entered, int* number_of_regions_entered) {

  // Change local region
  local_actors_.erase(actor);
  new_region->local_actors_.insert(actor);

  typedef std::set<WorldRegion*> WorldRegionSet;

  // Build a set of the new region's neighbors
  WorldRegionSet new_nearby_regions;
  {
    new_nearby_regions.insert(new_region);
    const int number_of_new_region_neighbors = new_region->number_of_neighbors_;
    WorldRegion** new_region_neighbors = new_region->neighbors_;
    for (int i = 0; i < number_of_new_region_neighbors; ++i) {
      new_nearby_regions.insert(new_region_neighbors[i]);
    }
  }

  // Build a set of our local regions
  WorldRegionSet nearby_regions;
  {
    nearby_regions.insert(this);
    const int number_of_neighbors = number_of_neighbors_;
    WorldRegion** neighbors = neighbors_;
    for (int i = 0; i < number_of_neighbors; ++i) {
      nearby_regions.insert(neighbors[i]);
    }
  }

  // Find entries in the new set that aren't part of the old set
  int nearby_regions_entered = 0;
  for (WorldRegionSet::iterator i = new_nearby_regions.begin();
     i != new_nearby_regions.end(); ++i) {
    WorldRegion* region = *i;
    if (nearby_regions.end() == nearby_regions.find(region)) {
      region->nearby_actors_.insert(actor);
      *(regions_entered++) = region;
      nearby_regions_entered++;
    }
  }
  *number_of_regions_entered = nearby_regions_entered;

  // Find entries in the old set that aren't part of the new set
  int nearby_regions_left = 0;
  for (WorldRegionSet::iterator i = nearby_regions.begin();
     i != nearby_regions.end(); ++i) {
    WorldRegion* region = *i;
    if (new_nearby_regions.end() == new_nearby_regions.find(region)) {
      region->nearby_actors_.insert(actor);
      *(regions_left++) = region;
      nearby_regions_left++;
    }
  }
  *number_of_regions_left = nearby_regions_left;
}




void WorldRegion::changeRegionTo(Client* client, WorldRegion* new_region,
                                 WorldRegion** regions_left, int number_of_regions_left,
                                 WorldRegion** regions_entered, int number_of_regions_entered) {

  // Change the local region
  local_clients_.erase(client);
  new_region->local_clients_.insert(client);

  // Switch the nearby regions
  for (int i = 0; i < number_of_regions_left; ++i) {
    regions_left[i]->nearby_clients_.erase(client);
  }
  for (int i = 0; i < number_of_regions_entered; ++i) {
    WorldRegion* region_entered = regions_entered[i];
    region_entered->nearby_clients_.insert(client);
  }
}


/*
void WorldRegion::sendFullDescriptionOfNearbyActorsToClient(Client* client) {
  ::Evidyon::Network::Packet::TellActorFullDescriptionsPacketEncoder encoder;
  NetworkPeer* client_peer = client->getPeer();

  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  int enemies_found = 0;
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin();
     i != end; ++i) {
    Actor* actor = *i;
    ::Evidyon::Network::Packet::ActorDescriptionData description;
    actor->fillDescription(&description);
    encoder.add(&description, "ff", client_peer);
  }
  encoder.dispatch(client_peer);
}*/


void WorldRegion::addNearbyActorsToSlowSync(Client* client, Actor* client_actor) {
  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  int enemies_found = 0;
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin();
       i != end; ++i) {
    Actor* actor = *i;
    if (actor != client_actor &&
        actor->withinViewOf(client_actor)) {
      client->addToSlowSyncPacket(actor->getSyncData());
    }
  }
}


void WorldRegion::tellNearbyActorFullDescriptions(ActorID* actor_ids,
                                                  size_t number_of_actors,
                                                  NetworkPeer* peer) {
  using namespace ::Evidyon::Network::Packet;
  TellActorFullDescriptionsPacketEncoder encoder;
  GlobalActorManager* actor_manager = GlobalActorManager::singleton();
  for (size_t i = 0; i < number_of_actors; ++i) {
    Actor* actor = actor_manager->getActor(actor_ids[i]);

    // Skip actors that are not near this region
    if (nearby_actors_.find(actor) == nearby_actors_.end()) continue;

    ActorDescriptionData description;
    actor->fillDescription(&description);
    encoder.add(&description, peer);
  }
  encoder.dispatch(peer);
}


#include "../shared/server-editor/server_gamefile.h"
#include "../shared/client-server/packets.h"

bool WorldRegion::addItemOnGround(bool important, int x, int y, Item* item) {
  int local_x = x - left_;
  int local_y = y - top_;
  CONFIRM(local_x >= 0 &&
          local_y >= 0 &&
          local_x < World::REGION_SIZE &&
          local_y < World::REGION_SIZE &&
          item != NULL) else return false;

  // Add to the region
  Sublocation local_coord(local_x, local_y);
  local_items_.insert(LocalItems::value_type(local_coord,
                                             item));

  // Add this clear to the global world manager
  ItemIDType summation = 0;
  LocalItems::iterator i = local_items_.lower_bound(local_coord);
  LocalItems::iterator upper = local_items_.upper_bound(local_coord);
  for (; i != upper; ++i) summation += i->second->getUniqueID();
  getMap()->addClearItemsOnLocation(important, x, y, summation);
  

  return true;
}


void WorldRegion::broadcastUpdateItemsOnLocation(int x, int y) {
  using namespace ::Evidyon::Network::Packet::Server;
  int local_x = x - left_;
  int local_y = y - top_;
  Sublocation local_coord(local_x, local_y);
  LocalItems::iterator lower = local_items_.lower_bound(local_coord);
  LocalItems::iterator upper = local_items_.upper_bound(local_coord);

  unsigned short item_types[MAX_ITEMS_VISIBLE_PER_LOCATION];
  int number_of_item_types = 0;
  for (LocalItems::iterator i = lower;
       (i != upper) &&
       (number_of_item_types < MAX_ITEMS_VISIBLE_PER_LOCATION); ++i) {
    Item* item = i->second;
    CONFIRM(item) else return;
    item_types[number_of_item_types++] = item->getGroundType();
  }

  broadcastOrphanedNetworkPacket(Encode::tellItemsOnMapLocation(x,
                                                                y,
                                                                item_types,
                                                                number_of_item_types));

}




void WorldRegion::sendLocalItemsOnGroundToClient(Client* client) {
  using namespace ::Evidyon::Network::Packet::Server;
  LocalItems::iterator lower = local_items_.begin();
  LocalItems::iterator upper = local_items_.end();

  LocalItems::iterator i = lower;
  while (i != upper) {
    Sublocation current_local_coord = i->first;

    unsigned short item_types[MAX_ITEMS_VISIBLE_PER_LOCATION];
    int number_of_item_types = 0;
    while ((i != upper) &&
           (current_local_coord == i->first) &&
           (number_of_item_types < MAX_ITEMS_VISIBLE_PER_LOCATION)) {
      Item* item = i->second;
      CONFIRM(item) else return;
      item_types[number_of_item_types++] = item->getGroundType();
      ++i;
    }

    // skip past any other entries here
    if (number_of_item_types >= MAX_ITEMS_VISIBLE_PER_LOCATION) {
      while ((i != upper) && (current_local_coord == i->first)) ++i;
    }

    // calculate this location's actual coordinates
    int x = current_local_coord.first + left_;
    int y = current_local_coord.second + top_;

    // send the location's items
    client->sendAsyncPacket(Encode::tellItemsOnMapLocation(x,
                                                           y,
                                                           item_types,
                                                           number_of_item_types));
  }
}


void WorldRegion::pickUpItems(int x,
                              int y,
                              Client* client) {
  bool items_changed = false; // whether or not items were picked up
  int local_x = x - left_;
  int local_y = y - top_;
  Sublocation local_coord(local_x, local_y);
  LocalItems::iterator lower = local_items_.lower_bound(local_coord);
  LocalItems::iterator upper = local_items_.upper_bound(local_coord);
  if (lower == upper) return; // nothing here

  int inventory_indices[MAX_ITEMS_PICKED_UP_AT_ONCE];
  int items_picked_up = 0;
  int last_assigned_inventory_index = 0;
  LocalItems::iterator i = lower;
  while ((i != upper) &&
         (items_picked_up < MAX_ITEMS_PICKED_UP_AT_ONCE)) {
    Item* item = i->second;
    CONFIRM(item) else {
      ++i;
      continue;
    }
    bool given = client->giveItem(item,
                                  last_assigned_inventory_index,
                                 &last_assigned_inventory_index);
    if (false == given) break;

    // assign this item
    inventory_indices[items_picked_up] = last_assigned_inventory_index;

    // start searching at the next inventory index
    ++last_assigned_inventory_index;

    // move to the next item
    ++i;
    ++items_picked_up;
    items_changed = true;
  }

  // Erase the items that were picked up
  local_items_.erase(lower, i);

  if (items_changed) {
    client->syncInventory(inventory_indices, items_picked_up);
    broadcastUpdateItemsOnLocation(x, y);

    WorldRegion_Sublocation local_coord(local_x, local_y);
    LocalItems::iterator lower = local_items_.lower_bound(local_coord);
    LocalItems::iterator upper = local_items_.upper_bound(local_coord);
    if (lower != upper) {
      ItemIDType summation = 0;
      for (LocalItems::iterator i = lower; i != upper; ++i)
        summation += i->second->getUniqueID();
      getMap()->addClearItemsOnLocation(true, x, y, summation);
    }
  }
}


void WorldRegion::clearOldItemsOnGround() {
  //GlobalItemManager* item_manager = GlobalItemManager::singleton();

  //// iterate through all set bits in the current mask
  //for (int s = 0; s < LocationMask::MASK_SIZE; ++s) {
  //  LocationMask::WORD_TYPE word = locations_with_items_to_clear_[0].values[s];
  //  int index = s * LocationMask::BITS_IN_WORD;
  //  unsigned int sentinel = 0; // forces the loop to break after 32 times no matter what
  //  while (word > 0 && sentinel < 32) {
  //    ++sentinel;
  //    int bit = word & 1;
  //    if (bit != 0) {
  //      WorldRegion_Sublocation local_coord(index % World::REGION_SIZE,
  //                                          index / World::REGION_SIZE);
  //      LocalItems::iterator lower = local_items_.lower_bound(local_coord);
  //      LocalItems::iterator upper = local_items_.upper_bound(local_coord);
  //      if (lower != upper) {
  //        item_manager->releaseWorldRegionItems(&local_items_, lower, upper);

  //        // erase all items on the clients
  //        using namespace Evidyon::Network::Packet::Server;
  //        broadcastOrphanedNetworkPacket(Encode::tellItemsOnMapLocation(left_ + local_coord.first,
  //                                                                      top_ + local_coord.second,
  //                                                                      NULL,
  //                                                                      0));
  //      }
  //    }
  //    word >>= 1;
  //    ++index;
  //  }
  //}

  //{ // swap down each mask
  //  int last_i = 0;
  //  for (int i = 1; i < NUMBER_OF_CLEAR_SETS; ++i) {
  //    locations_with_items_to_clear_[last_i].copy(locations_with_items_to_clear_[i]);
  //    last_i = i;
  //  }

  //  locations_with_items_to_clear_[NUMBER_OF_CLEAR_SETS-1].clearAll();
  //}
}


void WorldRegion::clearItemsOnMapLocation(const ClearItemsOnMapLocation* clear_struct) {
  WorldRegion_Sublocation local_coord(clear_struct->x - left_, clear_struct->y - top_);
  LocalItems::iterator lower = local_items_.lower_bound(local_coord);
  LocalItems::iterator upper = local_items_.upper_bound(local_coord);
  if (lower != upper) {
    ItemIDType summation = 0;
    for (LocalItems::iterator i = lower; i != upper; ++i) {
      summation += i->second->getUniqueID();
    }
    if (summation == clear_struct->sum_of_unique_ids) {
      GlobalItemManager::singleton()->releaseWorldRegionItems(&local_items_, lower, upper);

      // erase all items on the clients
      using namespace Evidyon::Network::Packet::Server;
      broadcastOrphanedNetworkPacket(Encode::tellItemsOnMapLocation(left_ + local_coord.first,
                                                                    top_ + local_coord.second,
                                                                    NULL,
                                                                    0));
    }
  }
}

int WorldRegion::findActorEnemiesNearby(Actor* enemies_of_actor,
                                        float x,
                                        float y,
                                        float radius_squared,
                                        Actor** actors_found,
                                        int buffer_size) {
  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  int enemies_found = 0;
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin();
       (i != end) &&
       (enemies_found < buffer_size); ++i) {
    Actor* actor = *i;
    if ((actor != enemies_of_actor) &&
        actor->isAlive() &&
        actor->isEnemyOf(enemies_of_actor) &&
        actor->distanceToSq(x, y) < radius_squared) {
      *(actors_found++) = actor;
      ++enemies_found;
    }
  }
  return enemies_found;
}



int WorldRegion::findActorsNearby(Actor* actor,
                                  float radius_squared,
                                  Actor** actors_found,
                                  int buffer_size) {
  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  int number_of_actors_found = 0;
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin();
       (i != end) &&
       (number_of_actors_found < buffer_size); ++i) {
    Actor* test_actor = *i;
    if ((test_actor != actor) &&
        test_actor->distanceToSq(actor) < radius_squared) {
      *(actors_found++) = test_actor;
      ++number_of_actors_found;
    }
  }
  return number_of_actors_found;
}



bool WorldRegion::findClosestActor(Actor* actor,
                                   float search_radius_squared,
                                   Actor** actor_found) {
  float closest_distance_sq = search_radius_squared;
  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  bool actor_was_found = false;
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin(); (i != end); ++i) {
    Actor* test_actor = *i;
    if (test_actor == actor) continue;
    float distance_sq = test_actor->distanceToSq(actor);
    if (test_actor->distanceToSq(actor) < closest_distance_sq) {
      *actor_found = test_actor;
      closest_distance_sq = distance_sq;
      actor_was_found = true;
    }
  }
  return actor_was_found;
}



Actor* WorldRegion::collideProjectileWithActor(
  float x, float y, float next_x, float next_y) {
  static const float PROJECTILE_COLLISION_RADIUS_SQ = 0.5f;

  WorldRegion::ActorSet::iterator end = nearbyActorsEnd();
  for (WorldRegion::ActorSet::iterator i = nearbyActorsBegin();
     i != end; ++i) {
    Actor* actor = *i;
    if (actor->objectsCanMoveThrough()) continue;
    // TODO: this can be optimized by doing integer comparisons, or
    // doing long-range coarse checks.  right now this involves a
    // lot of floating-point multiplications
    float distance = actor->distanceToSq(next_x, next_y);
    if (distance < PROJECTILE_COLLISION_RADIUS_SQ &&
      distance < actor->distanceToSq(x,y)) {
      return actor;
    }
  }
  return NULL;
}


Actor* WorldRegion::collideActorWithActor(Actor* actor,
                                          float x,
                                          float y,
                                          float next_x,
                                          float next_y) {
  //static const float ACTOR_COLLISION_RADIUS_SQ = 0.5f;

  float actor_radius = actor->getSize();

  WorldRegion::ActorSet::iterator begin, end;
  // todo: enable this uniquelyContainsArea check to speed up processing!
  /*if (uniquelyContainsArea(x, y, actor_radius_sq)) {
    begin = localActorsBegin();
    end = localActorsEnd();
  } else*/ {
    begin = nearbyActorsBegin();
    end = nearbyActorsEnd();
  }

  for (WorldRegion::ActorSet::iterator i = begin; i != end; ++i) {
    Actor* check_collision_actor = *i;
    if (check_collision_actor == actor ||
        check_collision_actor->objectsCanMoveThrough()) continue;
    // TODO: this can be optimized by doing integer comparisons, or
    // doing long-range coarse checks.  right now this involves a
    // lot of floating-point multiplications
    float range = check_collision_actor->checkContactWith(next_x, next_y, actor_radius);
    if (range < 0.0f &&
        range < check_collision_actor->checkContactWith(x, y, actor_radius)) {
      return check_collision_actor;
    }
  }
  return NULL;
}


bool WorldRegion::collideActorWithMap(Actor* actor, float next_x, float next_y) {
  return false;
}



void WorldRegion::broadcastNetworkPacket(NetworkPacket* packet) {
  WorldRegion::ClientSet::iterator end = nearbyClientsEnd();
  for (WorldRegion::ClientSet::iterator i = nearbyClientsBegin();
      i != end; ++i) {
    Client* client = *i;
    GlobalNetworkManager::sendAsyncPacket(client, packet);
  }
}


void WorldRegion::broadcastOrphanedNetworkPacket(NetworkPacket* packet) {
  broadcastNetworkPacket(packet);
  GlobalNetworkManager::handleOrphanedPacket(packet);
}

void WorldRegion::linkNewNeighbor(WorldRegion* neighbor) {

#if defined(EXTENDED_DEBUGGING)
  // Make sure this isn't already a neighbor
  for (int i = 0; i < number_of_neighbors_; ++i) {
    CONFIRM(neighbors_[i] != neighbor) else {
      return;
    }
  }
  CONFIRM(neighbor->number_of_neighbors_ < 8) else {
    DEBUG_INFO("%4i %4i X %4i %4i", region_x_, region_y_,
      neighbor->region_x_, neighbor->region_y_);
    return;
  }
#endif

  neighbor->addReference(); // link this->neighbor
  neighbors_[number_of_neighbors_++] = neighbor;

  addReference(); // link neighbor->this
  neighbor->neighbors_[neighbor->number_of_neighbors_++] = this;

  // Get local members
  copyNearbyRegionLocalsFrom(neighbor);
}



void WorldRegion::copyNearbyRegionLocalsFrom(WorldRegion* source) {
  nearby_actors_.insert(source->local_actors_.begin(), source->local_actors_.end());
  nearby_clients_.insert(source->local_clients_.begin(), source->local_clients_.end());
}


int WorldRegion::removeNeighbor(WorldRegion* neighbor) {
  int links_removed = 0;
  for (int i = 0; i < number_of_neighbors_; ++i) {
    if (neighbors_[i] == neighbor) {
      neighbors_[i] = neighbors_[--number_of_neighbors_];
      neighbors_[number_of_neighbors_] = NULL;
      ++links_removed;
      neighbor->removeReference();
    }
  }
  ASSERT(links_removed==1);
  return links_removed;
}

}