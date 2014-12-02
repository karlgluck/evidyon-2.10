#include "actiontargeter.h"
#include "../common/isometriccamera.h"
#include "globalactormanager.h"

namespace Evidyon {

static const double MOUSE_STICK_TIME = 0.35;


ActionTargeter::ActionTargeter() {
  last_update_time_ = 0.0;
  unstick_mouse_time_ = 0.0;
  mouse_stuck_ = false;
  mouse_world_x_ = 0.0f;
  mouse_world_y_ = 0.0f;
  next_time_to_return_movement_ = 0.0;
  return_movement_ = false;
  camera_ = NULL;
  global_actor_manager_ = NULL;
  action_is_valid_ = false;
  action_needs_target_ = false;
}


void ActionTargeter::create(IsometricCamera* camera,
                            GlobalActorManager* global_actor_manager) {
  camera_ = camera;
  global_actor_manager_ = global_actor_manager;
}


bool ActionTargeter::onMouseMove(GUIPoint position,
                                 GUIPoint oldPosition,
                                 GUIMouseInputSource* mouse) {
  last_mouse_position_ = position;
  return true; // don't propagate movement
}


bool ActionTargeter::onMouseButtonAction(GUIMouseButtonAction action,
                                           GUIPoint position,
                                           GUIMouseInputSource* mouse) {
  switch (action) {
  case GUIMOUSE_DOWN: {
      return_movement_ = !action_is_valid_;
      next_time_to_return_movement_ = 0.0; // return immediately
      mouse_stuck_ = true;
      unstick_mouse_time_ = last_update_time_ + MOUSE_STICK_TIME;

      // force the mouse to update since the user clicked here
      camera_->getWorldLocation(last_mouse_position_.x,
                                last_mouse_position_.y,
                               &mouse_world_x_,
                               &mouse_world_y_);
    } break;
  case GUIMOUSE_CLICK_SINGLE: {
      setActionTarget();
    } break;
  case GUIMOUSE_UP: {
      return_movement_ = false;
      //mouse_stuck_ = false; // don't clear mouse_stuck_ here so that movement clicks work out correctly
      clearMovementPath();
    } break;
  case GUIMOUSE_ALT_DOWN: {
      cancelAction();
      if (return_movement_) {
        buildMovementPath();
      } else {
        // TODO: if we're not moving, do some sort of inspection (ex. show items on ground, char names, etc.)
      }
    } break;
  case GUIMOUSE_ALT_UP: {
      clearMovementPath();
    } break;
  }

  // block other interaction
  return true;
}

static const double MOVEMENT_STREAM_PERIOD = 0.3;


void ActionTargeter::cancelAction() {
  action_is_valid_ = false;
  action_instance_.action.type = Action::TYPE_INVALID;
}


void ActionTargeter::setActionTarget() {
  if (action_is_valid_ == false) return;
  action_needs_target_ = false;
  switch (action_instance_.action.type) {
  case Action::TYPE_MOVE: {
      action_instance_.params.move.x = mouse_world_x_;
      action_instance_.params.move.y = mouse_world_y_;
    } break;
  case Action::TYPE_CONSUMABLE: {
      action_instance_.params.consumable.x = mouse_world_x_;
      action_instance_.params.consumable.y = mouse_world_y_;
    } break;
  case Action::TYPE_SPELL: {
      action_instance_.params.spell.x = mouse_world_x_;
      action_instance_.params.spell.y = mouse_world_y_;
    } break;
  case Action::TYPE_ACTORINTERACTION: {
      Actor* actor = NULL;
      if (global_actor_manager_->findLivingNonclientActorNearLocation(
            mouse_world_x_, mouse_world_y_, &actor) &&  // find an actor...
          (actor->getID() == action_instance_.params.actor_interaction.actor_id)) { // ...that is the same one we started with
        bool close_to_target = global_actor_manager_->getClientActor()->distanceToSq(actor) < 1.0f;
        action_is_valid_ = false; // invalidate the action
        switch (actor->getDescriptionTemplate()->template_type) {
          case ACTORTEMPLATETYPE_PLAYER:
          case ACTORTEMPLATETYPE_MONSTER:
            action_is_valid_ = true;
            action_needs_target_ = false;  
            action_instance_.action.type = Action::TYPE_ATTACK;
            action_instance_.params.attack.actor_id = actor->getID();
            action_instance_.params.attack.x = mouse_world_x_;
            action_instance_.params.attack.y = mouse_world_y_;
            break;
          case ACTORTEMPLATETYPE_NPC_STORAGE: if (close_to_target) npcInteraction(actor->getID(), NPCINTERACTION_STORAGE); break;
          case ACTORTEMPLATETYPE_NPC_BAZAAR:  if (close_to_target) npcInteraction(actor->getID(), NPCINTERACTION_BAZAAR);  break;
          case ACTORTEMPLATETYPE_NPC_GEOSID:  if (close_to_target) npcInteraction(actor->getID(), NPCINTERACTION_GEOSID);  break;
          case ACTORTEMPLATETYPE_NPC_MERCHANT:   if (close_to_target) npcInteraction(actor->getID(), NPCINTERACTION_MERCHANT); break;
          case ACTORTEMPLATETYPE_NPC_QUESTGIVER: if (close_to_target) npcInteraction(actor->getID(), NPCINTERACTION_QUEST);    break;

          default:
            action_is_valid_ = false;
            action_instance_.action.type = Action::TYPE_INVALID;
            break;
        }
      } else {
        action_is_valid_ = false;
        action_instance_.action.type = Action::TYPE_INVALID;
      }

      if (!action_is_valid_) { // default click action = move
        action_is_valid_ = true;
        action_needs_target_ = false;
        action_instance_.action.type = Action::TYPE_MOVE;
        action_instance_.params.move.streaming = false;
        action_instance_.params.move.x = mouse_world_x_;
        action_instance_.params.move.y = mouse_world_y_;
      }
    } break;
  }
}

void ActionTargeter::buildMovementPath() {
}

void ActionTargeter::clearMovementPath() {
}

bool ActionTargeter::isTargeting() const {
  return action_is_valid_ && action_needs_target_;
}

void ActionTargeter::userEvent(UserEventType user_event_type) {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = user_event_type;
}

UserEventAddAbilityPoint* ActionTargeter::attributesAddAbilityPoint() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = ATTRIBUTES_ADD_ABILITY_POINT;
  return &action_instance_.params.user_event.data_.add_ability_point_;
}

