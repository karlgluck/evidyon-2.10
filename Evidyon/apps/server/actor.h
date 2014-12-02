#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "magiccontroller.h"
#include "magicgenerator.h"
#include "../shared/server-editor/magicdescription.h"
#include "actorcontroller.h"
#include "../shared/networkdecl.h"
#include "../shared/client-server/actorsync.h"
#include "clientconfig.h"
#include "../shared/evidyon_actor.h"

namespace Evidyon {
namespace Network {
namespace Packet {
struct ActorDescriptionData;
}
}




// add a "client owner" parameter and this structure could easily be adapted
// to allow enchanted/pet monsters!  this also streamlines giving normal
// characters exp from killing monsters, and redirects exp from pets to their
// owner.  if ai lifeforms search for enemies of their own faction, then to
// enchant an actor, all that would ahve to be done is give it a client owner
// and change its faction to be "good"!







class ActorGroup;
class WorldRegion;
class Map;
struct MapLocationInfo;

struct ServerActorDescriptionTemplate;

// Represents a living, interactive part of the world.  Monsters, players,
// NPCs, merchants, etc. all use this class to represent themselves.  This is
// the most generic form of those types.
// An actor is a "dumb" class that is entirely beholden to its controller. 
// It does nothing other than maintain its own state, acccept modifications
// to it and ease the reporting and tabulation of values.  Be careful!
class Actor {
public:
  enum Faction {
    GOOD,
    NEUTRAL,
    EVIL,
    GOOD_NPC,
    NEUTRAL_NPC,
    EVIL_NPC,
    CHAOTIC,
  };

private:
  struct UpdateMask {
    unsigned position : 1;
    unsigned state : 1;
    unsigned direction : 1;
    unsigned speed : 1;
    unsigned dxdy : 1;  // when direction or speed change, dx_ and dy_ need to be updated
    unsigned visual_type : 1;
    unsigned __void : 26;
  };

  // These methods are called by the actor manager only.  The actor will enter
  // the local region upon calling this method.
  friend class GlobalActorManager;
  void acquire(const ServerActorDescriptionTemplate* description_template,
               ActorController* controller,
               int unique_id,
               Map* map,
               float x,
               float y);

  // Remove from all partitions
  void release();

public:

  // Synchronizes the state of this actor.  This method sends appropriate
  // state-change packets to clients as necessary.  Should be called by
  // the actor's controller class.
  // The coordinate parameters passed here are distinct from those passed
  // into the setPosition method.  These represent the actor's new interpolated
  // position, moved forward at the current speed.  setPosition(), on the other
  // hand, indicates a change in this movement and will generate an actor update
  // packet.
  // The 'left' and 'entered' parameters indicate world regions that this
  // actor is no longer a part of and has now entered, respectively.  This
  // method will automatically update the GlobalActorManager with its
  // region membership, and it is up to the controller to update its own.
  // The 'new_location' stores information about the new map location that the
  // actor occupies.  It can have special properties (such as being a linked
  // location) that need to be handled by the controller.
  // Returns 'true' if the region changed
  bool update(float x, float y,
              WorldRegion** left, int* number_left,
              WorldRegion** entered, int* number_entered);

  // This method checks for pure enemy and doesn't analyze group membership
  bool isEnemyOf(Actor* actor) const;

  // Checks to see if the actor is in a valid state (alive, has enough MP or
  // the item has enough charges) to use the given magic, then applies the
  // actor's modifiers to the template and fills out the output parameters.
  // Input:
  //  source <  0: the actor is the source of the magic
  //  source >= 0: the inventory index of the item being used
  //  magic:  source parameters for the magic effect
  // Output:
  //  times_to_generate: how many times to create stage0 targeters
  //  motion_delay: amount of time to delay caster motion
  //  action_delay: amount of time to delay caster action (essential + ability-based)
  //  visualization_power_parameter: param. passed to all visualizations
  //  stage_parameters: filled with all of the data for each effect stage
  // Returns:
  //  'true' if the magic has been successfully instanced
  inline bool instantiateMagic(int source, const Magic* magic,
      int* times_to_generate, double* motion_delay, double* action_delay,
      int* visualization_power_parameter,
      MagicDescriptionStageParameters stage_parameters[MagicDescription::NUM_STAGES]);

