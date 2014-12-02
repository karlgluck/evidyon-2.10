#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "animatedmeshcontroller.h"
#include "actortypedescription.h"
#include <string>
#include "../shared/client-server/actorsync.h"
#include "../shared/evidyon_actor.h"
#include "../shared/evidyon_enchantments.h"
#include "../shared/actorid.h"


#include "hscenery.h"


struct ClientActorTypeDescription;
class SceneryRenderer;




namespace Evidyon {
struct ClientActorDescriptionTemplate;


static const float SPEECH_EXPIRATION_DELAY = 8.0f;

// For binding enchantment effects
class SpecialFXBinding;


class Actor {

  struct EquipedItem {
    HSCENERY scenery;
    D3DXMATRIXA16 transformation;
  };

  struct MovementState {
    float x, y, angle, speed, rendering_angle;
  };

  enum CacheState {
    RESET,
    RENDERING,
    NOT_RENDERING,
  };
  void reset(float x, float y);

public:
  Actor(ActorID id,
        const ClientActorDescriptionTemplate* description_template);
  void release(SceneryRenderer* scenery_renderer);

  void setPosition(float x, float y);

  // Called several times per second when the client receives updates from the server
  void sync(float time_since_previous_sync,
            double time,
            float x,
            float y,
            Evidyon::Network::Packet::ActorSyncData::Flags flags);

  // Advances this actor's location and animation states.  The "invisible"
  // version is called when the actor is off the screen, and is significantly
  // faster.
  void update(SceneryRenderer* scenery_renderer, double time, double time_since_last_update);
  void updateInvisible(double time);

  void setDescription(const ActorTypeDescription* actor_type);
  const ClientActorDescriptionTemplate* getDescriptionTemplate() const;

  void setEnchantment(int class_index, SpecialFXBinding* enchantment_binding);
  void setCorporealState(CorporealState state);

  inline void setName(const char* name) { name_ = name; }
  inline const char* getName() { return name_.c_str(); }

  inline ActorID getID() { return id_; }
  inline float getX() { return current_.x; }
  inline float getY() { return current_.y; }
  inline float getAngle() { return current_.angle; }
  inline float getSpeed() { return current_.speed; }

  inline void setSpeechText(const char* text,
                            double current_time);

  inline void setRendering() { cache_state_ = RENDERING; }
  inline void setNotRendering() { cache_state_ = NOT_RENDERING; }
  inline bool isRendering() { return cache_state_ == RENDERING; }
  inline bool hasValidDescription() const { return cache_state_ != RESET && actor_type_ != NULL; }

  inline const D3DXMATRIX* getBoneMatrix(int bone);

  GameFileDataSkinnedMesh* getSkinnedMesh();
  inline AnimatedMeshController* getMeshController();
  inline const ActorTypeDescription* getActorTypeDescription();

  float distanceToSq(Actor* actor) const;

  inline const char* getSpeech() const;

  // If this actor has not been updated in more than some cutoff amount of time,
  // it can be assumed that it is no longer being updated by the server and
  // should be removed from the client's view of the world.
  inline const bool expired(double time) const;

  inline const D3DXMATRIX* getAttachmentPointMatrix(ActorAttachmentPoint point) const { return &equipped_items_[point].transformation; }
  inline HSCENERY getAttachmentPointScenery(ActorAttachmentPoint point) const { return equipped_items_[point].scenery; }
  inline void setAttachmentPointScenery(ActorAttachmentPoint point, HSCENERY scenery) { equipped_items_[point].scenery = scenery; }
  bool isDead() const { return corporeal_state_ == DEAD; }

  // added because global actor manager needs to know if the corporeal state
  // is SPIRIT_FORM so that this actor isn't rendered.
  inline CorporealState getCorporealState() { return corporeal_state_; }

private:
  void animateIdle();   // run idle animation
  void animateWalking();  // run walking animation
  void animateDeath();  // die, and end in dead pose

private:

  ActorID id_;
  const ClientActorDescriptionTemplate* description_template_;
  CacheState cache_state_;  // the actor's description state
  std::string name_;
  CorporealState corporeal_state_; // what the actor's body looks like

  // Current state
  MovementState current_;
  MovementState state_at_last_sync_;

  // Information from the most recent sync
  struct {
    double time_received;
    float x, y;
    float direction, speed;
    float time_since_previous_sync;
    bool moving;
    float facing_angle; // the angle that the actor is looking
    bool in_combat;
  } last_sync_;

  AnimatedMeshController mesh_controller_;  // Handles the animation of this mesh
  const ActorTypeDescription* actor_type_;  // How this actor looks

  // What this actor is saying, and when to remove the speech
  std::string speech_text_;
  double speech_expiration_time_;

  // Enchantments on this actor
  SpecialFXBinding* enchantment_bindings_[Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES];

  // Equipped items
  EquipedItem equipped_items_[ACTORATTACHMENTPOINT_COUNT];
};


inline void Actor::setSpeechText(const char* text, double current_time) {
  speech_text_ = text;
  speech_expiration_time_ = current_time + SPEECH_EXPIRATION_DELAY;
}

inline const bool Actor::expired(double time) const {
  static const double EXPIRATION_TIME = 1.0; // todo: put in shared constants file
  return hasValidDescription() && time - last_sync_.time_received > EXPIRATION_TIME;
}

inline const char* Actor::getSpeech() const {
  return speech_text_.empty() ? NULL : speech_text_.c_str();
}
inline AnimatedMeshController* Actor::getMeshController() {
  return &mesh_controller_;
}

inline const ActorTypeDescription* Actor::getActorTypeDescription() {
  return actor_type_;
}

inline const D3DXMATRIX* Actor::getBoneMatrix(int bone) {
  return mesh_controller_.getBoneMatrix(bone);
}

inline const ClientActorDescriptionTemplate* Actor::getDescriptionTemplate() const {
  return description_template_;
}



};



#endif