UserEventSpeak* ActionTargeter::speak() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = SPEAK;
  return &action_instance_.params.user_event.data_.speak_;
}

UserEventOnItem* ActionTargeter::dropItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = DROP_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}


UserEventOnItem* ActionTargeter::equipItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = EQUIP_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

UserEventOnItem* ActionTargeter::unequipItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = UNEQUIP_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}


UserEventOnItem* ActionTargeter::storageWithdrawItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = STORAGE_WITHDRAW_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

UserEventOnItem* ActionTargeter::storageDepositItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = STORAGE_DEPOSIT_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

UserEventOnItem* ActionTargeter::geosidSacrificeItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = GEOSID_SACRIFICE_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

void ActionTargeter::consumeItem(int inventory_index, bool needs_target) {
  action_instance_.action.type = Action::TYPE_CONSUMABLE;
  action_instance_.action.consumable.item_type = -1; // unknown
  action_instance_.params.consumable.inventory_index_hint = inventory_index;
  action_needs_target_ = needs_target;
  action_is_valid_ = true;
}

void ActionTargeter::npcInteraction(ActorID npc_actor_id, NPCInteractionType interaction_type) {
  action_instance_.action.type = Action::TYPE_NPCINTERACTION;
  action_instance_.params.npc_interaction.npc_actor_id = npc_actor_id;
  action_instance_.params.npc_interaction.interaction_type = interaction_type;
  action_needs_target_ = false;
  action_is_valid_ = true;
}

void ActionTargeter::castSpell(int type, bool needs_target) {
  action_instance_.action.type = Action::TYPE_SPELL;
  action_instance_.action.spell.spell_type = type;
  action_needs_target_ = needs_target;
  action_is_valid_ = true;
}

Action::Type ActionTargeter::update(double time, const ActionInstance** action_instance) {
  last_update_time_ = time;
  if (time > unstick_mouse_time_) {
    mouse_stuck_ = false;
  }
  if (!mouse_stuck_) {
    camera_->getWorldLocation(last_mouse_position_.x,
                              last_mouse_position_.y,
                             &mouse_world_x_,
                             &mouse_world_y_);
  }

  if (action_is_valid_) {
    if (action_instance_.action.type == Action::TYPE_ACTORINTERACTION &&
        !mouse_stuck_) {
      action_is_valid_ = false;
      action_needs_target_ = false;
      return_movement_ = true;
      next_time_to_return_movement_ = 0.0;
    }
    if (action_needs_target_) return Action::TYPE_INVALID; // don't do anything yet
    action_is_valid_ = false;
  } else { // return a movement action
    bool clicked = true;
    if (return_movement_) {
      if (next_time_to_return_movement_ > time) return Action::TYPE_INVALID;
      next_time_to_return_movement_ = time + MOVEMENT_STREAM_PERIOD;
      if (mouse_stuck_) {
        Actor* actor = NULL;
        if (global_actor_manager_->findLivingNonclientActorNearLocation(mouse_world_x_, mouse_world_y_, &actor)) {
          next_time_to_return_movement_ = -1.0; // be sure to do this!
          return_movement_ = false;
          action_is_valid_ = true;
          action_needs_target_ = true;
          action_instance_.action.type = Action::TYPE_ACTORINTERACTION;
          action_instance_.params.actor_interaction.actor_id = actor->getID();
          return Action::TYPE_INVALID;
        } else {
          // wait until the mouse is unstuck to send the next message
          // in order to prevent "bouncing" (backtracking)
          next_time_to_return_movement_ = time + MOVEMENT_STREAM_PERIOD + MOUSE_STICK_TIME;
        }
      }

      clicked = false; // streaming-movement mode:  don't attack anything
      action_instance_.action.type = Action::TYPE_MOVE;
      action_instance_.params.move.x = mouse_world_x_;
      action_instance_.params.move.y = mouse_world_y_;
      action_instance_.params.move.streaming = true;

    } else {

      // check to see if the final movement message has been sent to the
      // server to terminate streaming movement
      if (next_time_to_return_movement_ <= 0.0) return Action::TYPE_INVALID;

      // If we get here, the mouse was JUST released after having been
      // streaming--so we should send a final movement message to cancel
      // out the streaming.
      next_time_to_return_movement_ = -1.0;
    }

    if (clicked) { // the mouse was clicked; set up a "go here" message
      action_instance_.action.type = Action::TYPE_MOVE;
      action_instance_.params.move.x = mouse_world_x_;
      action_instance_.params.move.y = mouse_world_y_;
      action_instance_.params.move.streaming = false;
    }
  }

  // A valid action is being returned, so define its type
  *action_instance = &action_instance_;
  return action_instance_.action.type;
}


}