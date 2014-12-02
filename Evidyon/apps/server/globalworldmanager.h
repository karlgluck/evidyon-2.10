#ifndef __GLOBALWORLDMANAGER_H__
#define __GLOBALWORLDMANAGER_H__

#include "../shared/evidyon_world.h"
#include <dcx/memorypool>
#include <map>
#include "worldregion.h"
#include "../shared/evidyon_map.h"
#include "../shared/server-editor/maplocationspecialfunction.h"
#include "regionswarmspawner.h"
#include <vector>
#include <queue>
#include "itemidtype.h"

namespace Evidyon {


struct MapLocationInfo {
  MapNavigability navigability; // full navigability definition
  unsigned pass_by_moving : 1;
  unsigned pass_by_spirit : 1;

  // This allows the function to be accessed in linear time, if it exists.
  // A '0' in this location means no special function; any other value is
  // the function's actual index + 1 (but this is transparent to the user;
  // just pass this value to getSpecialFunction)
  unsigned special_function_index : 14; // teleport/key event/etc.
};

struct MapLocationSpecialFunction {
  MapLocationSpecialFunctionType type;

  union {
    struct MapLink {
      Map* target_map;
      float x, y;
    } map_link;
  };
};


class Item;


struct ClearItemsOnMapLocation {
  unsigned short x, y;
  unsigned int millisecond_timer_time;

  // these are used to check to make sure the location hasn't changed
  // since this clear was issued
  ItemIDType sum_of_unique_ids;

  bool operator < (const ClearItemsOnMapLocation& rhs) const {
    return millisecond_timer_time < rhs.millisecond_timer_time;
  }
};



class Map {
  typedef std::map<Evidyon::World::RegionID,WorldRegion*> WorldRegions;
  typedef std::vector<RegionSwarmSpawner*> ActiveRegionSwarmSpawners;

  struct Inhabitant {
    int x, y;
    size_t lifeform_ai_type;

    // If this inhabitant is active, the actor pointer will be valid.
    ActorPointer actor;
  };

public:
  Map();

  bool create(int id_number, int width, int height);
  bool createSpecialFunctions(int number_of_special_functions);
  void destroy();

  int getIDNumber() const;

  // Gets the allocated map information.  Used by the game file loader to
  // configure this map.
  MapLocationInfo* getMapInfo();

  // Creates a special function on the given map location.  This will set
  // the correct special function index for the location.
  // Used by the game file loader to configure this map.
  MapLocationSpecialFunction* setLocationSpecialFunction(int index,
                                                         int x,
                                                         int y);
  bool createRegionSwarmSpawner(int region_x,
                                int region_y,
                                unsigned int treasure_randomizer_mask,
                                const CompiledRegionSwarmSpawn* swarms,
                                size_t number_of_swarms,
                                const CompiledSwarmSpawnLocationInfo* locations);

  void createInhabitants(size_t number_of_inhabitants);
  void setInhabitant(size_t inhabitant_index, int x, int y, size_t lifeform_ai_type);

  // Looks through all of the map regions to see if any are completely out
  // of range of any client so that the region can be freed.
  void checkForUnusedMapRegions();

  // Gets rid of items on the map that have been there for a long time
  void clearOldItemsOnGround();

  // Updates the monster spawning
  void updateSpawn(double time);

  const MapLocationInfo* getMapLocationInfo(int x, int y) const;

  // Gets the special function at the given index.  This method should be
  // invoked by a managing structure (usually a Client) when it is on
  // a location with a valid specual function index.
  const MapLocationSpecialFunction* getSpecialFunction(int index) const;

  bool isValidTeleport(Actor* actor, float dest_x, float dest_y) const;


public:

  // Obtains a world region containing the given point (if it already exists).
  // This method will not create a new one.
  WorldRegion* getRegionFromPoint(float x, float y);
  WorldRegion* getRegionFromPoint(int x, int y);

  // Acquires a world region, but does not check to make sure that it has
  // valid borders.  This is a necessary distinction from a bordered region
  // used by computer-controlled characters, since acquiring a bordered
  // region instantiates more AI actors in the bordered areas, which in
  // turn would acquire borders further out until the whole map was
  // acquired.
  WorldRegion* acquireRegionFromPoint(float x, float y);
  WorldRegion* acquireRegionFromPoint(int x, int y);

