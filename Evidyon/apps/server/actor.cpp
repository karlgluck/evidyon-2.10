#include "actor.h"
#include "actorgroup.h"
#include "../common/null.h"
#include "globalclientmanager.h"
#include "globalactormanager.h"
#include "worldregion.h"
#include "../shared/evidyon_actor.h"
#include "../shared/evidyon_enchantments.h"
#include "encodepackets.h"
#include "globalworldmanager.h"
#include "client.h"
#include "../shared/client-server/actordescription.h"
#include "../shared/evidyon_specialfx.h"
#include "../shared/evidyon_network.h"
#include "../shared/actordescriptiontemplate.h"

#include "./packets/tellactorfulldescriptions-encoder.h"

#include <dc/debug>

#include <math.h>

namespace Evidyon {


void Actor::acquire(const ServerActorDescriptionTemplate* description_template,
                    ActorController* controller,
                    int unique_id,
                    Map* map,
                    float x,
                    float y) {
  description_template_ = description_template;

  controller_ = controller;
  sync_data_.id = unique_id;
  corporeal_state_ = SPIRIT_FORM; // invisible by default
  faction_ = CHAOTIC;
  group_ = NULL;
  memset(&update_mask_, 0, sizeof(update_mask_));
  setCombatState(false);
  x_ = x;
  y_ = y;
  dx_ = 0.0f;
  dy_ = 0.0f;

  // we only have to set up flags that aren't configured in initSyncFlags
  for (int i = 0; i < Evidyon::ClientConfig::SYNC_GROUPS+1; ++i) {
    sync_flags_[i].action = ACTORACTION_NONE;
    sync_flags_[i].teleport = 1;
  }

  // should not be necessary, strictly speaking
  moving_ = false;
  speed_ = 0.0f;
  size_ = description_template->radial_size;

  local_region_ = NULL;
  map_ = map;
  //// add to the actor's local region
  //map_ = map;
  //WorldRegion* local_region = map->acquireRegionFromPoint(x, y);
  //local_region_ = local_region;
  //local_region->enterWorld(this);
}


void Actor::release() {
  setGroup(NULL); // make sure the actor isn't part of a group anymore
  if (local_region_ != NULL) {
    local_region_->leaveWorld(this);

    local_region_->removeReference();
    local_region_ = NULL;
  } else {
    DEBUG_INFO("Actor @0x%X released without region", this);
  }

  { // cleanup code: not strictly necessary, but could help identify bugs
    map_ = NULL;
    corporeal_state_ = INVALID;
    controller_ = NULL;
  }
}


bool Actor::setGroup(ActorGroup* group) {
  if (group_ == group) return true;

  if (group_ != NULL) {
    group_->removeMember(this);
  }

  if (group != NULL) {
    group_ = group;
    if (group->addMember(this)) {
      group_ = group;
      return true;
    } else {
      group_ = NULL;
      return false;
    }
  } else {
    group_ = NULL;
    return true;
  }
}


void Actor::getMapLocation(int* x, int* y) const {
  *x = sync_data_.x_packed >> Network::FLOAT_PACK_DECIMAL_BITS;
  *y = sync_data_.y_packed >> Network::FLOAT_PACK_DECIMAL_BITS;
}

void Actor::getStrictRegion(int* map_x, int* map_y, WorldRegion** region) {
  int x, y;
  getMapLocation(&x, &y);
  if (local_region_->strictlyContainsPoint(x, y)) {
    *map_x = x;
    *map_y = y;
    *region = local_region_;
  } else {
    *map_x = x;
    *map_y = y;
    *region = map_->acquireRegionFromPoint(x, y);
  }
}

void Actor::faceLocation(float x, float y) {
  float dx = x - x_;
  float dy = y - y_;
  float dist = sqrtf(dx*dx+dy*dy);
  static const float ATAN2_THREASHOLD = 0.01f;
  if (dist > ATAN2_THREASHOLD) { // make sure dist > 0
    dx_ = dx / dist;
    dy_ = dy / dist;
    direction_ = atan2f(dy, dx);
    update_mask_.dxdy = 1;
  }
}


float Actor::angleTo(float x, float y) const {
  float dx = x - x_;
  float dy = y - y_;
  float dist = dx*dx+dy*dy;
  if (dist > 0.01f) { // make sure dist > 0
    return atan2f(dy, dx);
  } else {
    return 0.0f;
  }
}

float Actor::angleTo(const Actor* other) const {
  float dx = other->x_ - x_;
  float dy = other->y_ - y_;
  float dist = dx*dx+dy*dy;
  if (dist > 0.01f) { // make sure dist > 0
    return atan2f(dy, dx);
  } else {
    return 0.0f;
  }
}

bool Actor::isGlancingCollision(const Actor* bounding_overlap_collision) const {
  // get the distance to this actor
  float collider_x = bounding_overlap_collision->x_;
  float collider_y = bounding_overlap_collision->y_;
  float collider_dx = collider_x - x_,
        collider_dy = collider_y - y_;
  float collider_dd = sqrtf(collider_dx*collider_dx +
                            collider_dy*collider_dy);
  collider_dx /= collider_dd; // normalize
  collider_dy /= collider_dd;

  float dx = dx_, dy = dy_;
  float dd = sqrtf(dx*dx+dy*dy);
  float dot_x = dx / dd * collider_dx;
  float dot_y = dy / dd * collider_dy;

  // is the magnitude of the DP small? if so, ignore the collision
  static const float GLANCING_COLLISION_THREASHOLD = 0.4f;
  float collision_dp_sq = dot_x * dot_x + dot_y * dot_y;
  return collision_dp_sq < GLANCING_COLLISION_THREASHOLD;
}


void Actor::getPositionDelta(double delta_time, float* dx, float* dy) {
  if (moving_) {
    if (update_mask_.dxdy) {
      dx_ = speed_ * cosf(direction_);
      dy_ = speed_ * sinf(direction_);
      update_mask_.dxdy = 0;
    }
    *dx = dx_ * delta_time;
    *dy = dy_ * delta_time;
  } else {
    *dx = 0.0f;
    *dy = 0.0f;
  }
}


bool Actor::update(float x,
                   float y,
                   WorldRegion** left,
                   int* number_left,
                   WorldRegion** entered,
                   int* number_entered) {

  bool return_value = false;

  // Update the region that this actor is in
  WorldRegion* current_region = local_region_;
  Map* correct_map = map_;
  bool on_wrong_map = current_region == NULL || current_region->getMap() != correct_map;
  if (on_wrong_map) {
    //DEBUG_INFO("Actor on wrong map: going to %i @ %.02f,%.02f", correct_map->getIDNumber(), x, y);
    if (current_region != NULL) {
      current_region->leaveWorld(this, left, number_left);
    } else {
      *number_left = 0;
    }

    WorldRegion* new_region = getController()->isClient() ? correct_map->acquireBorderedRegionFromPoint(x,y) : correct_map->acquireRegionFromPoint(x, y);
    ASSERT(new_region) else return false;
    new_region->enterWorld(this, entered, number_entered);

    local_region_ = new_region;
    return_value = true;
    current_region = new_region;

  } else if (!current_region->containsPoint(x,y)) {
    WorldRegion* new_region = getController()->isClient() ? correct_map->acquireBorderedRegionFromPoint(x,y) : correct_map->acquireRegionFromPoint(x, y);
    current_region->moveToRegion(this, new_region, left, number_left, entered, number_entered);

    local_region_ = new_region;
    return_value = true;
    current_region = new_region;
  }

  if (update_mask_.state) {
    update_mask_.state = 0;
  }


  x_ = x;
  y_ = y;

  return return_value;
}


bool Actor::isEnemyOf(Actor* actor) const {
  static const unsigned int enemy_mask[] = {
    1<<EVIL | 1<<CHAOTIC | 1<<EVIL_NPC,// GOOD
    1<<CHAOTIC,// NEUTRAL
    1<<GOOD | 1<<CHAOTIC | 1<<GOOD_NPC,// EVIL
    1<<EVIL | 1<<EVIL_NPC, // GOOD_NPC
    1<<CHAOTIC,// NEUTRAL_NPC
    1<<GOOD | 1<<CHAOTIC | 1<<GOOD_NPC,// EVIL_NPC
    1<<CHAOTIC|1<<GOOD|1<<NEUTRAL|1<<EVIL|1<<GOOD_NPC|1<<NEUTRAL_NPC|1<<EVIL_NPC,// CHAOTIC
  };
  return ((enemy_mask[actor->faction_] >> faction_) & 1) == 0x1;
}


void Actor::broadcastMagicFX(int magic_fx_type,
                             Actor* src_actor,
                             float src_x,
                             float src_y,
                             int power,
                             Actor** targets,
                             int num_targets) {
  CONFIRM(local_region_) else return;

  using namespace ::Evidyon::Network::Packet::Server::Encode;
  int target_ids[Evidyon::MAX_SPECIALFX_TARGETS];
  for (int i = 0; i < num_targets && i < Evidyon::MAX_SPECIALFX_TARGETS; ++i) {
    if (!targets[i]) continue;
    target_ids[i] = targets[i]->getIDNumber();
  }

  int source_id = src_actor == NULL ? 0 : src_actor->getIDNumber();

  NetworkPacket* packet = createMagicFX(magic_fx_type,
                                        source_id,
                                        src_x,
                                        src_y,
                                        power,
                                        target_ids,
                                        num_targets);
  local_region_->broadcastOrphanedNetworkPacket(packet);
}



int Actor::findNearbyActorsInGroup(float radius_squared,
                   Actor** actors_found, int buffer_size) {
  if (group_ == NULL) {
    actors_found[0] = this;
    return 1;
  } else {
    return group_->findMembersNear(this, radius_squared, actors_found, buffer_size);
  }
}


int Actor::findNearbyEnemies(float radius_squared, Actor** actors_found,
               int buffer_size) {
  return findEnemiesNear(x_, y_, radius_squared, actors_found, buffer_size);
}


int Actor::findEnemiesNear(float x, float y, float radius_squared,
               Actor** actors_found, int buffer_size) {
  return group_ == NULL ?
    local_region_->findActorEnemiesNearby(this,
                                          x,
                                          y,
                                          radius_squared,
                                          actors_found,
                                          buffer_size) :
    group_->findEnemiesNear(this,
                            x,
                            y,
                            radius_squared,
                            actors_found,
                            buffer_size);
}


int Actor::findEnemiesNear(Actor* actor, float radius_squared,
               Actor** actors_found, int buffer_size) {
  return findEnemiesNear(actor->x_, actor->y_, radius_squared, actors_found, buffer_size);
}

NetworkPacket* Actor::getDestroyActorPacket() const {
  return ::Evidyon::Network::Packet::Server::Encode::eraseActorFromWorld(sync_data_.id);
}

NetworkPacket* Actor::getDescriptionPacket() {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  ActorDescriptionData description;
  fillDescription(&description);
  return Encode::tellSingleActorFullDescription(description,
    controller_ ? controller_->getName() : "");
}

void Actor::fillDescription(::Evidyon::Network::Packet::ActorDescriptionData* description) {
  packPositionIntoSyncData();
  description->sync_ = sync_data_;
  if (controller_) controller_->fillStateDescription(&description->state_);
  description->state_.corporeal_state =
    static_cast<unsigned>(corporeal_state_);
}

void Actor::broadcastUpdateDescriptionPacket() {
  if (!local_region_) return;
  local_region_->broadcastOrphanedNetworkPacket(getDescriptionPacket());
}

Map* Actor::getMap() const {
  CONFIRM(local_region_) else return NULL;
  return local_region_->getMap();
}

// this method was extracted from the constructor on 3/18/2009 because it is
// necessary to delay an actor's entrance into the world when spawning
// swarm monsters until their spawning effect finishes.
void Actor::enterWorld() {
  CONFIRM(map_) else return;
  CONFIRM(!local_region_) else return;
  WorldRegion* local_region = map_->acquireRegionFromPoint(x_, y_);
  local_region_ = local_region;
  local_region->enterWorld(this);
}

void Actor::packPositionIntoSyncData() {
  // encode the actor's position into sync_data_
  float x = x_, y = y_;
  float fract = fmodf(x, 1.0f);
  sync_data_.x_packed = ((int)(floor(x - fract)) << Network::FLOAT_PACK_DECIMAL_BITS) | static_cast<int>(floorf(fract * (1<<Network::FLOAT_PACK_DECIMAL_BITS)));
  fract = fmodf(y, 1.0f);
  sync_data_.y_packed = ((int)(floor(y - fract)) << Network::FLOAT_PACK_DECIMAL_BITS) | static_cast<int>(floorf(fract * (1<<Network::FLOAT_PACK_DECIMAL_BITS)));
}

}