  // The "power" parameter is an arbitrary number from 0-255 defined on a per-effect
  // basis that affects how much emphasis the client puts on this visualization
  void broadcastMagicFX(int magic_fx_type, Actor* src_actor, float src_x,
      float src_y, int power, Actor** targets, int num_targets);

  // Output:
  //  actor pointers in "actors_found"
  // Returns:
  //  The number of actors found, (<= to buffer_size)
  //int findAllNearbyActors(float radius_squared, Actor** actors_found, int buffer_size);
  //int findGroupMembersNear(float x, float y, float radius_squared, Actor** actors_found, int buffer_size);
  int findNearbyActorsInGroup(float radius_squared, Actor** actors_found, int buffer_size);
  int findNearbyEnemies(float radius_squared, Actor** actors_found, int buffer_size);
  int findEnemiesNear(float x, float y, float radius_squared, Actor** actors_found, int buffer_size);
  int findEnemiesNear(Actor* actor, float radius_squared, Actor** actors_found, int buffer_size);

  // Invoked by a magic targeter to apply the effect of the spell that the
  // targeter was controlling
  inline void applyMagicEffect(Actor* source,
                               double time,
                               const Magic* magic,
                               const MagicDescriptionStageEffect* effect,
                               const MagicDescriptionStageEffect::Parameters* effect_parameters);

  NetworkPacket* getDestroyActorPacket() const;

  // This method creates a packet containing the full description of just this actor.
  NetworkPacket* getDescriptionPacket();
  void fillDescription(::Evidyon::Network::Packet::ActorDescriptionData* description);

  // Sends out an updated description packet for this actor to its local region
  void broadcastUpdateDescriptionPacket();

  inline ActorController* getController();

  Map* getMap() const;

  // Finds a region and adds this actor to it if the current region is invalid.
  void enterWorld();

public:
  inline bool objectsCanMoveThrough() const; // returns true if the actor is dead, in spirit form, etc...
  inline bool isDead() const;
  inline bool isAlive() const;
  inline bool isInvisible() const;
  //inline CorporealState getCorporealState() const; // should be unnecessary
  inline WorldRegion* getRegion();
  inline ActorGroup* getGroup();
  inline void getPosition(float* x, float* y) const;
  inline float getDirection() const;
  inline float distanceToSq(const Actor* other) const;
  inline float distanceToSq(float x, float y) const;
  inline int getIDNumber() const;
  inline bool isMoving() const;
  inline const char* getName() const;
  inline bool inSameGroupAs(Actor* actor) const;

  // Changes the ActorGroup that this actor is a member of.  If the actor
  // is a member of a group currently, its membership is revoked.  If
  // the new group is valid (non-NULL), it is added to that group.
  bool setGroup(ActorGroup* group);

  // Gets the integral part of this actor's position
  void getMapLocation(int* x, int* y) const;

  // An actor doesn't immediately move to the next region when it crosses the
  // region's boundary.  This is so that an actor doesn't flip back and forth
  // between regions when making small movements.  Sometimes, though, it is
  // necessary to know *exactly* which region the actor is in.
  // For example, use this method when dropping items.
  // The output values are provided for reference, since they have to be found
  // in any case.
  // All parameters must be valid.  Be sure to check *region to see if it is
  // NULL before using.
  void getStrictRegion(int* map_x, int* map_y, WorldRegion** region);

  inline float getSize() const;
  inline void setSize(float size);

  // if the return value is < 0, the target's bounding circle is within
  // 'range' units of this actor's bounding circle.  'range' == 0.0f for
  // the check contact versions.  The return value has only relative
  // meaning; greater values indicate further distances
  inline float checkContactWith(float x, float y, float radius);
  inline float checkContactWith(const Actor* other_actor);
  inline float scaleWithinRangeOf(float x, float y, float radius, float range);
  inline float scaleWithinRangeOf(const Actor* other_actor, float range);

