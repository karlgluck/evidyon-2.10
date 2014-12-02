#ifndef __ACTOR_INLINE_H__
#define __ACTOR_INLINE_H__

//#include "../shared/client-server/actorsync.h"

bool Actor::objectsCanMoveThrough() const {
  return corporeal_state_ == DEAD || corporeal_state_ == SPIRIT_FORM;
}

ActorController* Actor::getController() {
  return controller_;
}

bool Actor::block(Actor* attacker, const MeleeAttack* attack) {
    return controller_ ? controller_->block(attacker, attack) : false;
}

bool Actor::instantiateMagic(int source,
                             const Magic* magic,
                             int* times_to_generate,
                             double* motion_delay,
                             double* action_delay,
                             int* visualization_power_parameter,
                             MagicDescriptionStageParameters stage_parameters[MagicDescription::NUM_STAGES]) {
  if (controller_) {
  return controller_->instantiateMagic(source,
                                       magic,
                                       times_to_generate,
                                       motion_delay,
                                       action_delay,
                                       visualization_power_parameter,
                                       stage_parameters);
  } else {
    return false;
  }
}

void Actor::setCorporealState(CorporealState state) { update_mask_.state = state != corporeal_state_; corporeal_state_ = state; }
CorporealState Actor::getCorporealState() const { return corporeal_state_; }
void Actor::setPosition(float x, float y) { update_mask_.position = 1; x_ = x; y_ = y; }
void Actor::setDirection(float direction) { update_mask_.direction = 1; update_mask_.dxdy = 1; direction_ = direction; }
void Actor::setSpeed(float speed) { update_mask_.speed = 1; update_mask_.dxdy = 1; moving_ = true; speed_ = speed; }
void Actor::stopMoving() { update_mask_.speed = 1; update_mask_.dxdy = 1; moving_ = false; speed_ = 0.0f; }
void Actor::teleportTo(float x, float y) {
  update_mask_.position = 1;
  x_ = x;
  y_ = y;
  for (int i = 0; i < Evidyon::ClientConfig::SYNC_GROUPS+1; ++i) {
    sync_flags_[i].teleport = 1;
  }
}

inline void Actor::setCombatState(bool enable) {
  if (enable) {
    if (corporeal_state_ == NORMAL) corporeal_state_ = IN_COMBAT;
  } else {
    if (corporeal_state_ == IN_COMBAT) corporeal_state_ = NORMAL;
  }
}

inline void Actor::setMemberOfFaction(Faction faction) {
  faction_ = faction;
}

inline void Actor::fleshHit() {
  for (int i = 0; i < Evidyon::ClientConfig::SYNC_GROUPS+1; ++i) {
    sync_flags_[i].blood = 1;
  }
}

inline void Actor::setAction(ActorAction action) {
  for (int i = 0; i < Evidyon::ClientConfig::SYNC_GROUPS+1; ++i) {
    sync_flags_[i].action = (unsigned)action;
  }
}

inline void Actor::changeMap(Map* map, float x, float y) {
  map_ = map;
  update_mask_.position = 1;
  x_ = x;
  y_ = y;
  for (int i = 0; i < Evidyon::ClientConfig::SYNC_GROUPS+1; ++i) {
    sync_flags_[i].teleport = 1;
  }
  update_mask_.speed = 1; // stop moving
  update_mask_.dxdy = 1;
  moving_ = false;
  speed_ = 0.0f;
}

bool Actor::isAlive() const { return corporeal_state_ != DEAD; }
bool Actor::isDead() const { return corporeal_state_ == DEAD; }
bool Actor::isInvisible() const { return corporeal_state_ == SPIRIT_FORM; }

WorldRegion* Actor::getRegion() { return local_region_; }
ActorGroup* Actor::getGroup() { return group_; }
void Actor::getPosition(float* x, float* y) const { *x = x_; *y = y_; }
float Actor::getDirection() const { return direction_; }
void Actor::applyMagicEffect(Actor* source, double time, const Magic* magic,
               const MagicDescriptionStageEffect* effect,
               const MagicDescriptionStageEffect::Parameters* effect_parameters) {
  if (controller_) controller_->applyMagicEffect(source, time, magic, effect, effect_parameters);
}
float Actor::distanceToSq(const Actor* other) const {
  float dx = other->x_-x_, dy = other->y_-y_;
  return dx*dx+dy*dy;
}

float Actor::distanceToSq(float x, float y) const {
  float dx = x-x_, dy = y-y_;
  return dx*dx+dy*dy;
}
int Actor::getIDNumber() const { return sync_data_.id; }
bool Actor::isMoving() const { return moving_; }

Evidyon::Network::Packet::ActorSyncData* Actor::getSyncData() {
  return &sync_data_;
}

void Actor::setSyncGroup(int sync_group) {
  initSyncFlags(sync_group);
  sync_data_.flags = sync_flags_[sync_group];
  packPositionIntoSyncData();
  resetSyncFlags(sync_group);
}