  // Similar to getRegionFromPoint, but these methods will create a new region
  // if none already exists.  These methods ensure that the region acquired
  // is bordered on all sides by valid regions.
  WorldRegion* acquireBorderedRegionFromPoint(float x, float y);
  WorldRegion* acquireBorderedRegionFromPoint(int x, int y);

  // If the point is outside of the current region, this method calls
  // getRegionFromPoint to access the updated region and returns 'true'.
  // Note that this will not create a new region or update any memberships.
  // This is intended for use by projectiles.
  bool updateRegion(float x, float y, const WorldRegion* current,
                    WorldRegion** updated);

  // Determines how much of *distance an actor can walk in the direction
  // (dx,dy) starting at (x,y).  If the return value is 'true', then
  // *distance specifies the maximum distance an actor can walk in the
  // given direction.
  bool validatePhysicalPath(float x, float y,
    float dx, float dy, float* distance);

  // Adds a clear request for the given location.  It is passed back to the
  // world region a few minutes in the future (depending on whether or not
  // it is important) when the region should be cleared of items.  The
  // 'summation' parameter is a rollover sum of all the item IDs on the
  // map location.  This is checked by the WorldRegion to make sure that
  // nothing has been added to or removed from the location since this
  // request was placed.
  void addClearItemsOnLocation(bool important, int x, int y, ItemIDType summation);


private:

  // Creates a new world region
  WorldRegion* createRegion(short region_x, short region_y);

  // Finds or creates the world region at (region_x, region_y), ensures it has
  // neighbors on all sides then returns that region.
  WorldRegion* acquireRegionAndNeighbors(World::RegionID region_id, short region_x, short region_y);

  // Obtains a region pointer, if the region exists
  WorldRegion* getRegion(short region_x, short region_y);

  // Erases the given region from the manager.  Performs all cleanup such as
  // removing AI lifeforms, freeing the spawn points, etc.
  void destroyRegion(WorldRegion* region);


private:
  WorldRegions regions_;  // Memory is pulled from the global world manager's pool
  MapLocationInfo* locations_; // Data about each map location
  int id_number_; // Used to identify this map on the client
  int width_, height_; // todo: make square and just use size_

  MapLocationSpecialFunction* special_functions_;
  int number_of_special_functions_;

  // these structures control the creation of the mass mobs of monsters that
  // appear on the map for players to interact with.  A region swarm spawner
  // is activated when its associated WorldRegion is acquired.
  int regions_wide_, regions_high_;
  RegionSwarmSpawner* region_swarm_spawners_;
  ActiveRegionSwarmSpawners active_region_swarm_spawners_;

  // The inhabitants are permanent members of the world.  The controller for these
  // entries is created when the world region containing the spawn point is
  // activated and the associated actor does not exist.
  Inhabitant* inhabitants_;
  size_t number_of_inhabitants_;
  typedef std::multimap<World::RegionID,Inhabitant*> WorldRegionInhabitants;
  WorldRegionInhabitants world_region_inhabitants_;

  typedef std::priority_queue<ClearItemsOnMapLocation> ClearItemsOnLocationQueue;
  ClearItemsOnLocationQueue clear_items_on_location_queue_;
};



class GlobalWorldManager {
  static const int MAX_WORLD_REGIONS = 512;

  typedef dcx::dcxMemoryPool<WorldRegion,MAX_WORLD_REGIONS> WorldRegionPool;
  typedef std::map<Evidyon::World::RegionID,WorldRegion*> WorldRegions;

  friend class WorldRegion;


public:
  static GlobalWorldManager* singleton();


public:
  GlobalWorldManager();
  bool create(int number_of_maps);
  void destroy();

  // Obtains the map at the given index
  Map* getMap(int index);

  // Performs world-maintenance, updating things like erasing unused regions.
  void update(double time);


protected:
  friend class Map;
  WorldRegion* allocateRegion(Map* map,
                              WorldRegion* neighbors[8],
                              short region_x,
                              short region_y,
                              World::RegionID region_id);
  void deallocateRegion(WorldRegion* region);


private:
  WorldRegionPool region_memory_pool_;
  int allocated_regions_;

  //WorldRegions regions_;
  Map* maps_;
  int number_of_maps_;

  // The next time at which the regions with no clients nearby will be
  // erased and returned to the memory pool.
  double next_update_time_;

  // The next time at which to get rid of old items from the map
  double next_time_to_clear_old_items_on_ground_;


private:
  static GlobalWorldManager* singleton_;
};


}


#endif