  // Finds out how much distance separates this actor's bounding circle and the
  // bounding circle of the target location (or actor).  These methods use a fast
  // approximation that is guaranteed to always return correct relative
  // magnitudes, but only gives accurate distances when the locations are
  // separated by an amount less than their combined radii.
  // For large separations, the returned distance is further than the actual
  // distance.
  inline float separationDistanceTo(float x, float y, float radius);
  inline float separationDistanceTo(const Actor* other_actor);

public: // melee combat methods
  inline bool block(Actor* attacker, const MeleeAttack* attack);

public:
  inline void setSyncGroup(int sync_group); // call before getSyncData to get the flags for a given group
  inline Evidyon::Network::Packet::ActorSyncData* getSyncData();
  inline Evidyon::Network::Packet::ActorSyncData::Flags getFastSyncFlags();

  // called before adding to a client's sync packet.  very fast comparison
  // that has no branch statements and uses the packed integer coordinates
  // in the actors' sync-data
  inline bool withinViewOf(const Actor* client_actor) const;

public: // should only be used by the actor's controller
  inline void setCorporealState(CorporealState state);
  inline CorporealState getCorporealState() const;
  inline void setPosition(float x, float y);
  inline void teleportTo(float x, float y);
  inline void changeMap(Map* map, float x, float y);
  inline void setDirection(float direction);
  inline void setVisualType(size_t type);
  inline void setSpeed(float speed);  // sets a movement speed and enables moving
  inline void stopMoving(); // sets movement speed = 0 and stops moving
  inline void setAction(ActorAction action);
  inline void setCombatState(bool enable);
  inline void setMemberOfFaction(Faction faction);
  inline void fleshHit();

  void faceLocation(float x, float y);
  float angleTo(float x, float y) const;
  float angleTo(const Actor* other) const;

  // Returns 'true' if the strict bounding-circle-overlap collision should be
  // ignored because this actor is passing the other and not trying to move
  // through it.
  bool isGlancingCollision(const Actor* bounding_overlap_collision) const;

  // Gets the position change of the actor after a given amount of time
  void getPositionDelta(double delta_time, float* dx, float* dy);

protected:
  void reset(); // Called by owner when it acquires this structure

private:
  inline void initSyncFlags(int sync_group);
  inline void resetSyncFlags(int sync_group);

  // takes the current (x_, y_) floating point coordinates and packs them
  // into the fixed 16-bit coordinates in the sync packet.
  void packPositionIntoSyncData();

private:

  // The class that handles how this actor behaves, its stats, etc.
  ActorController* controller_;

  // A bit is set in this mask for each type of modification to the actor
  // so that an update message can be sent to clients informing them of
  // the actors' changed attributes.
  UpdateMask update_mask_;

  // Data that defines this actor's state when updates are sent to a client.
  // This structure holds important world information, such as the actor's
  // ID number and coordinates.
  Evidyon::Network::Packet::ActorSyncData sync_data_;

  // The group at index SYNC_GROUPS is the fastsync group.  It is only used
  // if this actor is fastsynced with a client.
  Evidyon::Network::Packet::ActorSyncData::Flags sync_flags_[Evidyon::ClientConfig::SYNC_GROUPS+1];

  // how large this actor is, radially
  float size_;

  // definition of this actor's visible properties
  const ServerActorDescriptionTemplate* description_template_;

  CorporealState corporeal_state_; // This actor's body's status
  float x_, y_; // the actor's location in the world; copied to sync_data_ on update()
  float direction_;
  float speed_; // How fast the actor is moving

  // the map that the actor should be a part of.  this can be out-of-sync with
  // the local region (i.e. map doesn't contain region) for at most 1 update.
  Map* map_;  // considered authoratative
  WorldRegion* local_region_; // The region that the actor is directly inside

  // Whether or not the actor is moving.  This is simply used as a convenience
  // state flag, since this is often used.
  bool moving_;

  // (x,y) component movement speeds.  These are sync'ed internally to make
  // per-frame movement require fewer trig functions.
  float dx_, dy_;

  // Classes of which this actor is a member
  Faction faction_;
  ActorGroup* group_;
};


#include "actor_inline.h"

}

#endif