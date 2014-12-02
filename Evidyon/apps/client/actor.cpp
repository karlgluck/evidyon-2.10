#include "../shared/evidyon_network.h"
#include "actor.h"
#include <math.h>
#include "../shared/client-editor/client_gamefile.h"
#include "specialfxbinding.h"
#include "../common/null.h"

#include "../shared/actordescriptiontemplate.h"

#include <dc/debug>

#include "sceneryrenderer.h"



namespace Evidyon {

Actor::Actor(ActorID id, const ClientActorDescriptionTemplate* description_template) {
  id_ = id;
  memset(&enchantment_bindings_, 0, sizeof(enchantment_bindings_));
  memset(equipped_items_, 0, sizeof(equipped_items_));
  reset(0.0f,0.0f);

  { // update the description
    setName(description_template->name);
    description_template_ = description_template;
  }
}


void Actor::reset(float x, float y) {
  name_ = "";
  description_template_ = NULL;
  cache_state_ = RESET;
  corporeal_state_ = NORMAL;
  memset(&current_, 0, sizeof(current_));
  current_.x = x;
  current_.y = y;
  memset(&last_sync_, 0, sizeof(last_sync_));
  last_sync_.x = x;
  last_sync_.y = y;
  last_sync_.time_since_previous_sync = 1.0f;
  actor_type_ = NULL;

  // Copy the current state over
  state_at_last_sync_ = current_;

  // Reset other values
  speech_expiration_time_ = 0.0;
  speech_text_.clear();
  release(NULL);
}



void Actor::release(SceneryRenderer* scenery_renderer) {
  mesh_controller_.destroy();

  for (int i = 0; i < Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES; ++i) {
    SpecialFXBinding* binding = enchantment_bindings_[i];
    if (binding == NULL) continue;
    binding->setExpired();
    binding->removeReference();
    enchantment_bindings_[i] = NULL;
  }

  if (scenery_renderer != NULL) {
    for (int i = 0; i < ACTORATTACHMENTPOINT_COUNT; ++i) {
      scenery_renderer->removeScenery(equipped_items_[i].scenery);
    }
  }

  cache_state_ = RESET;
}



void Actor::setPosition(float x, float y) {
  last_sync_.x = x;
  last_sync_.y = y;
  current_.x = x;
  current_.y = y;
  current_.speed = 0.0f;
  last_sync_.speed = 0.0f;
}



void Actor::sync(float time_since_previous_sync,
                 double time,
                 float x,
                 float y,
                 Evidyon::Network::Packet::ActorSyncData::Flags flags) {

  float dx = x-last_sync_.x;
  float dy = y-last_sync_.y;
  last_sync_.direction = atan2f(dy,dx);
  last_sync_.speed = flags.moving ? (sqrtf(dx*dx+dy*dy) / time_since_previous_sync) : 0.0f;

  last_sync_.x = x;
  last_sync_.y = y;
  last_sync_.time_received = time;
  last_sync_.time_since_previous_sync = time_since_previous_sync;
  last_sync_.moving = flags.moving;
  last_sync_.facing_angle = flags.angle / ((float)(1<<ACTORSYNC_ANGLE_BITS)) * (2*3.1415);

  {
    bool in_combat = flags.mesh_state == (unsigned)IN_COMBAT;
    last_sync_.in_combat = in_combat;
    if (in_combat) {
      corporeal_state_ = NORMAL;
    } else {
      CorporealState current_corporeal_state = corporeal_state_;
      corporeal_state_ = (CorporealState)flags.mesh_state;

      // make sure to animate death even if the action isn't specified
      if (corporeal_state_ == DEAD &&
          current_corporeal_state != DEAD) {
        flags.action = ACTORACTION_DIE;
      }
    }
  }

  if (flags.teleport) { // update the actor's position immediately
    current_.x = x;
    current_.y = y;
    current_.speed = 0.0f; 
    last_sync_.speed = 0.0f;
  }

  switch ((ActorAction)flags.action) {
    case ACTORACTION_NONE: break;

    case ACTORACTION_DIE: {
      corporeal_state_ = DEAD;
      animateDeath();
    } break;

    case ACTORACTION_GOTHIT: {
      if (!actor_type_ || corporeal_state_ != NORMAL) break;
      const float speed = 1.0f; // needs speed
      mesh_controller_.runAnimationOnce(
          const_cast<UnseenSkeletalAnimation*>(actor_type_->animations[ACTORANIMATION_GOTHIT].animation),
          speed * actor_type_->animations[ACTORANIMATION_GOTHIT].speed_factor +
                  actor_type_->animations[ACTORANIMATION_GOTHIT].speed_offset);
    } break;

    case ACTORACTION_CASTSPELL1:
    case ACTORACTION_CASTSPELL2: {
      if (!actor_type_ || corporeal_state_ != NORMAL) break;
      const int action_animation = ((int)ACTORANIMATION_ATTACK1) + flags.action;
      mesh_controller_.changeBasePose(
          const_cast<UnseenSkeletalAnimation*>(actor_type_->animations[action_animation].animation),
          1.0f);  // 100% of animation
    }

    case ACTORACTION_ATTACK1:
    case ACTORACTION_ATTACK2:
    case ACTORACTION_ATTACK3:
    case ACTORACTION_ATTACK4:
    case ACTORACTION_BLOCK1:
    case ACTORACTION_BLOCK2:
    case ACTORACTION_BLOCK3: {
      if (!actor_type_ || corporeal_state_ != NORMAL) break;
      const int action_animation = ((int)ACTORANIMATION_ATTACK1) + flags.action;
      const float speed = description_template_->action_speeds[flags.action]; // needs speed
      const ActorTypeDescription::Animation* animation = &actor_type_->animations[action_animation];
      mesh_controller_.runAnimationOnce(const_cast<UnseenSkeletalAnimation*>(animation->animation),
                                        speed * animation->speed_factor + animation->speed_offset);
    } break;
  }

  state_at_last_sync_ = current_;
}


void Actor::updateInvisible(double time) {
  double time_since_last_sync = time - last_sync_.time_received;
  float distance_since_last_sync = (time_since_last_sync) * last_sync_.speed;
  float sync_x = last_sync_.x + cosf(last_sync_.direction) * distance_since_last_sync;
  float sync_y = last_sync_.y + sinf(last_sync_.direction) * distance_since_last_sync;
  current_.x = sync_x;
  current_.y = sync_y;
  current_.speed = 0.0f;
  current_.rendering_angle = last_sync_.facing_angle;
  current_.angle = last_sync_.facing_angle;

  // Update text
  if (time > speech_expiration_time_) {
    speech_text_.clear();
  }
}

void Actor::update(SceneryRenderer* scenery_renderer,
                   double time,
                   double time_since_last_update) {
  double time_since_last_sync = time - last_sync_.time_received; // don't use predicted time--too much jitter!
  float distance_since_last_sync = (time_since_last_sync) * last_sync_.speed;
  float sync_x = last_sync_.x + cosf(last_sync_.direction) * distance_since_last_sync;
  float sync_y = last_sync_.y + sinf(last_sync_.direction) * distance_since_last_sync;
  float dx = sync_x - current_.x;
  float dy = sync_y - current_.y;
  float dd = sqrtf(dx*dx+dy*dy);

  static const float DISTANCE_THREASHOLD_STOP = 0.15f;
  static const float DISTANCE_THREASHOLD_TELEPORT = 1.0f;
  static const float TIME_THREASHOLD = 1.0f;
  bool stop_moving = last_sync_.moving == false && (dd < DISTANCE_THREASHOLD_STOP || time_since_last_sync > TIME_THREASHOLD);

  if (stop_moving) {
    current_.speed = 0.0f;
  } else {
    // is reaching the current location going to take less time than
    // a single sync?  if so, slow down; othwerise, go at the fastest speed
    if (dd/current_.speed < last_sync_.time_since_previous_sync) {
      current_.speed = dd/last_sync_.time_since_previous_sync;
    } else {
      current_.speed = stop_moving ? 0.0f : max(last_sync_.speed, current_.speed);
    }
  }

  current_.angle = atan2f(dy,dx);

  // Update the actor's animation
  if (last_sync_.moving) {
    animateWalking();
  } else {
    animateIdle();
  }

  if (dd < DISTANCE_THREASHOLD_TELEPORT) { // move the actor
    float distance = current_.speed * time_since_last_update;
    distance = min(dd, distance);
    //distance = (1.0f + (dd - distance) / 5) * distance; // modulate distance by difference with dd
    current_.x = current_.x + distance * cosf(current_.angle);
    current_.y = current_.y + distance * sinf(current_.angle);


  } else { // jump to the location since we've gotten out of sync
    float distance = current_.speed * 1.1f * time_since_last_update;
    distance = min(dd, distance);
    current_.x = current_.x + distance * cosf(current_.angle);
    current_.y = current_.y + distance * sinf(current_.angle);
    //current_.x = sync_x;
    //current_.y = sync_y;
    //DEBUG_INFO("out of sync!");
  }

  // Don't rotate if the actor is dead
  // Also, don't rotate if the actor is stopping: this prevents flipping direction on overshoot
  if (corporeal_state_ != DEAD && (state_at_last_sync_.speed == last_sync_.speed || last_sync_.moving)) {
    // Find the shortest distance between the last sync's rendering angle
    // and the direction in which the actor is moving
    float angle = last_sync_.moving ? current_.angle : last_sync_.facing_angle;
    float rendering_angle = state_at_last_sync_.rendering_angle;
    while (rendering_angle + D3DX_PI / 2 < angle) rendering_angle += 2 * D3DX_PI;
    while (rendering_angle - D3DX_PI / 2 > angle) rendering_angle -= 2 * D3DX_PI;

    //bool do_smooth_rotation =
    //  (fabsf(angle - rendering_angle) < D3DX_PI*2.0f/3.0f) &&
    //  (state_at_last_sync_.speed != 0.0f);
    bool do_smooth_rotation = false; // disable rotation smoothing for now

    if (do_smooth_rotation) {
      //double time_delta = 8.0f * (time - last_sync_.time_received);
      double time_delta = state_at_last_sync_.speed * 3.0f * (time - last_sync_.time_received);
      time_delta = min(1.0,time_delta);
      current_.rendering_angle = rendering_angle * (1.0 - time_delta) +
                    angle * time_delta;
    } else {
      current_.rendering_angle = angle;
    }
  }

  if (actor_type_) {  // animate the actor
    D3DXMATRIXA16 build, edit;
    D3DXMatrixRotationY(&edit, -current_.rendering_angle);
    D3DXMatrixMultiply(&build, &actor_type_->root_transform, &edit);
    D3DXMatrixTranslation(&edit, current_.x, 0.0f, current_.y);
    D3DXMatrixMultiply(&build, &build, &edit);

    // Update the actor's animation
    mesh_controller_.advance(&build,
                             time,
                             time_since_last_update);

    // Update the equipped items
    for (int i = 0; i < ACTORATTACHMENTPOINT_COUNT; ++i) {
      D3DXMATRIX transform;
      memcpy(&transform,
             getBoneMatrix(actor_type_->attachment_points[i].source_bone),
             sizeof(D3DXMATRIX));
      D3DXMatrixMultiply(&equipped_items_[i].transformation,
                         &actor_type_->attachment_points[i].offset_matrix,
                         &transform);
      scenery_renderer->updateSceneryTransform(equipped_items_[i].scenery,
                                              &equipped_items_[i].transformation);
    }
  }

  // Update text
  if (time > speech_expiration_time_) {
    speech_text_.clear();
  }
}



void Actor::setDescription(const ActorTypeDescription* actor_type) {

  cache_state_ = NOT_RENDERING;
  mesh_controller_.create(const_cast<GameFileDataSkinnedMesh*>(actor_type->mesh));
  actor_type_ = actor_type;

  animateIdle();

  current_.x = last_sync_.x;
  current_.y = last_sync_.y;
}



void Actor::setEnchantment(int class_index, SpecialFXBinding* enchantment_binding) {
  SpecialFXBinding* current_binding = enchantment_bindings_[class_index];
  if (current_binding != NULL) {
    current_binding->removeReference();
    current_binding->setExpired();
  }
  enchantment_bindings_[class_index] = enchantment_binding;
  if (enchantment_binding != NULL) {
    enchantment_binding->addReference();
  }
}


void Actor::setCorporealState(CorporealState state) {
  if (state == corporeal_state_) return;
  corporeal_state_ = state;
  switch (state) {
    case NORMAL:  break;
    case DEAD:  animateDeath(); break;
  }
}


GameFileDataSkinnedMesh* Actor::getSkinnedMesh() {
  return const_cast<GameFileDataSkinnedMesh*>(actor_type_->mesh);
}


float Actor::distanceToSq(Actor* actor) const {
  float dx = actor->current_.x - current_.x;
  float dy = actor->current_.y - current_.y;
  return dx*dx + dy*dy;
}


void Actor::animateIdle() {
  if (corporeal_state_ != NORMAL || !actor_type_) return;
  ActorAnimation idle_animation =
      last_sync_.in_combat ? ACTORANIMATION_COMBAT_IDLE : ACTORANIMATION_IDLE;
  mesh_controller_.changeBaseAnimation(
    const_cast<UnseenSkeletalAnimation*>(actor_type_->animations[idle_animation].animation),
    actor_type_->animations[idle_animation].speed_offset);
}


void Actor::animateWalking() {
  if (corporeal_state_ != NORMAL || !actor_type_) return;
  ActorAnimation moving_animation =
      last_sync_.in_combat ? ACTORANIMATION_RUN_ATTACK : ACTORANIMATION_RUN; // todo: run attack
  mesh_controller_.changeBaseAnimation(
    const_cast<UnseenSkeletalAnimation*>(actor_type_->animations[moving_animation].animation),
    actor_type_->animations[moving_animation].speed_factor * current_.speed +
    actor_type_->animations[moving_animation].speed_offset);
}


void Actor::animateDeath() {
  if (!actor_type_) return;
  int animation_index = (ActorAnimation)(ACTORANIMATION_DIE1 + rand()%4);
  mesh_controller_.changeBasePose(
      const_cast<UnseenSkeletalAnimation*>(actor_type_->animations[animation_index].animation),
      1.0f);  // 100% of animation
  mesh_controller_.runAnimationOnce(
      const_cast<UnseenSkeletalAnimation*>(actor_type_->animations[animation_index].animation),
      actor_type_->animations[animation_index].speed_factor * 1.0f +
      actor_type_->animations[animation_index].speed_offset);
}


}