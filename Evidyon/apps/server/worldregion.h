#ifndef __WORLDREGION_H__
#define __WORLDREGION_H__

#include "../shared/evidyon_world.h"
#include <set>
#include "../shared/networkdecl.h"
#include "../shared/actorid.h"
#include <map>
#include "worldregion_items.h"
#include <dcx/bitmask>

namespace Evidyon {

class Actor;
class Client;
class Map;
struct ClearItemsOnMapLocation;


class WorldRegion {
public:
  typedef std::set<Actor*> ActorSet;
  typedef std::set<Client*> ClientSet;
  typedef WorldRegion_Sublocation Sublocation;
  typedef WorldRegion_LocalItems LocalItems;

public:
  WorldRegion() {}
  ~WorldRegion() {}

  // Does a simple comparison to see if the given point is inside of
  // this region.
  inline bool containsPoint(float x, float y) const;

  // Determines if the given coordinate is exactly containsed by this region
  inline bool strictlyContainsPoint(int x, int y) const;

  // Determines whether or not this region completely contains the
  // given area, or whether it could possibly overlap another.  If
  // this returns 'true', a comparison with that area only has to
  // compare itself with entries inside of this region.
  // TODO: Right now this method isn't implemented, and always
  // returns false.
  inline bool uniquelyContainsArea(float x, float y, float radius_sq) const;

  inline void addReference();
  inline void removeReference();

  inline World::RegionID getRegionID() const;
  inline int getRegionX() const;
  inline int getRegionY() const;



  // Call this when an actor enters the world for the first time and is
  // located within this region
  void enterWorld(Actor* actor);
  void enterWorld(Actor* actor, WorldRegion** regions_entered, int* number_of_regions_entered);

  // Call this when a client enters the world for the first time and is
  // located within this region
  void enterWorld(Client* client);
  void enterWorld(Client* client, WorldRegion** regions_entered, int* number_of_regions_entered);

  // Call these when a structure that is currently located in this region
  // leaves the world completely
  void leaveWorld(Actor* actor);
  void leaveWorld(Actor* actor, WorldRegion** regions_left, int* number_of_regions_left);
  void leaveWorld(Client* client);

  // Finds out which regions change when an actor moves from one part of the
  // world to another, and updates the actor's membership in those regions.
  void moveToRegion(Actor* actor, WorldRegion* new_region,
          WorldRegion** regions_left, int* number_of_regions_left,
          WorldRegion** regions_entered, int* number_of_regions_entered);

  // Changes the region that a client is a member of.  Since clients are
  // attached to an actor, this should be called after the actor's moveToRegion
  // has found the appropriate region membership changes.
  void changeRegionTo(Client* client, WorldRegion* new_region,
                      WorldRegion** regions_left, int number_of_regions_left,
                      WorldRegion** regions_entered, int number_of_regions_entered);


  // This method will send packets to the client of the activities of actors
  // that are local to this region.  This method should be called for each
  // newly visible region when moving between regions.
  //void sendActivityOfLocalActorsToClient(Client* client);

  // Sends the full description of all nearby neighbors to the given client. 
  // This should be called on a client's local region when it enters the
  // world for the first time.
  //void sendFullDescriptionOfNearbyActorsToClient(Client* client);

  void addNearbyActorsToSlowSync(Client* client, Actor* client_actor);
  void tellNearbyActorFullDescriptions(ActorID* actor_ids, size_t number_of_actors, NetworkPeer* peer);

  // Called repeatedly to add items that have been dropped due to a monster
  // dying, a character dropping items, etc.  If the return value is false,
  // then the given coordinates are outside of this region or the item is
  // invalid.
  // Parameters:
  //  important - if this is set, the item should stay on the ground for the
  //              maximum amount of time
  bool addItemOnGround(bool important, int x, int y, Item* item);
  void broadcastUpdateItemsOnLocation(int x, int y);
  void sendLocalItemsOnGroundToClient(Client* client);

  // Adds the items at (x, y) to the client's inventory.
  void pickUpItems(int x, int y, Client* client);

  void clearOldItemsOnGround();

  void clearItemsOnMapLocation(const ClearItemsOnMapLocation* clear_struct);

public:

  // Returns a list of less than buffer_size actors in actors_found that are
  // members region, enemies of the enemies_of_actor and within
  // sqrt(radius_squared) of (x,y).
  int findActorEnemiesNearby(Actor* enemies_of_actor,
                             float x,
                             float y,
                             float radius_squared,
                             Actor** actors_found,
                             int buffer_size);

  // Returns a list of less than buffer_size actors in actors_found of actors
  // that are within sqrt(radius_squared) of the actor's position
  int findActorsNearby(Actor* actor,
                       float radius_squared,
                       Actor** actors_found,
                       int buffer_size);

