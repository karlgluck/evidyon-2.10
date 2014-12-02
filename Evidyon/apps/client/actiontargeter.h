#ifndef __ACTIONTARGETER_H__
#define __ACTIONTARGETER_H__

#include "../shared/actorid.h"
#include "useractionqueue.h"
#include "../common/gui/guimouse.h"
#include "../shared/evidyon_npc.h"


class IsometricCamera;

namespace Evidyon {

class GlobalActorManager;

struct Action_CastSpell {
  int spell_type;
  struct Parameters {
    bool needs_target;
    float x, y;
  };
};

struct Action_UseConsumable {
  int item_type;
  struct Parameters {
    int inventory_index_hint;
    bool needs_target;
    float x, y;
  };
};

struct Action_Move {
  struct Parameters {
    bool streaming;
    float x, y;
  };
};

struct Action_ActorInteraction {
  struct Parameters {
    ActorID actor_id;
  };
};

struct Action_Attack {
  struct Parameters {
    ActorID actor_id;
    float x, y;
  };
};

struct Action_NPCInteraction {
  struct Parameters {
    ActorID npc_actor_id;
    NPCInteractionType interaction_type;
  };
};

struct Action {
  enum Type {
    TYPE_INVALID,
    TYPE_USEREVENT,
    TYPE_SPELL,
    TYPE_CONSUMABLE,
    TYPE_MOVE,
    TYPE_ACTORINTERACTION, // when the mouse goes up, this is parsed into nothing/attack/npc types
    TYPE_ATTACK,
    TYPE_NPCINTERACTION,
  } type;
  union {
    // can be bound to keys
    UserEventType user_event;
    Action_CastSpell spell;
    Action_UseConsumable consumable;
  };
};

struct ActionInstance {
  Action action;
  union {
    UserEvent user_event;
    Action_CastSpell::Parameters spell;
    Action_UseConsumable::Parameters consumable;
    Action_Move::Parameters move;
    Action_ActorInteraction::Parameters actor_interaction;
    Action_Attack::Parameters attack;
    Action_NPCInteraction::Parameters npc_interaction;
  } params;
};


class ActionTargeter : public GUIMouseSentinel {
public:
  ActionTargeter();
  void create(IsometricCamera* camera, GlobalActorManager* global_actor_manager);

  virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
  virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);

  // Advances the targeter.  The returned value is a reference to the action
  // the user wishes to perform.
  Action::Type update(double time, const ActionInstance** action_instance);

  bool isTargeting() const;

  void userEvent(UserEventType user_event_type);
  UserEventAddAbilityPoint* attributesAddAbilityPoint();
  UserEventSpeak* speak();
  UserEventOnItem* dropItem();
  UserEventOnItem* equipItem();
  UserEventOnItem* unequipItem();
  UserEventOnItem* storageWithdrawItem();
  UserEventOnItem* storageDepositItem();
  UserEventOnItem* geosidSacrificeItem();

  void consumeItem(int inventory_index, bool needs_target);
  void castSpell(int type, bool needs_target);

  void npcInteraction(ActorID npc_actor_id, NPCInteractionType interaction_type);


private:
  // Called when the user wants to perform an action.  This either feeds straight
  // into the update() output, or blocks waiting for a target.
  void beginAction(const Action* action);

  // Gets rid of the current action, if it is valid
  void cancelAction();

  // If action_is_valid_, defines the action target with the current mouse world
  // coordinates and clears action_needs_target_
  void setActionTarget();

  // Used for pathfinding
  void buildMovementPath();
  void clearMovementPath();


private:
  double last_update_time_;

  // This value is returned via update().  It is filled out either by an explicit
  // command from the user, by movement, or by nothing.
  ActionInstance action_instance_;
  bool action_is_valid_;      // begin() has been called
  bool action_needs_target_;  // action_is_valid_ == true, and we're waiting for a target

  // Used to "stick" the mouse position and keep it from updating
  // for a short period when its first pressed
  double unstick_mouse_time_;
  bool mouse_stuck_;
  float mouse_world_x_, mouse_world_y_;
  GUIPoint last_mouse_position_;

  // movement is returned periodically when the primary mouse button is down
  bool return_movement_;
  double next_time_to_return_movement_;

  // if both the primary and secondary buttons are held, this list contains a stream
  // that takes the user to the location at which this condition first occurred
  //std::list<std::pair<float, float>> movement_path_;

  // Translates screen->world coordinates
  IsometricCamera* camera_;

  // Finds actors that the player clicked on to provide interaction
  GlobalActorManager* global_actor_manager_;
};


}


#endif