bool Actor::withinViewOf(const Actor* client_actor) const {
  static const int SHIFT_SIGN = sizeof(int) * 8 - 1; // 31
  static const int VIEW_RANGE = 10 << 5;//<< Evidyon::Network::FLOAT_PACK_DECIMAL_BITS;
  // todo: subtract off for this actor's size
  {
    const int delta = sync_data_.x_packed - client_actor->sync_data_.x_packed;
    int abs_diff = delta - (((delta) & (delta >> SHIFT_SIGN)) << 1);
    // abs_diff -= actor_size_in_squares_;
    if (abs_diff > VIEW_RANGE) return false;
  }
  {
    const int delta = sync_data_.y_packed - client_actor->sync_data_.y_packed;
    int abs_diff = delta - (((delta) & (delta >> SHIFT_SIGN)) << 1);
    // abs_diff -= actor_size_in_squares_;
    if (abs_diff > VIEW_RANGE) return false;
  }

  return true;
}


Evidyon::Network::Packet::ActorSyncData::Flags Actor::getFastSyncFlags() {
  initSyncFlags(Evidyon::ClientConfig::SYNC_GROUPS);
  Evidyon::Network::Packet::ActorSyncData::Flags return_value =
    sync_flags_[Evidyon::ClientConfig::SYNC_GROUPS];
  resetSyncFlags(Evidyon::ClientConfig::SYNC_GROUPS);
  return return_value;
}

inline const char* Actor::getName() const { return  controller_ ? controller_->getName() : ""; }

inline bool Actor::inSameGroupAs(Actor* actor) const {
  return (group_ != 0) && (group_ == actor->getGroup());
}

inline float Actor::getSize() const {
  return size_;
}

inline void Actor::setSize(float size) {
  size_ = size;
}


inline float Actor::checkContactWith(float x, float y, float radius) {
  float rhs = radius + size_;
  return distanceToSq(x, y) - rhs*rhs;
}

inline float Actor::checkContactWith(const Actor* other_actor) {
  float rhs = (size_ + other_actor->size_);
  return distanceToSq(other_actor) - rhs*rhs;
}

inline float Actor::scaleWithinRangeOf(float x, float y, float radius, float range) {
  float rhs = radius + size_ + range;
  return distanceToSq(x, y) - rhs*rhs;
}

float Actor::separationDistanceTo(float x, float y, float radius) {
// the distance approximation is:
//  dx - x coordinate difference
//  dy - y coordinate difference
//  a, b - radii
//  t - the separation distance
// dx*dx + dy*dy = (a+b+t)*(a+b+t)
// dx*dx + dy*dy = (a+b)*(a+b) + 2*(a+b)*t + t*t
// (dx*dx + dy*dy - (a+b)*(a+b)) / (2*(a+b)) <=> t
// (dx*dx + dy*dy) / (a+b) - (a+b) / (2) <=> t
// t*t should be small relative to 2*(a+b)*t for best results

// the actual distance is:
// (-2*(a+b) +- sqrt(4*(a+b)*(a+b)-4*((a+b)*(a+b)-(dx*dx+dy*dy)))) / 2
// -(a+b) +- sqrt((a+b)*(a+b)-((a+b)*(a+b)-(dx*dx+dy*dy)))
// -(a+b) +- (a+b)*sqrt(1-(1-(dx*dx+dy*dy)/((a+b)*(a+b))))
// -(a+b) +- sqrt( dx*dx+dy*dy )
  float dd = distanceToSq(x, y) / 2.0f;
  float combined_radii = size_ + radius;
  return dd / combined_radii - combined_radii / 2.0f;
}

float Actor::separationDistanceTo(const Actor* other_actor) {
  float dd = distanceToSq(other_actor) / 2.0f;
  float combined_radii = size_ + other_actor->size_;
  return dd / combined_radii - combined_radii / 2.0f;
}


inline float Actor::scaleWithinRangeOf(const Actor* other_actor, float range) {
  // a = actor 1 radius
  // b = actor 2 radius
  // d = distance between actors
  // t = threashold distance
  // d - (a + b) < t
  // d < t + a + b
  // d*d < (t + a + b)^2
  // d*d = t*t + 2*t*(a+b) + (a+b)*(a+b)
  // 0 = t*t + 2*t*(a+b) * (a+b)*(a+b) - d*d
  // t1,t2 = (-2*(a+b) +- sqrt(4*(a+b)*(a+b) - 4*1*((a+b)*(a+b) - d*d)))/2
  // t1,t2 = (-2*(a+b) +- sqrt(4*(a+b)*(a+b)*(1 - 1) + 4*1*d*d))/2
  // t1,t2 = (-2*(a+b) +- sqrt(4*d*d))/2
  // t1,t2 = (-(a+b) +- d)
  // t1 = d - (a+b)
  float rhs = (range + size_ + other_actor->size_);
  return distanceToSq(other_actor) - rhs*rhs;
}


inline void Actor::initSyncFlags(int sync_group) {
  sync_flags_[sync_group].moving = moving_ ? 1 : 0;
  sync_flags_[sync_group].angle = (unsigned __int8)(direction_/(2*3.1415f)*(1<<Evidyon::ACTORSYNC_ANGLE_BITS));
  sync_flags_[sync_group].mesh_state = (unsigned)corporeal_state_;
}

inline void Actor::resetSyncFlags(int sync_group) {
  sync_flags_[sync_group].teleport = 0;
  sync_flags_[sync_group].action = ACTORACTION_NONE;
  sync_flags_[sync_group].blood = 0;
}


#endif