  // Gets the actor closest to the argument within the given radius
  bool findClosestActor(Actor* actor,
                        float search_radius_squared,
                        Actor** actor_found);

  // Searches the actor manager to see if a projectile has collided with any
  // Actor.  A projectile must be heading *toward* an Actor for it to be
  // considered colliding.  This way, launched projectiles won't collide
  // with the Actor spawning them.
  Actor* collideProjectileWithActor(float x, float y, float next_x, float next_y);
  //bool collideProjectileWithMap(float x, float y) const;
  inline Map* getMap();

  // TODO: add actor radii! this is definitely needed!!!
  // actors already within the radius of this actor are pushed away
  Actor* collideActorWithActor(Actor* actor, float x, float y, float next_x, float next_y);
  bool collideActorWithMap(Actor* actor, float next_x, float next_y);


public:

  // Sends a packet to all of the nearby clients
  void broadcastNetworkPacket(NetworkPacket* packet);

  // Same as broadcastNetworkPacket, but will release the packet if it has no
  // references left.
  void broadcastOrphanedNetworkPacket(NetworkPacket* packet);


public:
  //inline WorldRegion** getNeighbors();
  //inline int getNumberOfNeighbors();

  inline ActorSet::iterator localActorsBegin();
  inline ActorSet::iterator localActorsEnd();
  inline ActorSet::iterator nearbyActorsBegin();
  inline ActorSet::iterator nearbyActorsEnd();
  inline ClientSet::iterator localClientsBegin();
  inline ClientSet::iterator localClientsEnd();
  inline ClientSet::iterator nearbyClientsBegin();
  inline ClientSet::iterator nearbyClientsEnd();

  // Finds out how many clients are somewhere around this region.  This is
  // primarily used to find out when this region should be deallocated.
  // The number of local clients is necessairily less than or equal to
  // the returned value.
  inline size_t numberOfNearbyClients() const;

  // When this method returns false, it is safe to assume that anything in
  // this region is not visible to a client
  inline bool neighboringRegionsHaveNoNearbyClients() const;


friend class GlobalWorldManager;
protected:

  // Initializes this region.  Any non-NULL neighbor must be an unlinked
  // world region bordering this one.
  void acquire(Map* map,
               WorldRegion* neighbors[8],
               short region_x,
               short region_y,
               World::RegionID region_id);

  // Extracts this region from its neighbors.  Should only be called once
  // this region has no local entries; i.e. all actors, clients, etc. should
  // have removed themselves from this region.
  void release();


// Internal-only methods
private:

  // Does all of the initialization to link this region to a neighbor.  This
  // method will insert 'neighbor' in the internal neighbors list, and
  // increment reference counts and number_of_neighbors_ for both classes
  // appropriately.  Additionally, this method calls copyNearbyRegionLocalsFrom
  // to add entries to the local_* sets.
  void linkNewNeighbor(WorldRegion* neighbor);

  // Puts all of the "local" members of the source region into the "nearby"
  // members of this region.
  // This method is used when adding a new neighbor.
  void copyNearbyRegionLocalsFrom(WorldRegion* source);

  // Erases the given neighbor from this region's neighbor list.  The return
  // value is the number of references to this neighbor that were erased,
  // and should always be 1 if the program is working correctly.
  int removeNeighbor(WorldRegion* neighbor);

  //void addClearItemsOnLocationRequest(bool important, int x, int y, int local_x, int local_y);

private:
  Map* map_;  // the map that this region is a part of
  World::RegionID region_id_;
  float center_x_, center_y_; // World-coordinates of the region's center
  int region_x_, region_y_; // Region-coordinates of this region
  int references_;
  int left_, top_;

  WorldRegion* neighbors_[8];
  int number_of_neighbors_;

  ActorSet local_actors_;   // Actors inside of this region
  ActorSet nearby_actors_;  // Actors in this region or neighbors
  ClientSet local_clients_;   // Clients in this region
  ClientSet nearby_clients_;  // Clients in this region or neighbors

  // This set of items maps subregion coordinate offsets
  LocalItems local_items_;

  // When the update time is higher than this value, erase the coordinates
  // corresponding to bits set in locations_to_clear_[0], then copy down all
  // of the other masks and clear the last mask, then advance this value.
  static const int NUMBER_OF_CLEAR_SETS = 7;
  typedef dcx::dcxBitMask<World::REGION_SIZE*World::REGION_SIZE> LocationMask;
  LocationMask locations_with_items_to_clear_[NUMBER_OF_CLEAR_SETS];
};


#include "worldregion_inline.h"


#endif