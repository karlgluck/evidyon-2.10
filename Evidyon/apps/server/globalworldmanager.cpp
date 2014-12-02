#include "globalworldmanager.h"
#include "../common/null.h"
#include "../shared/evidyon_map.h"
#include "globalgamefilemanager.h"
#include "globalclientmanager.h"
#include "globalactormanager.h"
#include "globallifeformaimanager.h"

#include <math.h>
#include <dc/debug>
#include <set>
#include "../common/safemacros.h"

// need for GetTickCount
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


namespace Evidyon {

// this makes clearing items more granular since it
// doesn't need to be executed constantly
static const double ITEM_CLEARING_PERIOD = 10.0;


GlobalWorldManager*  GlobalWorldManager::singleton_ = NULL;

GlobalWorldManager* GlobalWorldManager::singleton() {
  return singleton_;
}

GlobalWorldManager::GlobalWorldManager() {
  number_of_maps_ = 0;
  maps_ = NULL;
  allocated_regions_ = 0;
  next_update_time_ = 0.0;
  next_time_to_clear_old_items_on_ground_ = 0.0;
}



bool GlobalWorldManager::create(int number_of_maps) {
  if (!region_memory_pool_.create()) return false;
  singleton_ = this;
  next_update_time_ = 0.0;
  next_time_to_clear_old_items_on_ground_ = 0.0;

  number_of_maps_ = number_of_maps;
  maps_ = new Map[number_of_maps];


  return true;
}

void GlobalWorldManager::destroy() {
  if (number_of_maps_ > 0) {
    for (int i = 0; i < number_of_maps_; ++i) {
      maps_[i].destroy();
    }
    SAFE_DELETE_ARRAY(maps_);
    number_of_maps_ = 0;
  }
  region_memory_pool_.destroy();
  singleton_ = NULL;
}



Map* GlobalWorldManager::getMap(int index) {
  if (index < 0 || index >= number_of_maps_) return NULL;
  return &maps_[index];
}


void GlobalWorldManager::update(double time) {

  for (int i = 0; i < number_of_maps_; ++i) {
    maps_[i].updateSpawn(time);
  }

  if (time > next_time_to_clear_old_items_on_ground_) {
    next_time_to_clear_old_items_on_ground_ = time + ITEM_CLEARING_PERIOD;
    for (int i = 0; i < number_of_maps_; ++i) {
      maps_[i].clearOldItemsOnGround();
    }
  }

  if (time < next_update_time_) return;

  // Check more often as more regions are allocated
  next_update_time_ = time + 1.0 * (MAX_WORLD_REGIONS - allocated_regions_);

  for (int i = 0; i < number_of_maps_; ++i) {
    maps_[i].checkForUnusedMapRegions();
  }
}

WorldRegion* GlobalWorldManager::allocateRegion(Map* map,
                                                WorldRegion* neighbors[8],
                                                short region_x,
                                                short region_y,
                                                World::RegionID region_id) {
  WorldRegion* region = region_memory_pool_.acquire();
  CONFIRM(region != NULL) else return NULL;
  region->acquire(map, neighbors, region_x, region_y, region_id);
  ++allocated_regions_;
  return region;
}

void GlobalWorldManager::deallocateRegion(WorldRegion* region) {
  region->release();
  CONFIRM(region_memory_pool_.release(region));
  --allocated_regions_;
}


Map::Map() {
  locations_ = NULL;
  special_functions_ = NULL;
  id_number_ = 0;
  width_ = 0;
  height_ = 0;
  region_swarm_spawners_ = NULL;
}


bool Map::createSpecialFunctions(int number_of_special_functions) {
  number_of_special_functions_ = 0;
  if (number_of_special_functions > 0) {
    special_functions_ = new MapLocationSpecialFunction[number_of_special_functions];
    CONFIRM(special_functions_) else { destroy(); return false; }
    memset(special_functions_,
           0x00000000,
           sizeof(MapLocationSpecialFunction) * number_of_special_functions);
    number_of_special_functions_ = number_of_special_functions;
  }
  return true;
}


bool Map::create(int id_number, int width, int height) {
  id_number_ = id_number;
  {
    locations_ = new MapLocationInfo[width*height];
    CONFIRM(locations_) else { destroy(); return false; }
    memset(locations_,
           0x00000000,
           sizeof(MapLocationInfo) * width * height);
    width_ = width;
    height_ = height;
  }
  { // initialize the spawn points
    regions_wide_ = width  / Evidyon::World::REGION_SIZE;
    regions_high_ = height / Evidyon::World::REGION_SIZE;
    region_swarm_spawners_ = new RegionSwarmSpawner[regions_wide_*regions_high_];
  }
  {
    inhabitants_ = NULL;
    number_of_inhabitants_ = 0;
  }
  return true;
}


void Map::destroy() {
  { // Release all regions
    GlobalWorldManager* manager = GlobalWorldManager::singleton();
    for (WorldRegions::iterator i = regions_.begin();
         i != regions_.end(); ++i) {
      WorldRegion* region = i->second;
      manager->deallocateRegion(region);
    }
    regions_.clear();
  }

  if (region_swarm_spawners_ != NULL)
  { // Get rid of the region swarm spawners
    for (int y = 0; y < regions_high_; ++y) {
      for (int x = 0; x < regions_wide_; ++x) {
        const int index = regions_wide_ * y + x;
        region_swarm_spawners_[index].destroy();
      }
    }
    SAFE_DELETE_ARRAY(region_swarm_spawners_);
  }
  regions_wide_ = 0;
  regions_high_ = 0;
  active_region_swarm_spawners_.clear();

  { // free inhabitants
    SAFE_DELETE_ARRAY(inhabitants_);
    number_of_inhabitants_ = 0;
    world_region_inhabitants_.clear();
  }

  SAFE_DELETE_ARRAY(special_functions_);
  number_of_special_functions_ = 0;
  SAFE_DELETE_ARRAY(locations_);
  width_ = 0;
  height_ = 0;
}

int Map::getIDNumber() const {
  return id_number_;
}

MapLocationInfo* Map::getMapInfo() {
  return locations_;
}


MapLocationSpecialFunction* Map::setLocationSpecialFunction(int index,
                                                            int x,
                                                            int y) {
  CONFIRM(x >= 0 && y >=0 &&
          x < width_ && y < height_ &&
          index < number_of_special_functions_) else return NULL;
  MapLocationInfo* location = &locations_[y*width_ + x];
  location->special_function_index = index + 1; /// The plus one is important
  return &special_functions_[index];
}

bool Map::createRegionSwarmSpawner(int region_x,
                                   int region_y,
                                   unsigned int treasure_randomizer_mask,
                                   const CompiledRegionSwarmSpawn* swarms,
                                   size_t number_of_swarms,
                                   const CompiledSwarmSpawnLocationInfo* locations) {
  CONFIRM(region_swarm_spawners_ && region_x >= 0 && region_y >= 0 &&
          region_x < regions_wide_ && region_y < regions_high_) else return false;
  int index = region_x + regions_wide_ * region_y;
#if defined(EXTENDED_DEBUGGING)
  ASSERT(index > 0 && index < width_*height_/(World::REGION_SIZE*World::REGION_SIZE)) else return false;
#endif

  return region_swarm_spawners_[index].create(treasure_randomizer_mask,
                                              swarms,
                                              number_of_swarms,
                                              locations);
}


void Map::createInhabitants(size_t number_of_inhabitants) {
  inhabitants_ = new Inhabitant[number_of_inhabitants];
  number_of_inhabitants_ = number_of_inhabitants;
}

void Map::setInhabitant(size_t inhabitant_index, int x, int y, size_t lifeform_ai_type) {
  ASSERT(inhabitant_index < number_of_inhabitants_) else return;
  Inhabitant* inhabitant = &inhabitants_[inhabitant_index];
  inhabitant->x = x;
  inhabitant->y = y;
  inhabitant->lifeform_ai_type = lifeform_ai_type;
  inhabitant->actor.reset();

  world_region_inhabitants_.insert(WorldRegionInhabitants::value_type(World::regionIDFromPoint(x, y), inhabitant));
}


void Map::checkForUnusedMapRegions() {
  typedef std::set<WorldRegion*> RegionSet;
  RegionSet regions_to_remove;
  for (WorldRegions::iterator i = regions_.begin();
       i != regions_.end(); ++i) {
    WorldRegion* region = i->second;
    if (region->neighboringRegionsHaveNoNearbyClients()) {
      regions_to_remove.insert(region);
    }
  }

  if (regions_to_remove.empty()) return;

  for (RegionSet::iterator i = regions_to_remove.begin();
       i != regions_to_remove.end(); ++i) {
    destroyRegion(*i);
  }
}


void Map::clearOldItemsOnGround() {
  unsigned long time = GetTickCount();
  while (!clear_items_on_location_queue_.empty()) {
    const ClearItemsOnMapLocation& clear_struct = clear_items_on_location_queue_.top();
    if (time < clear_items_on_location_queue_.top().millisecond_timer_time) break;
    WorldRegion* region = getRegionFromPoint(clear_struct.x, clear_struct.y);

    if (region) {
      region->clearItemsOnMapLocation(&clear_struct);
    }

    clear_items_on_location_queue_.pop();
  }
  /*
  for (WorldRegions::iterator i = regions_.begin();
       i != regions_.end(); ++i) {
    WorldRegion* region = i->second;
    region->clearOldItemsOnGround();
  }*/
}


void Map::destroyRegion(WorldRegion* region) {
  {
    GlobalLifeformAIManager* lifeform_ai_manager =
        GlobalLifeformAIManager::singleton();
    lifeform_ai_manager->releaseWorldRegion(region);
  }
  {
    for (ActiveRegionSwarmSpawners::iterator i = active_region_swarm_spawners_.begin();
        i != active_region_swarm_spawners_.end(); ++i) {
      RegionSwarmSpawner* spawner = *i;
      if (spawner->deactivateIfRegionIs(region)) {
        active_region_swarm_spawners_.erase(i);
        break;
      }
    }
  }
  { // remove from the internal map
    regions_.erase(region->getRegionID());
  }
  {
    GlobalWorldManager* world_manager = GlobalWorldManager::singleton();
    world_manager->deallocateRegion(region);
  }
}

void Map::updateSpawn(double time) {
  GlobalLifeformAIManager* lifeform_ai_manager = GlobalLifeformAIManager::singleton();
  for (ActiveRegionSwarmSpawners::iterator i = active_region_swarm_spawners_.begin();
      i != active_region_swarm_spawners_.end(); ++i) {
    RegionSwarmSpawner* spawner = *i;
    spawner->update(lifeform_ai_manager);
  }
}


const MapLocationInfo* Map::getMapLocationInfo(int x, int y) const {
  CONFIRM(x >= 0 && y >=0 && x < width_ && y < height_) else {
    return NULL;
  }
  return &locations_[y*width_ + x];
}


const MapLocationSpecialFunction* Map::getSpecialFunction(int index) const {
  --index; // all the indices are offset by 1
  if (index < 0) return NULL; // this isn't an error
  CONFIRM(index < number_of_special_functions_) else return NULL;
  return &special_functions_[index];
}


bool Map::isValidTeleport(Actor* actor, float dest_x, float dest_y) const {
  const MapLocationInfo* location = getMapLocationInfo(dest_x, dest_y);
  if (!location) return false;
  return location->pass_by_moving == 1;
}



WorldRegion* Map::getRegionFromPoint(float x, float y) {
  Evidyon::World::RegionID id = Evidyon::World::regionIDFromPoint(x, y);
  WorldRegions::iterator region = regions_.find(id);
  if (region == regions_.end()) return NULL;
  return region->second;
}


WorldRegion* Map::getRegionFromPoint(int x, int y) {
  Evidyon::World::RegionID id = World::regionIDFromPoint(x,y);
  WorldRegions::iterator region = regions_.find(id);
  if (region == regions_.end()) return NULL;
  return region->second;
}

WorldRegion* Map::acquireRegionFromPoint(float x, float y) {
  short region_x, region_y;
  Evidyon::World::RegionID id = World::regionIDFromPoint(x, y, &region_x, &region_y);
  WorldRegions::iterator region = regions_.find(id);
  return region == regions_.end() ? createRegion(region_x, region_y) : region->second;
}

WorldRegion* Map::acquireRegionFromPoint(int x, int y) {
  short region_x, region_y;
  Evidyon::World::RegionID id = World::regionIDFromPoint(x, y, &region_x, &region_y);
  WorldRegions::iterator region = regions_.find(id);
  return region == regions_.end() ? createRegion(region_x, region_y) : region->second;
}

WorldRegion* Map::acquireBorderedRegionFromPoint(float x, float y) {
  short region_x, region_y;
  Evidyon::World::RegionID id = World::regionIDFromPoint(x, y, &region_x, &region_y);
  //WorldRegions::iterator region = regions_.find(id);
  //return region == regions_.end() ? createRegionAndNeighbors(region_x, region_y) : region->second;
  return acquireRegionAndNeighbors(id, region_x, region_y);
}


WorldRegion* Map::acquireBorderedRegionFromPoint(int x, int y) {
  short region_x, region_y;
  Evidyon::World::RegionID id = World::regionIDFromPoint(x, y, &region_x, &region_y);
  //WorldRegions::iterator region = regions_.find(id);
  //return region == regions_.end() ? createRegionAndNeighbors(region_x, region_y) : region->second;
  return acquireRegionAndNeighbors(id, region_x, region_y);
}


bool Map::updateRegion(float x, float y, const WorldRegion* current, WorldRegion** updated) {
  if (current && current->containsPoint(x, y)) return true;
  *updated = getRegionFromPoint(x, y);
  return false;
}


WorldRegion* Map::createRegion(short region_x, short region_y) {
  World::RegionID region_id = World::regionIDFromRegionCoordinates(region_x, region_y);

  // Find all of the neighbors of this region
  short region_x_m1 = region_x-1;
  short region_x_p1 = region_x+1;
  short region_y_m1 = region_y-1;
  short region_y_p1 = region_y+1;
  WorldRegion* neighbors[] = {
    getRegion(region_x_m1, region_y_m1),
    getRegion(region_x_m1, region_y),
    getRegion(region_x_m1, region_y_p1),
    getRegion(region_x, region_y_m1),
    getRegion(region_x, region_y_p1),
    getRegion(region_x_p1, region_y_m1),
    getRegion(region_x_p1, region_y),
    getRegion(region_x_p1, region_y_p1),
  };

  GlobalWorldManager* manager = GlobalWorldManager::singleton();
  WorldRegion* region = manager->allocateRegion(this,
                                                neighbors,
                                                region_x,
                                                region_y,
                                                region_id);
  CONFIRM(region != NULL) else return NULL;
  regions_.insert(WorldRegions::value_type(region_id, region));

  if (region_x >=0 && region_y >= 0 &&
      region_x < regions_wide_ && region_y < regions_high_)
  { // active the swarm spawner in this region
    const int region_index = region_x + region_y * regions_wide_;
    RegionSwarmSpawner* swarm_spawner = &region_swarm_spawners_[region_index];
    swarm_spawner->activate(region);
    active_region_swarm_spawners_.push_back(swarm_spawner);
  }

  {
    WorldRegionInhabitants::iterator lower = world_region_inhabitants_.lower_bound(region_id);
    WorldRegionInhabitants::iterator upper = world_region_inhabitants_.upper_bound(region_id);
    for (WorldRegionInhabitants::iterator i = lower; i != upper; ++i) {
      Inhabitant* inhabitant = i->second;
      if (inhabitant->actor.invalid()) { // create this actor
        GlobalLifeformAIManager::singleton()->acquire(inhabitant->x,
                                                      inhabitant->y,
                                                      this,
                                                      GlobalActorManager::singleton(),
                                                      inhabitant->lifeform_ai_type,
                                                      0, // level 0
                                                      0, // no treasure
                                                      false, // don't delay activity
                                                      &inhabitant->actor);
      }
    }
  }

  return region;
}


WorldRegion* Map::acquireRegionAndNeighbors(World::RegionID region_id,
                                            short region_x,
                                            short region_y) {
  WorldRegion* region = NULL;
  {
    WorldRegions::iterator region_iterator = regions_.find(region_id);
    region = region_iterator == regions_.end() ? createRegion(region_x, region_y) : region_iterator->second;
  }

  //World::RegionID region_id = World::regionIDFromRegionCoordinates(region_x, region_y);
  //WorldRegion* region = createRegion(region_x, region_y);

  // Add all of the neighbors
  short region_x_m1 = region_x-1;
  short region_x_p1 = region_x+1;
  short region_y_m1 = region_y-1;
  short region_y_p1 = region_y+1;
  short neighbor_coordinates[8][2] = {
    region_x_m1, region_y_m1,
    region_x_m1, region_y,
    region_x_m1, region_y_p1,
    region_x, region_y_m1,
    region_x, region_y_p1,
    region_x_p1, region_y_m1,
    region_x_p1, region_y,
    region_x_p1, region_y_p1,
  };

  for (int i = 0; i < 8; ++i) { // acquire all of this region's neighbors
    short neighbor_x = neighbor_coordinates[i][0];
    short neighbor_y = neighbor_coordinates[i][1];

    WorldRegion* neighbor_region = getRegion(neighbor_x, neighbor_y);
    if (NULL == neighbor_region) {
      neighbor_region = createRegion(neighbor_x, neighbor_y);
    }

    CONFIRM(neighbor_region != NULL);
  }


  // Return
  return region;
}


WorldRegion* Map::getRegion(short region_x, short region_y) {
  Evidyon::World::RegionID id = World::regionIDFromRegionCoordinates(region_x,region_y);
  WorldRegions::iterator region = regions_.find(id);
  if (region == regions_.end()) return NULL;
  return region->second;
  
}

void Map::addClearItemsOnLocation(bool important, int x, int y, ItemIDType summation) {
  ClearItemsOnMapLocation clear_struct = { x, y, GetTickCount() + (important ? 5*60*1000 : 1*60*1000), summation };
  clear_items_on_location_queue_.push(clear_struct);
}

}