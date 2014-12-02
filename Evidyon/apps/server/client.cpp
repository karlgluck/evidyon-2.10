#include "client.h"
#include "globalnetworkmanager.h"
#include "../common/enet/enet.h"
#include "../common/null.h"
#include "globaldatabasemanager.h"
#include "actor.h"
#include <dc/debug>
#include "item.h"
#include "../shared/client-server/packets.h"
#include "encodepackets.h"
#include "decodepackets.h"
#include "../shared/evidyon_version.h"
#include "../shared/evidyon_accounts.h"
#include "globalgamefilemanager.h"
#include "globalactormanager.h"
#include "../shared/server-editor/server_gamefile.h"
#include "globalitemmanager.h"
#include "globalworldmanager.h"
#include "globalclientmanager.h"
#include "time.h"
#include "globalmagicmanager.h"
#include "../shared/client-server/fastsync.h"
#include "../shared/evidyon_network.h"

#include "../shared/server-editor/magicdescription.h"
#include "../shared/client-server/actordescription.h"
#include "../shared/client-server/askactorfulldescriptions.h"
#include "./packets/tellactorfulldescriptions-encoder.h"

#include "meleeattack.h"
#include "meleeblock.h"

#include "actorgroup.h"

// used to access Evidyon::running so that the server can be disabled remotely
#include "main.h"


// atan2f, sqrtf
#include <math.h>

#include "statisticslogger.h"
#include "../shared/server-editor/evidyon_gameplay_design.h"

#include "uniqueavatarnamechecker.h"

/*

TODO:
  - change all coordinate info to use fixed-point?
  - fill out addRedFlag
  - insert addRedFlag whenever a client-fault error occurs
  - report red flags to server

*/





namespace Evidyon {

static const double STREAMING_MOVEMENT_PACKET_PERIOD = 1.0 / 3.0;
static const float MAX_VISIBLE_DISTANCE_SQ = 8.0f*8.0f*1.1f;

static const float TIME_DEAD_BEFORE_RESPAWN = 10.0;
static const float MAX_NPC_INTERACTION_DISTANCE_SQ = 2.0f; // this should be larger than the client's interaction distance

std::set<unsigned int> Client::active_accounts_;

static const int GEONITE_RECALL_COST = -5;

Client::Client() {
  zero();
}



void Client::acquire(NetworkPeer* peer) {
  zero();
  peer_ = peer;
  state_ = CONNECTED;
  peer->data = (void*)this;
}



void Client::release() {

  {
    // stuff goes here
    leaveWorld();
    logoutAccount();
  }

  zero();
}




void Client::update(double time, double time_since_last_update) {
  if (state_ != LOGGED_INTO_CHARACTER) return;
  Actor* actor = actor_.dereference();
  if (actor->isDead()) {
    actor->stopMoving();
    magic_generator_.stop();

    // No requests can be executed while dead
    client_request_mask_ = 0;

    if (time_to_stop_moving_ < time) { // respawn
      GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
      float x, y;
      gamefile_manager->getPlayerRespawnLocation(&x, &y);
      actor->changeMap(GlobalWorldManager::singleton()->getMap(0), x, y);
      actor->setAction(ACTORACTION_NONE);
      actor->setCorporealState(NORMAL);
      avatar_attributes_.hp = avatar_attributes_.current.max_hp;
    }
  }

  // Handle enchantments
  if (enchantment_controller_.update(time)) {
    buildCurrentAvatarAttributes(time);
  }

  float x, y;
  actor->getPosition(&x, &y);

  // Advance the combat state
  melee_combat_controller_.update(time);
  if (melee_combat_controller_.frozen()) {
    actor->stopMoving();
  }

  // Have the client's actor constantly follow the target
  if (melee_combat_controller_.pursuingTarget()) {
    setRequest(REQUEST_MOVE);
    Actor* target = melee_combat_controller_.getTarget();
    target->getPosition(&client_request_move_.x,
                        &client_request_move_.y);
  }

  // Process user requests
  unsigned int request_index = 0;
  unsigned int request_mask = actor->isDead() ? 0 : client_request_mask_;
  unsigned int unprocessed_request_mask = 0;
  while (request_mask != 0) {
    if ((request_mask & 1) == 0x1) {
      switch ((ClientRequest)request_index) {
        case REQUEST_MOVE: {
          bool cant_act_immediately = (magic_controller_.frozen() ||
                                       melee_combat_controller_.frozen());

          int attack_target_actor_id = client_request_move_.attack_target_actor_id;
          { // set the attack target
            GlobalActorManager* actor_manager = GlobalActorManager::singleton();
            ActorPointer attack_target;
            actor_manager->getPointer(attack_target_actor_id, &attack_target);
            if (attack_target.invalid()) {
              actor->setCombatState(false);
              if (cant_act_immediately) {
                melee_combat_controller_.attack(NULL);
              } else {
                melee_combat_controller_.stop();
              }
            } else {
              melee_combat_controller_.attack(&attack_target);
            }
          }

          if (cant_act_immediately) {
            break;
          }

          float dx = client_request_move_.x - x,
                dy = client_request_move_.y - y;
          float direction = atan2f(dy,dx);
          actor->setDirection(direction);
          if (magic_controller_.rooted()) {
            break;
          }

          float distance = sqrtf(dy*dy+dx*dx);

          float speed = avatar_attributes_.current.speed;
          actor->setSpeed(speed);

          // If this is a stream, allow the user to move past the coordinate
          // by some amount of time dependent on the stream generation period
          // and the variance in the network connection
          double time_to_travel_distance = distance / speed;
          if (client_request_move_.streaming) {
            //double delay = peer_->roundTripTime;
            //delay = max(delay,time_to_travel_distance);
            //double stream_adjustment = STREAMING_MOVEMENT_PACKET_PERIOD;
            time_to_stop_moving_ = time + 3.0; // 3-5-09: a "long time" involves little calculation
                         //stream_adjustment +
                         //delay;
          } else {
            time_to_stop_moving_ = time + time_to_travel_distance;
          }

        } break;

        case REQUEST_SPELL: {
          //magic_generator_.stop(); // ?
          if (magic_controller_.busy()) break;
          if (melee_combat_controller_.busy()) {
            melee_combat_controller_.stop();
          }
          Magic magic;
          GlobalActorManager* actor_manager = GlobalActorManager::singleton();
          actor_manager->getPointer(client_request_spell_.actor_id, &magic.target);

          magic.x = client_request_spell_.x;
          magic.y = client_request_spell_.y;
          GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
          const SpellDescription* spell = gamefile_manager->getSpell(client_request_spell_.spell_id);
          if (spell && spell->magic && validateSpellParameters(actor,
                                                               magic.x,
                                                               magic.y,
                                                               magic.target.dereference())) {
            int mp_cost_of_spell = spell->mp_cost;
            if (avatar_attributes_.mp >= mp_cost_of_spell) {
              magic.description = spell->magic;
              magic_controller_.cast(-mp_cost_of_spell, // define this spell as being cast using MP
                                     &magic);
            }
          }
        } break;

        case REQUEST_USE_ITEM: {
          if (magic_controller_.busy()) break;
          if (melee_combat_controller_.busy()) {
            melee_combat_controller_.stop();
          }
          Magic magic;
          GlobalActorManager* actor_manager = GlobalActorManager::singleton();
          actor_manager->getPointer(client_request_use_item_.actor_id, &magic.target);
          int inventory_index = client_request_use_item_.inventory_index;
          magic.x = client_request_spell_.x;
          magic.y = client_request_spell_.y;

          Item* item = inventory_[inventory_index].item;
          CONFIRM(item) else break;
          const ServerItemDescription* description = item->getDescription();
          CONFIRM(description) else break;
          GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
          const MagicDescription* magic_desc =
            gamefile_manager->getMagic(description->consumable_magic);
          if (magic_desc && validateSpellParameters(actor,
                                                     magic.x,
                                                     magic.y,
                                                     magic.target.dereference())) {
            magic.description = magic_desc;
            magic_controller_.cast(inventory_index + 1, // define this spell as being cast from an item
                                   &magic);
          }
        } break;
      }
    }

    // Move to the next entry
    request_mask >>= 1;
    ++request_index;
  }
  client_request_mask_ = unprocessed_request_mask;

  // This flag is used in various locations, so make a local copy
  bool actor_is_moving = actor->isMoving();

  // Update the actor's magic-casting state
  if (magic_controller_.update(time)) {
    if (magic_controller_.rooted() || magic_controller_.frozen()) {
      actor_is_moving = false;
      actor->stopMoving();
    }
  }
  magic_generator_.update(GlobalMagicManager::singleton(), time);



  // Apply motion
  float dx, dy;
  actor->getPositionDelta(time_since_last_update, &dx, &dy);
  float next_x = x + dx, next_y = y + dy;

  WorldRegion* current_region = actor->getRegion();
  const MapLocationInfo* next_location_info;
  CONFIRM(current_region) else return;
  {
    int next_map_x = (int)(next_x+0.5f),
        next_map_y = (int)(next_y+0.5f);
    Map* map = current_region->getMap();
    CONFIRM(map) else return;
    next_location_info = map->getMapLocationInfo(next_map_x, next_map_y);
  }


  // Stop when we reach the target
  if (actor_is_moving) {
    bool stop_moving = time_to_stop_moving_ < time;
    if (stop_moving) {
      int map_x, map_y;
      WorldRegion* strict_region = NULL;
      actor->getStrictRegion(&map_x, &map_y, &strict_region);
      ASSERT(strict_region) else {
        DEBUG_INFO("Failed finding strict region at %i, %i", map_x, map_y); // this shouldn't happen!
        strict_region = current_region;
      }
      strict_region->pickUpItems(map_x,
                                 map_y,
                                 this);
    }
    if (!stop_moving) {
      Actor* collision = current_region->collideActorWithActor(actor,
                                                               x,
                                                               y,
                                                               next_x,
                                                               next_y);
      bool collided = (collision != NULL);
      if (collided) { // check for a glancing collision and allow movement
        collided = !actor->isGlancingCollision(collision);
      }

      // if we didn't find an actor collision, check the location
      if (!collided && (next_location_info != current_location_info_)) {
        collided = next_location_info == NULL ||
                    !((state_ == SPIRIT_FORM) ?
                      next_location_info->pass_by_spirit :
                      next_location_info->pass_by_moving);
      }

      if (collided) {
        stop_moving = true;
      }
    }

    if (stop_moving) {
      actor->stopMoving();
      next_x = x;
      next_y = y;
    }
  }

  // Move to the location as long as we were moving.  If this was just a
  // test and the move test failed, this can cause collision tests to
  // get mucked up.
  if (!current_location_info_ || (actor->isMoving() && current_location_info_ != next_location_info)) {
    current_location_info_ = next_location_info;
  }

  // Advance the actor state
  WorldRegion* regions_left[9];
  WorldRegion* regions_entered[9];
  int number_of_regions_left=0, number_of_regions_entered=0;
  if (actor->update(next_x,
                    next_y,
                    regions_left,
                   &number_of_regions_left,
                    regions_entered,
                   &number_of_regions_entered)) {
    WorldRegion* new_region = actor->getRegion();
    current_region->changeRegionTo(this,
                                   new_region,
                                   regions_left,
                                   number_of_regions_left,
                                   regions_entered,
                                   number_of_regions_entered);

    Map* new_map = new_region->getMap();
    if (current_region->getMap() != new_map) {
      using namespace Evidyon::Network::Packet::Server;
      sendPacket(Encode::changeMap(new_map->getIDNumber()));
      //DEBUG_INFO("Client changing map:  %.02f,%.02f -> %i @ %.02f, %.02f", x, y, new_map->getIDNumber(), next_x, next_y);
      melee_combat_controller_.stop();
      magic_generator_.stop();
      actor->stopMoving();
      time_to_stop_moving_ = 0.0;  
      magic_controller_.freeze(time + 0.1); // stop the client from doing anything    
      current_location_info_ = new_map->getMapLocationInfo(next_x, next_y);
    } else {
      using namespace Evidyon::Network::Packet::Server;
      sendPacket(Encode::updateLocalWorldRegion(new_region->getRegionID()));
    }

    // tell the items in the new regions
    for (int i = 0; i < number_of_regions_entered; ++i) {
      regions_entered[i]->sendLocalItemsOnGroundToClient(this);
    }
  }
}


void Client::changeEXP(int amount) {
  Actor* actor = actor_.dereference();
  CONFIRM(actor) else return;
  { // share xp with group
    ActorGroup* group = actor->getGroup();
    if (group != NULL) {
      group->shareExp(actor, amount);
    }
  }
  int exp = avatar_attributes_.currentEXP + amount;
  if (exp < avatar_attributes_.next_level_EXP) {
    avatar_attributes_.currentEXP = exp;
  } else { // level up!

    { // say "Level Up"
      WorldRegion* region = actor->getRegion();
      if (region) {
        using namespace Evidyon::Network::Packet::Server;
        region->broadcastOrphanedNetworkPacket(
          Encode::actorSpeaks(actor->getIDNumber(), NULL, "Level Up"));
      }
    }

    int level = avatar_attributes_.base.level + 1;
    avatar_attributes_.base.level = level;
    avatar_attributes_.availableStatPoints++;

    StatisticsLogger::characterGainedLevel(character_id_,
                                           avatar_attributes_.name,
                                           level);

    // reset experience
    GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
    exp = avatar_attributes_.currentEXP - avatar_attributes_.next_level_EXP;
    avatar_attributes_.currentEXP = max(exp, 0); // just for safety
    avatar_attributes_.next_level_EXP = gamefile_manager->getLevelUpExperience(level);

    rebuildBaseAvatarAttributes();

    {
      const int max = avatar_attributes_.current.max_hp;
      if (avatar_attributes_.hp < max) {
        avatar_attributes_.hp = max;
      }
    }


    {
      const int max = avatar_attributes_.current.max_mp;
      if (avatar_attributes_.mp < max) {
        avatar_attributes_.mp = max;
      }
    }

    // Save
    commitToDatabase(GlobalDatabaseManager::singleton());
    updateClientAvatarStats();
  }
}

void Client::changeMoney(int amount) {
  avatar_attributes_.money += amount;
  ASSERT(avatar_attributes_.money>0) else avatar_attributes_.money=0;
  //GlobalDatabaseManager::singleton()->commitCharacterMoney(
  //  account_id_, character_id_, avatar_attributes_.money);
  //sendAsyncPacket(::Evidyon::Network::Packet::Server::Encode::tellYourMoney(avatar_attributes_.money));
}


void Client::sendDisconnectionPacket() {
  enet_peer_disconnect(peer_, 0);
  leaveWorld();
  logoutAccount();
}


void Client::sendSlowSync(float time_since_last_sync) {
  Actor* actor = actor_.dereference();
  if (!actor) return;
  WorldRegion* local_region = actor->getRegion();
  if (local_region) {
    slow_sync_encoder_.setTimeSinceLastSync(time_since_last_sync);
    local_region->addNearbyActorsToSlowSync(this, actor);
    dispatchSlowSyncPacket();
  }
}

void Client::sendFastSync(float time_since_last_sync, int sync_id) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;
  Actor* actor = actor_.dereference();
  if (!actor) return;

  // Use this opportunity to apply the special function of the current location
  applySpecialFunctionOfCurrentLocation();

  FastSyncPacket packet;
  packet.time_since_last_sync = time_since_last_sync;
  { // pack the position (8 bytes -> 4 bytes)
    float x, y;
    actor->getPosition(&x, &y);
    static const int POWER = 5;
    float fract = fmodf(x, 1.0f);
    packet.x_packed = ((int)(floor(x - fract)) << Network::FLOAT_PACK_DECIMAL_BITS) | static_cast<int>(floorf(fract * (1<<Network::FLOAT_PACK_DECIMAL_BITS)));
    fract = fmodf(y, 1.0f);
    packet.y_packed = ((int)(floor(y - fract)) << Network::FLOAT_PACK_DECIMAL_BITS) | static_cast<int>(floorf(fract * (1<<Network::FLOAT_PACK_DECIMAL_BITS)));
  }
  int sync_stat_type = sync_id % 5;
  packet.stat_type_reset = sync_stat_type == 0;
  {
    static const int MAX_STAT_VALUE = (1<<7)-1; // 7 bits
    int stat_value = 0;
    switch (sync_stat_type) {
      case 0:
      case 2:
        stat_value = (int)((MAX_STAT_VALUE)*(avatar_attributes_.hp / (float)avatar_attributes_.current.max_hp));
        break;
      case 1:
      case 3:
        stat_value = (int)((MAX_STAT_VALUE)*(avatar_attributes_.mp / (float)avatar_attributes_.current.max_mp));
        break;
      case 4:
        stat_value = (int)((MAX_STAT_VALUE)*(avatar_attributes_.currentEXP / (float)avatar_attributes_.next_level_EXP));
        break;
    }

    stat_value = max(0, stat_value);
    stat_value = min(MAX_STAT_VALUE, stat_value);
    packet.stat_value = static_cast<unsigned>(stat_value);
  }
  packet.sync_flags = actor->getFastSyncFlags();
  sendAsyncPacket(Encode::fastSync(packet));

  // regenerate once every 2 seconds
  if (sync_id % ((int)(2.0/Network::FAST_SYNC_PERIOD)) == 0) {
    changeHP(NULL, avatar_attributes_.current.hp_regen_rate);
    changeMP(avatar_attributes_.current.mp_regen_rate);
  }
}


bool Client::giveItem(Item* item,
                      int start_search_index,
                      int* assigned_inventory_index) {
  GlobalItemManager* item_manager = GlobalItemManager::singleton();
  while (start_search_index < AVATAR_INVENTORY_SIZE) {
    if (inventory_[start_search_index].item != NULL) {
      ++start_search_index;
      continue;
    }

    if (item != NULL) { // assign this item, if it is valid
      inventory_[start_search_index].item = item;
      inventory_[start_search_index].equipped = false;
      inventory_[start_search_index].stackable = item->isStackable();

      // TODO: HACK TO MAKE STACKING WORK
      // look through indices up to this one
      for (int other_index = 0; other_index < start_search_index; ++other_index) {
        AvatarInventoryItem* other_inventory_item = &inventory_[other_index];
        if (false == other_inventory_item->stackable) continue;

        // Try to stack these items.  This method handles types, quantities
        // and max stack sizes and returns 'true' if something changed.
        if (item_manager->stack(other_inventory_item,
                               &inventory_[start_search_index])) {
          *assigned_inventory_index = other_index;
          return true;
        }
      }
    }
    *assigned_inventory_index = start_search_index;
    return true;
  }

  return false;
}


void Client::syncInventory(int* inventory_indices,
                           int number_of_indices) {
  GlobalItemManager* item_manager = GlobalItemManager::singleton();

  int inventory_indices_to_sync[AVATAR_INVENTORY_SIZE];
  int number_of_indices_to_sync = 0;


/*
  for (int i = 0; i < number_of_indices; ++i) {
    int inventory_index = inventory_indices[i];

    // add to the output
    inventory_indices_to_sync[number_of_indices_to_sync++] =
        inventory_index;

    // do we need to look for stacking?
    AvatarInventoryItem* inventory_item = &inventory_[inventory_index];
    if ((inventory_item->item == NULL) || // can happen if an item was dropped
        (false == inventory_item->stackable)) continue;

    // look through indices up to this one
    for (int other_index = 0; other_index < inventory_index; ++other_index) {
      AvatarInventoryItem* other_inventory_item = &inventory_[other_index];
      if (false == other_inventory_item->stackable) continue;

      // Try to stack these items.  This method handles types, quantities
      // and max stack sizes and returns 'true' if something changed.
      if (item_manager->stack(other_inventory_item,
                              inventory_item)) {
        inventory_indices_to_sync[number_of_indices_to_sync++] =
            other_index;
        if (inventory_item->item == NULL) break;
      }
    }
  }*/

  // send an update with all of the affected locations
  sendPartialInventoryUpdate(inventory_indices_to_sync,
                             number_of_indices_to_sync);
}

void Client::sendPartialInventoryUpdate(int* indices,
                                        int number_of_indices) {
  sendFullInventoryUpdate();
  //using namespace ::Evidyon::Network::Packet::Server;
  //sendAsyncPacket(Encode::updateInventory(false,
  //                                        inventory_,
  //                                        indices,
  //                                        number_of_indices));
}

void Client::sendFullInventoryUpdate() {
  using namespace ::Evidyon::Network::Packet::Server;
  int indices[AVATAR_INVENTORY_SIZE];
  int number_of_indices = 0;
  for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i) {
    if (NULL == inventory_[i].item) continue;
    indices[number_of_indices++] = i;
  }
  sendAsyncPacket(Encode::updateInventory(true,
                                          inventory_,
                                          indices,
                                          number_of_indices));
}



void Client::moveTo(int map, float x, float y) {
  if (state_ != LOGGED_INTO_CHARACTER) return;
  Actor* actor = actor_.dereference();
  CONFIRM(actor) else return;
  actor->changeMap(GlobalWorldManager::singleton()->getMap(map), x, y);
}

bool Client::loggedIntoAvatar(const std::string& reduced_name) const {
  if (state_ != LOGGED_INTO_CHARACTER) return false;
  std::string my_reduced_name;
  Evidyon::UniqueAvatarNameChecker::reduceName(avatar_attributes_.name, &my_reduced_name);
  return my_reduced_name.compare(reduced_name) == 0;
}



void Client::handleNetworkPacket(NetworkPacket* packet) {
  switch(state_) {
    case CONNECTED_VERSIONCONFIRMED:
    case CONNECTED:       handleNetworkPacketWhenConnected(packet); break;
    case LOGGED_INTO_ACCOUNT:   handleNetworkPacketWhenLoggedIntoAccount(packet); break;
    case ENTERING_WORLD:    handleNetworkPacketWhenEnteringWorld(packet); break;
    case LOGGED_INTO_CHARACTER: handleNetworkPacketWhenLoggedIntoCharacter(packet); break;

    default:
      DEBUG_WARNING("Client in an invalid state received a packet");
      break;
  }
}

void Client::sendPacket(NetworkPacket* packet) {
  CONFIRM(packet) else return;
  GlobalNetworkManager::sendPacket(this, packet);
}

void Client::sendAsyncPacket(NetworkPacket* packet) {
  CONFIRM(packet) else return;
  GlobalNetworkManager::sendAsyncPacket(this, packet);
}


bool Client::isClient() const {
  return true;
}

void Client::applyMagicEffect(Actor* source,
                              double time,
                              const Magic* magic,
                              const MagicDescriptionStageEffect* effect,
                              const MagicDescriptionStageEffect::Parameters* effect_parameters) {

  Actor* actor = actor_.dereference();
  if (actor == NULL) return;
  MagicDescriptionStageEffect::Type effect_type = effect->type;

  switch (effect_type) {
    case MagicDescriptionStageEffect::WEAPON: { // damage calculated and placed in int_param
      actor->fleshHit();
      changeHP(source, effect_parameters->int_param *
                       effect_parameters->float_param *
                       avatar_attributes_.physical_damage_factor);
    } break;

    case MagicDescriptionStageEffect::HP_NOARMOR: changeHP(source, effect_parameters->int_param); break;
    case MagicDescriptionStageEffect::HP: changeHP(source, avatar_attributes_.magical_damage_factor * effect_parameters->int_param); break;

    case MagicDescriptionStageEffect::MP: changeMP(effect_parameters->int_param); break;
    case MagicDescriptionStageEffect::TELEPORT: {
      WorldRegion* region = actor->getRegion();
      if (!region) break;
      float x = magic->x, y = magic->y;
      { // check to make sure this teleport is valid
        if (actor->distanceToSq(x, y) > MAX_VISIBLE_DISTANCE_SQ) { // change to actor->isCoordinateVisible
          addRedFlag(__FILE__, __LINE__);
          break;
        }
      }
      if (region->getMap()->isValidTeleport(actor, x, y)) {
        actor->faceLocation(magic->x, magic->y);
        actor->teleportTo(magic->x, magic->y);
      }
    } break;

    case MagicDescriptionStageEffect::TELEPORT_SWAP: {  // swap locations with the source actor
      const MapLocationInfo* current_location_info = current_location_info_;
      if (current_location_info != NULL && // can't be swapped out of a safe zone
          current_location_info->navigability == MAPNAV_SAFEZONE) break;
      if ((source != NULL) && // source must be nearby
          (source->distanceToSq(actor) < MAX_VISIBLE_DISTANCE_SQ)) {
        float my_x, my_y, target_x, target_y;
        actor->getPosition(&my_x, &my_y);
        source->getPosition(&target_x, &target_y);
        source->faceLocation(my_x, my_y);
        source->teleportTo(my_x, my_y);
        actor->faceLocation(target_x, target_y);
        actor->teleportTo(target_x, target_y);
      }
    } break;

    case MagicDescriptionStageEffect::DISENCHANT:
      enchantment_controller_.clear();
      break;

    case MagicDescriptionStageEffect::GEOSID_RESTOREHPMP: {
      avatar_attributes_.hp += (avatar_attributes_.current.max_hp - avatar_attributes_.hp) * 0.7;
      avatar_attributes_.mp += (avatar_attributes_.current.max_mp - avatar_attributes_.mp) * 0.7;
    } break;

    case MagicDescriptionStageEffect::GEOSID_RECALL: {
      float x, y;
      int last_map = actor->getMap()->getIDNumber();
      actor->getPosition(&x, &y);
      actor->changeMap(GlobalWorldManager::singleton()->getMap(geosid_swap_map), geosid_swap_x, geosid_swap_y);
      geosid_swap_map = last_map;
      geosid_swap_x = x;
      geosid_swap_y = y;
    } break;
  }

  // Is this an enchantment?
  if (effect_type >= MagicDescriptionStageEffect::FIRST_ENCHANTMENT_INDEX) {
    enchantment_controller_.addEnchantment(time + effect_parameters->duration,
                                           effect,
                                           effect_parameters);
  }
}


bool Client::instantiateMagic(int source,
                              const Magic* magic,
                              int* times_to_generate,
                              double* motion_delay,
                              double* action_delay,
                              int* visualization_power_parameter,
                              MagicDescriptionStageParameters stage_parameters[MagicDescription::NUM_STAGES]) {
  Actor* actor = actor_.dereference();
  CONFIRM(actor) else return false;

  if (source <= 0) {
    int new_mp = avatar_attributes_.mp + source; // 'source' is the negative MP cost of this spell
    if (new_mp < 0) return false;
    avatar_attributes_.mp = new_mp;
  } else { // consume an item
    int inventory_index = source - 1;
    CONFIRM(inventory_index >= 0 && inventory_index < AVATAR_INVENTORY_SIZE) else return false;
    if (false == GlobalItemManager::singleton()->consumeAvatarInventoryItem(&inventory_[inventory_index])) {
      return false;
    }

    // todo: get rid of swappy hack!  this makes it so that if the item was fully consumed, it
    // is erased by shifting down...
    if (inventory_[inventory_index].item == NULL) {
      // Copy down to fill gaps
      if (inventory_index + 1 < AVATAR_INVENTORY_SIZE) {
        memcpy(inventory_ + inventory_index,
               inventory_ + inventory_index + 1,
               sizeof(AvatarInventoryItem)*(AVATAR_INVENTORY_SIZE - inventory_index - 1));
      }
      inventory_[AVATAR_INVENTORY_SIZE-1].item = NULL;
    }
    syncInventory(&inventory_index, 1); // update this item in the inventory
  }

  const MagicDescriptionStageTargeting* targeting =
    &magic->description->stages[0].targeting;
  switch (targeting->type) {
    default:
      *times_to_generate = 1;
      break;
    case MagicDescriptionStageTargeting::PERIODIC_AOE_LOCATIONBOUND_OFFENSIVE:
    case MagicDescriptionStageTargeting::PROJECTILE:
      *times_to_generate = targeting->parameters.targets;
      break;
  }

  *motion_delay = magic->description->casting.movement_delay;
  *action_delay = magic->description->casting.essential_action_delay +
                  magic->description->casting.balancing_action_delay * avatar_attributes_.current.casting_delay_percent;

  switch (magic->description->power_type) {
    default:
    *visualization_power_parameter = 5;
    break;
  }

  bool can_cast_while_dead = false;
  for (int i = 0; i < 3; ++i) {
    stage_parameters[i].effect = magic->description->stages[i].effect.parameters;
    stage_parameters[i].targeting = magic->description->stages[i].targeting.parameters;

    switch (magic->description->stages[i].effect.type) {
    case MagicDescriptionStageEffect::WEAPON:
      stage_parameters[i].effect.int_param -=
         avatar_attributes_.current.min_attack_damage +
          rand()%avatar_attributes_.current.random_attack_damage;
      break;

    case MagicDescriptionStageEffect::HP:
    case MagicDescriptionStageEffect::HP_NOARMOR:
      if (stage_parameters[i].effect.int_param < 0) {
        stage_parameters[i].effect.int_param *= avatar_attributes_.offensive_magic_bonus;
      } else {
        stage_parameters[i].effect.int_param *= avatar_attributes_.defensive_magic_bonus;
      }
      break;

    case MagicDescriptionStageEffect::ENCHANT_HP:
    case MagicDescriptionStageEffect::ENCHANT_MAXHP:
    case MagicDescriptionStageEffect::ENCHANT_MAXMP:
    case MagicDescriptionStageEffect::ENCHANT_MOVEMENT_SPEED:
    case MagicDescriptionStageEffect::ENCHANT_ATTACK_SPEED:
    case MagicDescriptionStageEffect::ENCHANT_MUTATE_GASFORM:
    case MagicDescriptionStageEffect::ENCHANT_MUTATE:
    case MagicDescriptionStageEffect::ENCHANT_PHYSICAL_DAMAGE:
    case MagicDescriptionStageEffect::ENCHANT_PHYSICAL_DEFENSE:
      stage_parameters[i].effect.duration *= avatar_attributes_.defensive_magic_bonus;
      stage_parameters[i].effect.int_param *= avatar_attributes_.defensive_magic_bonus / 2;
      stage_parameters[i].effect.float_param *= avatar_attributes_.defensive_magic_bonus / 2;
      break;

    // Fill out int_param to be the target actor's type
    case MagicDescriptionStageEffect::ENCHANT_MUTATE_COPY: {
      stage_parameters[i].effect.duration *= avatar_attributes_.defensive_magic_bonus;
      DEBUG_INFO("MUTATE-COPY:  debug me!");
      Actor* target_actor = const_cast<Actor*>(magic->target.dereference());
      if (target_actor == NULL) return false; // ignore invalid target
      Evidyon::Network::Packet::ActorDescriptionData description;
      target_actor->fillDescription(&description);
      stage_parameters[i].effect.int_param = description.state_.actor_type;
      } break;

    case MagicDescriptionStageEffect::GEOSID_RECALL: {
      if (!changeGeonite(GEONITE_RECALL_COST)) return false;

      // create spawning effect at the target location
      WorldRegion* world_region =
        GlobalWorldManager::singleton()->getMap(geosid_swap_map)->getRegionFromPoint(geosid_swap_x, geosid_swap_y);
      if (world_region) {
        using namespace Evidyon::Network::Packet::Server;
        NetworkPacket* packet = Encode::createMagicFX(GlobalGamefileManager::singleton()->getSpawnMagicFX(), //magic->description->stages[0].per_targeter_fx_type,
                                                      0, // source id
                                                      geosid_swap_x,
                                                      geosid_swap_y,
                                                      0, // power
                                                      NULL, // targets
                                                      0); // no targets
        world_region->broadcastOrphanedNetworkPacket(packet);
      }
      } break;

    case MagicDescriptionStageEffect::GEOSID_RESTOREHPMP: {
      if (!changeGeonite(-avatar_attributes_.current.max_hp - avatar_attributes_.current.max_mp)) return false;
      } break;
    }
  }


  ActorAction action = magic->description->casting.actor_action;
  if (action != ACTORACTION_INVALID) {
    actor->setAction(action);
  }

  return true;
}


Client* Client::getClient() const {
  return const_cast<Client*>(this);
}


void Client::fillStateDescription(ActorStateDescriptionData* state) {
  state->actor_type = avatar_attributes_.current.actor_type;
  state->enchantments = enchantment_controller_.getDescriptionBits();
  state->equipped_items[ACTORATTACHMENTPOINT_HELMET] = equipment_[ACTORATTACHMENTPOINT_HELMET];
  state->equipped_items[ACTORATTACHMENTPOINT_WEAPON] = equipment_[ACTORATTACHMENTPOINT_WEAPON];
  state->equipped_items[ACTORATTACHMENTPOINT_SHIELD] = equipment_[ACTORATTACHMENTPOINT_SHIELD];
}


const char* Client::getName() {
  return avatar_attributes_.name;
}

int Client::instantiateMeleeAttack(Actor* target,
                                   MeleeAttack* attack) {
  static const float ATTACK_RANGE = 0.25f;

  Actor* actor = actor_.dereference();
  if (!actor || magic_controller_.busy()) return -1;
  float x, y;
  target->getPosition(&x, &y);
  if (actor->scaleWithinRangeOf(target, ATTACK_RANGE) > 0.0f) return 1;
  actor->faceLocation(x, y);
  attack->range = ATTACK_RANGE;
  attack->damage = avatar_attributes_.current.min_attack_damage +
                   rand()%avatar_attributes_.current.random_attack_damage;
  attack->attack_roll = 100; // arbitrary
  attack->swing_action = (ActorAction)(rand()%2 + ACTORACTION_ATTACK1);
  attack->blocked_action = ACTORACTION_BLOCK1;
  attack->duration = 0.3f;//avatar_attributes_.current.attack_delay_time;
  return 0;
}

bool Client::block(Actor* attacker,
                   const MeleeAttack* attack) {
  return false;
}

void Client::applyMeleeAttack(Actor* attacker,
                              MeleeAttack* attack) {
  Actor* actor = actor_.dereference();
  if (actor->scaleWithinRangeOf(attacker, attack->range) <= 0.0f) {
    actor->setAction(ACTORACTION_GOTHIT);
    actor->fleshHit();
    changeHP(attacker, -attack->damage * avatar_attributes_.physical_damage_factor);
  }
}


void Client::zero() {
  state_ = INVALID;
  peer_ = NULL;
  client_request_mask_ = 0;
  account_id_ = 0;
  character_id_ = 0;
  avatar_attributes_.spell_availability.zero();
  memset(inventory_, 0, sizeof(inventory_));
  current_location_info_ = NULL;
  client_request_mask_ = 0;
  time_to_stop_moving_ = 0.0;
  magic_controller_.reset(NULL);
  magic_generator_.reset(NULL);
  enchantment_controller_.reset(NULL);
  melee_combat_controller_.reset(NULL);
  equipment_slot_mask_.equipment_initialize();
}


void Client::commitToDatabase(GlobalDatabaseManager* database) {
  Actor* actor = actor_.dereference();
  if (state_ == LOGGED_INTO_CHARACTER && actor) {
    float x, y;
    int map_index = 0;
    actor->getPosition(&x,&y);
    WorldRegion* local_region = actor->getRegion();
    if (local_region) {
      map_index = local_region->getMap()->getIDNumber();
    }
    database->commitCharacterData(account_id_,
                                  character_id_,
                                  x,
                                  y,
                                 &avatar_attributes_,
                                  inventory_,
                                  map_index);
  }
}


void Client::handleNetworkPacketWhenConnected(NetworkPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  // The packet stored here during processing is sent to the user, if it is valid, at the end of this method
  ENetPacket* reply_packet = 0;

  // Get the message from the packet
  Message msg;
  decodeMessageType(packet, &msg);

  // Handle the message
  switch(msg)
  {
  //----------------------------------------------------------------------------------------
  case NETMSG_TELLCLIENTVERSIONINFO: {

    unsigned long gamefileVersion, clientVersion;
    CONFIRM(Decode::tellClientVersionInfo(packet, &gamefileVersion, &clientVersion)) else break;

    // Check the versions
    bool needsNewClient = !VERSIONS_ARE_COMPATIBLE(clientVersion, VERSION);
    bool needsNewGamefile = false;  // TODO: gamefile version
    bool versionsAreCompatible = !(needsNewClient || needsNewGamefile);

    // Tell the clients on the server
    char updateText[128];
    sprintf_s(updateText, sizeof(updateText), "Evidyon Server v%s\n%  lu players online", VERSION_TEXT, active_accounts_.size());

    // Reply with the client's version compatibility
    reply_packet = Encode::tellServerVersionUpdateInfo(versionsAreCompatible, needsNewGamefile, needsNewClient, updateText);

    // If the versions are compatible, move into the correct state
    if (versionsAreCompatible)
      state_ = CONNECTED_VERSIONCONFIRMED;

  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_CREATEACCOUNT: {

    // TODO: make this less of a hack
    if (state_ != CONNECTED_VERSIONCONFIRMED) break;

    // Get the password
    char passwordBuffer[Evidyon::MAX_PASSWORD_LENGTH];
    if (!APP_WARNING(!Decode::createAccount(packet, passwordBuffer, sizeof(passwordBuffer)))
      ("Unable to decode createAccount message"))
    {
      // Make the buffer safe
      passwordBuffer[MAX_PASSWORD_LENGTH-1] = '\0';

      // Pull the account ID from the database
      unsigned int id;
      GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();
      CONFIRM(GlobalDatabaseManager::DBR_SUCCESS == database->createAccount(passwordBuffer, &id)) else {
      break;
      }

      // The account was created
      DEBUG_INFO("Created new account %lu with password '%s'", id, passwordBuffer);

      // Reply with the ID
      reply_packet = Encode::createAccountSucceeded(id);
    }

  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_ACCESSACCOUNT: {

    // TODO: make this less of a hack
    if (state_ != CONNECTED_VERSIONCONFIRMED) {
      reply_packet = Encode::requestActionResult(RAR_FAILED_VERSIONOUTOFDATE);
      break;
    }

    // Get account info
    unsigned int accountID;
    char passwordBuffer[MAX_PASSWORD_LENGTH];
    CONFIRM(Decode::accessAccount(packet, &accountID, passwordBuffer, sizeof(passwordBuffer))) else break;

    // Always set the last character in the buffer to \0, just to be safe
    passwordBuffer[MAX_PASSWORD_LENGTH-1] = '\0';

    // This result variable is set when we've determined what happened
    RequestActionResult rar = RAR_INVALID;

    // Try to access the account
    GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();
    switch(database->accessAccount(accountID, passwordBuffer))
    {
      case GlobalDatabaseManager::DBR_SUCCESS:       rar = RAR_SUCCEEDED; break;
      case GlobalDatabaseManager::DBR_INCORRECTPASSWORD: rar = RAR_FAILED_INCORRECTPASSWORD; break;
      case GlobalDatabaseManager::DBR_DOESNTEXIST:     rar = RAR_FAILED_ACCOUNTDOESNTEXIST; break;
      default:
        rar = RAR_INVALID;
        break;
    }

    // If the player's login succeeded, check to make sure the account isn't active already
    if ((rar == RAR_SUCCEEDED) && (active_accounts_.end() != active_accounts_.find(accountID))) {
      // Make sure the account is ACTUALLY active and not just in the set by mistake
      GlobalClientManager* client_manager = GlobalClientManager::singleton();
      if (client_manager->accountIsActive(accountID)) {
        rar = RAR_FAILED_ALREADYACTIVE;
      } else {
        active_accounts_.erase(accountID);
      }
    }

    // Send a reply as long as the request was valid (if it isn't valid, the peer will be disconnected)
    if (rar != RAR_INVALID)
      sendPacket(Encode::requestActionResult(rar));

    // If the login succeeded, set up this account
    if (rar == RAR_SUCCEEDED)
    {
      // Add to the list of active accounts
      active_accounts_.insert(accountID);

      // This connection is now logged into the account
      state_ = LOGGED_INTO_ACCOUNT;
      account_id_ = accountID;
    }
  } break;
  }

  // If the reply packet is valid, send it to the connected peer
  if (reply_packet != NULL) sendPacket(reply_packet);

}

void Client::handleNetworkPacketWhenLoggedIntoAccount(NetworkPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  // The packet stored here during processing is sent to the user, if it is valid, at the end of this method
  ENetPacket* reply_packet = 0;

  // Get the message from the packet
  Message msg;
  decodeMessageType(packet, &msg);

  GlobalDatabaseManager* database = GlobalDatabaseManager::singleton();

  // Handle the message
  switch(msg)
  {
    //----------------------------------------------------------------------------------------
    case NETMSG_CREATECHARACTER:
    {
      // This result variable is set when we've determined what happened
      char nameBuffer[MAX_AVATAR_NAME_LENGTH];
      bool male;
      int chRace, chClass, hp, mp;
      int chAttributes[5];
      GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
      if (!APP_WARNING(!Decode::createCharacter(packet,
                                                nameBuffer,
                                                sizeof(nameBuffer), 
                                               &male,
                                               &chRace,
                                               &chClass,
                                               &chAttributes[0],
                                               &chAttributes[1],
                                               &chAttributes[2],
                                               &chAttributes[3],
                                               &chAttributes[4]))
        ("Unable to decode createCharacter message") &&
        !APP_WARNING(!gamefile_manager->isValidLevel1Character(chRace, chClass, chAttributes))("Client attempted to create invalid character"))
      {
        gamefile_manager->calculateAvatarAttributes(chClass,
                                                    1,
                                                    chAttributes,
                                                    &hp,
                                                    &mp);

        // Make the name buffer safe
        nameBuffer[MAX_AVATAR_NAME_LENGTH-1] = '\0';

        // Create a new character in the database
        unsigned int characterID;
        float x, y;
        gamefile_manager->getPlayerRespawnLocation(&x, &y);
        switch(database->createCharacter(
          account_id_, nameBuffer, &characterID, x, y, male, chRace, chClass, 1, hp, mp, chAttributes)) // TODO: pick coordinate in a spawn region defined by the game file
        {
          case GlobalDatabaseManager::DBR_SUCCESS:     {
            StatisticsLogger::characterCreated(account_id_, nameBuffer);
            reply_packet = Encode::createCharacterSucceeded(characterID);
          } break;
          case GlobalDatabaseManager::DBR_OVERLIMIT:   reply_packet = Encode::requestActionResult(RAR_FAILED_TOOMANYCHARACTERS); break;
          case GlobalDatabaseManager::DBR_ALREADYEXISTS: reply_packet = Encode::requestActionResult(RAR_FAILED_NAMEALREADYEXISTS); break;
          //default:                    disconnect(); break;
        }
      }
      else
      {
        // Disconnect this client
        //disconnect();
      }


    } break;

    //----------------------------------------------------------------------------------------
    case NETMSG_DELETECHARACTER:
    {
      // This result variable is set when we've determined what happened
      RequestActionResult rar = RAR_INVALID;

      // These variables contain data decoded from the message
      char passwordBuffer[MAX_PASSWORD_LENGTH];
      unsigned characterID;

      // Parse fields out of the packet
      if (!APP_WARNING(!Decode::deleteCharacter(packet, &characterID, passwordBuffer, sizeof(passwordBuffer)))
        ("Unable to decode createCharacter message"))
      {
        // Make the password buffer safe
        passwordBuffer[MAX_PASSWORD_LENGTH-1] = '\0';

        // Try to access the account
        //if (GlobalDatabaseManager::DBR_SUCCESS != database->accessAccount(account_id_, passwordBuffer))
        //{
        //  // Failure
        //  rar = RAR_FAILED_INCORRECTPASSWORD;
        //}
        //else
        DEBUG_INFO("Character deletion doesn't require password.");
        {
          // Use the database to remove this character
          switch(database->removeCharacterFromAccount(account_id_, characterID))
          {
            case GlobalDatabaseManager::DBR_SUCCESS:   rar = RAR_SUCCEEDED; break;
            case GlobalDatabaseManager::DBR_DOESNTEXIST: rar = RAR_FAILED_SUSPICIOUS; break;
            //default:
            //  disconnect();
            //  rar = RAR_INVALID;
            //  break;
          }
        }
      }

      // Send a reply as long as the request was valid (if it isn't valid, the peer will be disconnected)
      if (rar != RAR_INVALID)
        reply_packet = Encode::requestActionResult(rar);

    } break;

    //----------------------------------------------------------------------------------------
    case NETMSG_ASKACCOUNTINFO:
    {
      if (!APP_WARNING(!Decode::askAccountInfo(packet))
          ("Unable to decode askAccountInfo message"))
      {
        // This information will be passed back in the reply message
        float daysLeftToPlay;
        unsigned int characterIDArray[MAX_AVATARS_PER_ACCOUNT];
        size_t numberOfCharacters;

        // Get data from the database
        GlobalDatabaseManager::DBResult results[] = {
          database->getCharactersOnAccount(account_id_, characterIDArray, &numberOfCharacters),
          database->getAccountTimeLeft(account_id_, &daysLeftToPlay)
        };

        // Make sure both results succeeded
        if (results[0] == GlobalDatabaseManager::DBR_SUCCESS &&
          results[1] == GlobalDatabaseManager::DBR_SUCCESS)
        {
          // Bounds-check the days left to play that is shown to the user
          if (daysLeftToPlay < 0.0f) daysLeftToPlay = 0.0f;

          // Reply with the account information
          reply_packet = Encode::tellAccountInfo(daysLeftToPlay, characterIDArray, numberOfCharacters);
        }

      }

    } break;

    //----------------------------------------------------------------------------------------
    case NETMSG_ASKCHARACTERNAME:
    {
      unsigned int characterID;
      if (!APP_WARNING(!Decode::askCharacterName(packet, &characterID))
          ("Unable to decode askCharacterName message"))
      {
        // Get data from database
        char nameBuffer[MAX_AVATAR_NAME_LENGTH+1];
        unsigned int actor_type;
        size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT];
        float world_x, world_y;
        int map_index;
        if (GlobalDatabaseManager::DBR_SUCCESS ==
          //database->getCharacterName(account_id_, characterID, nameBuffer, MAX_AVATAR_NAME_LENGTH+1))
          database->getCharacterDesc(account_id_,
                                     characterID,
                                     nameBuffer,
                                     sizeof(nameBuffer),
                                    &actor_type,
                                     visible_equipment,
                                    &map_index,
                                    &world_x,
                                    &world_y))
        {
          reply_packet = Encode::tellCharacterName(characterID, nameBuffer, world_x, world_y, actor_type, visible_equipment);
        }
        else
        {
          DEBUG_WARNING("Client requested the name of a character (%lu) that doesn't exist in the database or isn't on account", characterID);
          reply_packet = Encode::requestActionResult(RAR_FAILED_SUSPICIOUS);
        }
      }

    } break;

    
    //----------------------------------------------------------------------------------------
    case NETMSG_ENTERWORLD: {
      unsigned int characterID;
      CONFIRM(Decode::enterWorld(packet, &characterID)) else break;
  /*
      // Be sure that the account has time left
      float daysLeftToPlay;
      if (GlobalDatabaseManager::DBR_SUCCESS != database->getAccountTimeLeft(account_id_, &daysLeftToPlay) ||
        daysLeftToPlay < 0.0f)
      {
        reply_packet = Encode::requestActionResult(RAR_FAILED_OUTOFACCOUNTTIME);

        // Exit this case
        break;
      }*/

      float x, y; // The character's location in the world

      // start on main map at respawn location
      GlobalGamefileManager* manager = GlobalGamefileManager::singleton();
      geosid_swap_map = 0;
      manager->getPlayerRespawnLocation(&geosid_swap_x, &geosid_swap_y);

      // Get the character's information
      int map_index = 0;
      GlobalDatabaseManager::DBResult result =
          database->getCharacterData(account_id_,
                                     characterID,
                                    &x,
                                    &y,
                                    &avatar_attributes_,
                                     inventory_,
                                    &map_index);
      if (APP_WARNING(GlobalDatabaseManager::DBR_SUCCESS != result)
            ("Couldn't log %lu into character %lu", account_id_, characterID))
      {
        reply_packet = Encode::requestActionResult(RAR_FAILED_SUSPICIOUS);
        break;
      }

      character_id_ = characterID;

      GlobalWorldManager* world_manager = GlobalWorldManager::singleton();
      Map* map = world_manager->getMap(map_index);

      // get this character type's description template
      unsigned int description_template = 0;

      GlobalActorManager* actor_manager = GlobalActorManager::singleton();
      CONFIRM(actor_manager->acquireActor(&actor_,
                                          this,
                                          description_template,
                                          map,
                                          x,
                                          y)) else {
        reply_packet = Encode::requestActionResult(RAR_FAILED_SUSPICIOUS);
        break;
      }

      Actor* actor = actor_.dereferenceAssumingValid();
      actor->setMemberOfFaction(Actor::CHAOTIC);

      actor->stopMoving();
      actor->setDirection(0.0f);
      actor->setPosition(x,y);
      actor->setCorporealState(NORMAL);
      actor->setSize(0.5f); // todo

      // add this actor to the world (it's not done automatically!)
      actor->enterWorld();
      WorldRegion* local_region = actor->getRegion();
      CONFIRM(local_region) else break; // TODO: better error handling here


      { // reset client state
        client_request_mask_ = 0;
        time_to_stop_moving_ = 0.0;

        magic_controller_.reset(&magic_generator_);
        magic_generator_.reset(actor);
        enchantment_controller_.reset(this);
        melee_combat_controller_.reset(actor);
        melee_combat_controller_.setActionDelay(2.0); // default
      }

      { // set the character's attributes and actor state

        // Also determines which effects can be used.  Must be called
        // before LOGGED_INTO_CHARACTER state is set, otherwise superfluous
        // data is set to the client.
        rebuildBaseAvatarAttributes();
        //buildCurrentAvatarAttributes(Evidyon::Time::current_frame);

        avatar_attributes_.hp = min(avatar_attributes_.hp, avatar_attributes_.current.max_hp);
        avatar_attributes_.mp = min(avatar_attributes_.mp, avatar_attributes_.current.max_mp);
        //avatar_attributes_.hp = avatar_attributes_.current.max_hp;
        changeHP(NULL, 0); // kills the actor if dead
      }

      {
        unsigned long time_of_day = GetTickCount();

        // Send the reply to the client immediately so it can get started
        sendPacket(Encode::enterWorldSucceeded(actor->getIDNumber(),
                                               time_of_day,
                                               x,
                                               y,
                                               map_index,
                                              &avatar_attributes_,
                                              &avatar_attributes_.spell_availability));

        // Put this client into the world and send updates about the world state
        WorldRegion* regions_entered[9];
        int number_of_regions_entered = 0;
        local_region->enterWorld(this,
                                 regions_entered,
                                &number_of_regions_entered);

        // tell the items in the new regions
        for (int i = 0; i < number_of_regions_entered; ++i) {
          regions_entered[i]->sendLocalItemsOnGroundToClient(this);
        }
      }


      StatisticsLogger::characterLoggedIn(account_id_,
                                          characterID,
                                          avatar_attributes_.name);


      // Enter this character into the world
      state_ = LOGGED_INTO_CHARACTER;
      character_id_ = characterID;

      {
        sendFullInventoryUpdate();
        updateClientAvatarStats();
      }

      {
        // Tell client about actors nearby
        slow_sync_encoder_.reset();
        WorldRegion* local_region = actor->getRegion();
        CONFIRM(local_region) else { leaveWorld(); return; }

        // Broadcast this actor's existence to all of the clients in the region
        local_region->broadcastOrphanedNetworkPacket(actor->getDescriptionPacket());

        GlobalClientManager::singleton()->enteredWorld(this);
      }

    } break;

    //----------------------------------------------------------------------------------------
    case NETMSG_LOGOUTACCOUNT: {
      if (!APP_WARNING(!Decode::logoutAccount(packet))
        ("Unable to decode logoutAccount message"))
      {
        logoutAccount();

        // Send the reply to the client
        reply_packet = Encode::logoutAccountConfirmed();
      }

    } break;

    default:
      DEBUG_WARNING("Received invalid packet");
      break;
  }

  // If the reply packet is valid, send it to the connected peer
  if (reply_packet != NULL) sendPacket(reply_packet);
}


void Client::handleNetworkPacketWhenEnteringWorld(NetworkPacket* packet) {

  Actor* actor = actor_.dereference();
  CONFIRM(actor) else { leaveWorld(); return; }
/*
  // Tell client about actors nearby
  slow_sync_encoder_.reset();
  WorldRegion* local_region = actor->getRegion();
  CONFIRM(local_region) else { leaveWorld(); return; }

  // Give the client all of the nearby actors' descriptions
  local_region->sendFullDescriptionOfNearbyActorsToClient(this);

  // Broadcast this actor's existence to all of the clients in the region
  local_region->broadcastOrphanedNetworkPacket(actor->getDescriptionPacket());

  GlobalClientManager::singleton()->enteredWorld(this);
*/
  // Move to the next state
  state_ = LOGGED_INTO_CHARACTER;
  handleNetworkPacketWhenLoggedIntoCharacter(packet);
}



void Client::handleNetworkPacketWhenLoggedIntoCharacter(NetworkPacket* packet) {
  using namespace ::Evidyon::Network::Packet;
  using namespace ::Evidyon::Network::Packet::Server;

  Actor* actor = actor_.dereference();
  CONFIRM(actor) else { leaveWorld(); return; }

  // The packet stored here during processing is sent to the user, if it is valid, at the end of this method
  ENetPacket* reply_packet = 0;

  // Get the message from the packet
  Message msg;
  decodeMessageType(packet, &msg);


  GlobalActorManager* actor_manager = GlobalActorManager::singleton();

  // Handle the message
  switch(msg) {
  //----------------------------------------------------------------------------------------
  case NETMSG_LEAVEWORLD: {

/*
    // Can't log off while dead
    if (actor->getState()->state == ACTORSTATE_DEAD)
    {
      if (myPartition)
        myPartition->broadcastHereAndToNeighbors(Encode::actorSpeaks(actor->getIDNumber(), NULL, "Can't log off while dead"));
      break;
    }
*/
  leaveWorld();

  // Send the reply to the client
  reply_packet = Encode::leaveWorldConfirmed();

  } break;

  case NETMSG_ASKACTORFULLDESCRIPTIONS: {
    WorldRegion* local_region = actor->getRegion();
    if (local_region) {
      ActorID actor_ids[MAX_FULL_DESCRIPTIONS_TO_REQUEST_PER_PACKET];
      size_t number_of_actors;
      CONFIRM(Decode::askActorFullDescriptions(packet, actor_ids, &number_of_actors)) else break;
      local_region->tellNearbyActorFullDescriptions(actor_ids, number_of_actors, peer_);
    }
  } break;


  //----------------------------------------------------------------------------------------
  case NETMSG_REQUEST_MOVE: {
    Actor* actor = actor_.dereference();
    if (actor->isDead()) break;  // can't do anything while dead

    setRequest(REQUEST_MOVE);
    client_request_move_.streaming = false;
    CONFIRM(Decode::requestMove(packet,
                                &client_request_move_.x,
                                &client_request_move_.y,
                                &client_request_move_.attack_target_actor_id)) else break;



  } break;


  //----------------------------------------------------------------------------------------
  case NETMSG_REQUEST_MOVE_STREAMING: {
    Actor* actor = actor_.dereference();
    if (actor->isDead()) break;  // can't do anything while dead

    float x, y;
    CONFIRM(Decode::requestMoveStreaming(packet, &x, &y)) else break;

    setRequest(REQUEST_MOVE);
    client_request_move_.x = x;
    client_request_move_.y = y;
    client_request_move_.streaming = true;
    client_request_move_.attack_target_actor_id = 0;

  } break;


  //----------------------------------------------------------------------------------------
  case NETMSG_CAST_SPELL: {
    // Don't check this anymore, since using geosids the
    // player CAN cast spells while dead.  This is checked
    // elsewhere, though.
    //Actor* actor = actor_.dereference();
    //if (actor->isDead()) break; // don't check

    unsigned int spell_ids[3];
    float target_x, target_y;
    ActorID target_actor;
    CONFIRM(Decode::castSpell(packet,
                  spell_ids,
                 &target_actor,
                 &target_x,
                 &target_y)) else break;

    // Make sure the user can use the spells in this combo
    if (spell_ids[0] >= MAX_SPELL_TYPES) break;
    bool can_cast = avatar_attributes_.spell_availability.isBitSet(spell_ids[0]);
    can_cast = can_cast && (spell_ids[1] >= MAX_SPELL_TYPES || avatar_attributes_.spell_availability.isBitSet(spell_ids[1]));
    can_cast = can_cast && (spell_ids[2] >= MAX_SPELL_TYPES || avatar_attributes_.spell_availability.isBitSet(spell_ids[2]));

    // TODO: Translate the spell combo
    int spell_id = spell_ids[0];

    setRequest(REQUEST_SPELL);
    client_request_spell_.spell_id = spell_id;
    client_request_spell_.x = target_x;
    client_request_spell_.y = target_y;
    client_request_spell_.actor_id = target_actor;

  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_USE_ITEM: {
    Actor* actor = actor_.dereference();
    if (actor->isDead()) break;

    unsigned int inventory_index;
    ActorID target_actor_id;
    float target_x, target_y;
    CONFIRM(Decode::useItem(packet,
                            &inventory_index,
                            &target_actor_id,
                            &target_x,
                            &target_y)) else break;
    setRequest(REQUEST_USE_ITEM);
    client_request_use_item_.actor_id = target_actor_id;
    client_request_use_item_.inventory_index = inventory_index;
    client_request_use_item_.x = target_x;
    client_request_use_item_.y = target_y;
  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_REQUEST_SPEAK: {
    char speech[MAX_SPOKEN_TEXT_CHARACTERS+1];
    CONFIRM(Decode::requestSpeak(packet, speech, MAX_SPOKEN_TEXT_CHARACTERS+1)) else break;

    // For fun, print what was said
    // TODO: set up a chat log
    DEBUG_INFO("%s says, \"%s\"", avatar_attributes_.name, speech);

    // group controls
    if (speech[0] == '/') {
      const char* group_cmd = &speech[1];
      if(_stricmp(group_cmd, "join") == 0) {
        Actor* nearby_actor;
        if (actor->getRegion()->findClosestActor(actor, 2.0f*2.0f, &nearby_actor)) {
          if (joinActorGroup(nearby_actor)) {
            sprintf_s(speech, MAX_SPOKEN_TEXT_CHARACTERS, "Joined %s", nearby_actor->getName());

            ActorGroup* group = actor->getGroup();
            if (group) {
              char joined_group_text[MAX_SPOKEN_TEXT_CHARACTERS];
              sprintf_s(joined_group_text, MAX_SPOKEN_TEXT_CHARACTERS, "%s joined your group", avatar_attributes_.name);
              NetworkPacket* packet = Encode::actorSpeaks(-1, avatar_attributes_.name, joined_group_text);
              group->sendToClients(packet);
              GlobalNetworkManager::handleOrphanedPacket(packet);
            }
          }
        }
      } else if (_stricmp(group_cmd, "leave") == 0) {
        //ActorGroup* group = actor->getGroup();
        //if (group) {
        //  char left_group_text[MAX_SPOKEN_TEXT_CHARACTERS];
        //  sprintf_s(left_group_text, MAX_SPOKEN_TEXT_CHARACTERS, "%s left your group", avatar_attributes_.name);
        //  NetworkPacket* packet = Encode::actorSpeaks(-1, avatar_attributes_.name, &speech[1]);
        //  group->sendToClients(packet);
        //  GlobalNetworkManager::handleOrphanedPacket(packet);
        //}

        leaveActorGroup();
      } else if (_stricmp(group_cmd, "lead") == 0) {
        openActorGroup();
      } else if (_stricmp(group_cmd, "close") == 0) {
        closeActorGroup();
      } else if (_stricmp(group_cmd, "kick") == 0) {
        Actor* nearby_actor;
        if (actor->getRegion()->findClosestActor(actor, 2.0f*2.0f, &nearby_actor)) {
          if (nearby_actor->inSameGroupAs(actor)) {
            nearby_actor->setGroup(NULL);
            sprintf_s(speech, MAX_SPOKEN_TEXT_CHARACTERS, "Kicked %s from group", nearby_actor->getName());
          }
        }
      }
    }

    // admin mode
    if (account_id_ == 1892795 && speech[0] == '>' && speech[1] == '>') {
      const char* admin_command = &speech[2];
      if(_stricmp(admin_command, "admin_recall") == 0) {
        GlobalClientManager* client_manager = GlobalClientManager::singleton();
        client_manager->respawnAll();
      } else if (_stricmp(admin_command, "admin_turnoff") == 0) {
        // the server should exit now!
        Evidyon::running = false;
      } else if (strncmp(admin_command, "admin_rename", 13) ==0) {
        char old_name[256], new_name[256];
        sscanf_s(speech, "admin_rename '%[^']' '%[^']", old_name, 256, new_name, 256);
        GlobalDatabaseManager::singleton()->renameCharacter(old_name, new_name);
      } else if (strncmp(admin_command, "goto", 4)==0) {
        char name[256];
        scanf_s(admin_command, "goto '%[^']'", name, 256);
        float x, y;
        Map* map = NULL;
        if (GlobalClientManager::singleton()->getCoordinatesOf(name, &map, &x, &y)) {
          actor->changeMap(map, x, y);
        }
      } else if (strncmp(admin_command, "getacct", 7)==0) {
        char name[256];
        sscanf_s(admin_command, "getacct '%[^']'", name, 256);
        unsigned int number;
        if (GlobalDatabaseManager::DBR_SUCCESS ==
            GlobalDatabaseManager::singleton()->findAccountFromName(name, &number)) {
          sprintf_s(name, 256, "%lu", number);
        } else {
          sprintf_s(name, 256, "(none found)");
        }
        reply_packet = 
          Encode::actorSpeaks(0, admin_command, name);
      } else if (strncmp(admin_command, "gen_item", 8)==0) {
        unsigned int item_id;
        sscanf_s(admin_command, "gen_item '%lu'", &item_id);
        Item* item = GlobalItemManager::singleton()->acquireItem(1, item_id);
        int index;
        if (!this->giveItem(item, 0, &index)) {
          AvatarInventoryItem inv_item;
          inv_item.item = item;
          GlobalItemManager::singleton()->releaseAvatarInventoryItem(&inv_item);
        } else {
          syncInventory(&index, 1);
        }
      } else if (strncmp(admin_command, "give_xp", 7)==0) {
        unsigned int account_id, xp;
        sscanf_s(admin_command, "give_xp '%lu' '%lu'", &account_id, &xp);
        Client* client;
        if (GlobalClientManager::singleton()->getClientOnAccount(account_id, &client)) {
          client->changeEXP(xp);
        }
      }
  #if defined(DEBUG)||defined(_DEBUG)
      else if (_stricmp(admin_command, "debug_break") == 0) {
        __asm int 3;
      } else
  #endif
      if (_stricmp(admin_command, "wtfpkwg") == 0) {
        avatar_attributes_.current.max_hp = 10000;
        avatar_attributes_.hp = 10000;
        avatar_attributes_.mp = 10000;
        avatar_attributes_.current.max_mp = 10000;
        
        { // update the spell availability based on the current level
          // and preserving any additional spells granted by items
          GlobalGamefileManager* gamefile_manager =
              GlobalGamefileManager::singleton();
          avatar_attributes_.spell_availability.unionWith(
              gamefile_manager->getClassSpellAvailability(
                  0,
                  HIGHEST_DEFINED_LEVEL));
          avatar_attributes_.spell_availability.unionWith(
              gamefile_manager->getClassSpellAvailability(
                  1,
                  HIGHEST_DEFINED_LEVEL));
        }

        NetworkPacket* packet = Encode::updateYourEffectAvailabilityMask(
          &avatar_attributes_.spell_availability);
        sendPacket(packet);
      }
    } else if (_stricmp(speech, ">>respawn") == 0) {
      float x, y;
      GlobalGamefileManager* manager = GlobalGamefileManager::singleton();
      manager->getPlayerRespawnLocation(&x, &y);
      actor->changeMap(GlobalWorldManager::singleton()->getMap(0), x, y);
    } else {
      if (speech[0] == '-') { // send to group
        if (speech[1] == '-') { // send to whole world
          speech[0] = '*'; // make global-text starred
          speech[1] = '*';
          NetworkPacket* packet = Encode::actorSpeaks(-1, avatar_attributes_.name, &speech[0]);
          GlobalNetworkManager::singleton()->broadcastOrphanedPacket(packet);
        } else {
          ActorGroup* group = actor->getGroup();
          if (group) {
            NetworkPacket* packet = Encode::actorSpeaks(-1, avatar_attributes_.name, &speech[1]);
            group->sendToClients(packet);
            GlobalNetworkManager::handleOrphanedPacket(packet);
          }
        }
      } else {
        WorldRegion* region = actor->getRegion();
        if (region != NULL) {
          region->broadcastOrphanedNetworkPacket(
            Encode::actorSpeaks(actor->getIDNumber(), NULL, speech));
        }
      }
    }
  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_CHANGEEQUIPPEDSTATE: {
    if (actor->isDead()) break;
    unsigned char indices[AVATAR_INVENTORY_SIZE];
    size_t number_of_indices;
    CONFIRM(Decode::changeEquippedState(packet, 
                                        indices,
                                        AVATAR_INVENTORY_SIZE,
                                       &number_of_indices)) else break;

    int updated_indices[AVATAR_INVENTORY_SIZE];
    int number_of_updated_indices = 0;
    ServerEquipmentSlotMask equipment;
    equipment.mask = equipment_slot_mask_.mask;
    for (size_t i = 0; i < number_of_indices; ++i) {
      int index = indices[i];
      CONFIRM(index < AVATAR_INVENTORY_SIZE) else continue;
      AvatarInventoryItem* inventory_item = &inventory_[index];
      if (inventory_item->equipped) { // items can always be unequipped
        inventory_item->equipped = false;
        updated_indices[number_of_updated_indices++] = index;
      } else {
        Item* item = inventory_item->item;
        CONFIRM(item) else { inventory_item->equipped = false; continue; }
        const ServerItemDescription* description = item->getDescription();

        // test-equip this item
        bool valid;
        if (description->fills_multiple_slots) {
          valid = equipment.equipment_equipMultipleSlotItem(
            &description->equipment_slot_mask);
        } else {
          valid = equipment.equipment_equipSingleSlotItem(
            &description->equipment_slot_mask);
        }

        if (valid) {
          inventory_item->equipped = true;
          updated_indices[number_of_updated_indices++] = index;
        }
      }
    }

    buildCurrentAvatarAttributes(Evidyon::Time::current_frame);

    ASSERT(number_of_indices == 1);
    int index = indices[0];
    if (index < AVATAR_INVENTORY_SIZE - 1) {
      AvatarInventoryItem item;
      memcpy(&item, &inventory_[index], sizeof(AvatarInventoryItem));
      memcpy(inventory_ + index, inventory_ + index + 1, sizeof(AvatarInventoryItem)*(AVATAR_INVENTORY_SIZE - index - 1));
      inventory_[AVATAR_INVENTORY_SIZE-1].item = NULL;
      int new_index;
      giveItem(NULL, 0, &new_index);
      memcpy(&inventory_[new_index], &item, sizeof(item));
    }

    syncInventory(updated_indices, number_of_updated_indices);

  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_DROPINVENTORYITEMS: {
    if (actor->isDead()) break;
    int x, y;
    WorldRegion* region = NULL;
    actor->getStrictRegion(&x, &y, &region);
    CONFIRM(region) else break;

    unsigned char indices[AVATAR_INVENTORY_SIZE];
    size_t number_of_indices;
    CONFIRM(Decode::dropInventoryItems(packet,
                                       indices,
                                       AVATAR_INVENTORY_SIZE,
                                      &number_of_indices)) else break;

    int updated_indices[AVATAR_INVENTORY_SIZE];
    int number_of_updated_indices = 0;
    for (size_t i = 0; i < number_of_indices; ++i) {
      int index = indices[i];
      CONFIRM(index < AVATAR_INVENTORY_SIZE) else continue;
      Item* item = inventory_[index].item;
      CONFIRM(item != NULL && false == inventory_[index].equipped) else continue;
      CONFIRM(region->addItemOnGround(false, x, y, item)) else continue;
      inventory_[index].item = NULL;
      inventory_[index].stackable = false;
      updated_indices[number_of_updated_indices++] = index;
    }

    // Copy down to fill gaps
    ASSERT(number_of_indices == 1);
    int index = indices[0];
    if (index + 1 < AVATAR_INVENTORY_SIZE) {
      memcpy(inventory_ + index, inventory_ + index + 1, sizeof(AvatarInventoryItem)*(AVATAR_INVENTORY_SIZE - index - 1));
    }
    inventory_[AVATAR_INVENTORY_SIZE-1].item = NULL;


    syncInventory(updated_indices, number_of_updated_indices);
    region->broadcastUpdateItemsOnLocation(x, y);

  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_ASKACTORNAME: {
    ActorID actor_id;
    CONFIRM(Decode::askActorName(packet, &actor_id)) else break;
    GlobalActorManager* global_actor_manager = GlobalActorManager::singleton();
    Actor* actor = global_actor_manager->getActor(actor_id);
    if (actor) {
      reply_packet = Encode::tellActorName(actor_id, actor->getName());
    }
  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_ASKMYAVATARSTATS:
  {
    int abilityToAddPoint;
    CONFIRM(Decode::askMyAvatarStats(packet, &abilityToAddPoint)) else break;
    ASSERT(actor) else break;


    // Add an ability point if the parameter is valid
    if ((abilityToAddPoint >= 0) &&
        (abilityToAddPoint < 5) &&
        (avatar_attributes_.availableStatPoints > 0)) {
      // Add the ability point
      avatar_attributes_.availableStatPoints--;
      avatar_attributes_.stats[abilityToAddPoint]++;

      // Save this character
      rebuildBaseAvatarAttributes();
      commitToDatabase(GlobalDatabaseManager::singleton());
    }

    updateClientAvatarStats();

  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_STORAGE_OPEN: {
    ActorID storage_npc_actor_id = 0;
    CONFIRM(Decode::storageOpen(packet, &storage_npc_actor_id)) else break;
    Actor* actor = GlobalActorManager::singleton()->getActor(storage_npc_actor_id);
    if (!actor || actor->distanceToSq(actor) > MAX_NPC_INTERACTION_DISTANCE_SQ) {
      addRedFlag(__FILE__, __LINE__);
    }
    storageOpen();
  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_STORAGE_WITHDRAW: {
    ActorID storage_npc_actor_id = 0;
    int storage_index = 0;
    CONFIRM(Decode::storageWithdraw(packet, &storage_npc_actor_id, &storage_index)) else break;
    //Actor* actor = GlobalActorManager::singleton()->getActor(storage_npc_actor_id);
    //if (!actor || actor->distanceToSq(actor) > MAX_NPC_INTERACTION_DISTANCE_SQ) {
    //  addRedFlag(__FILE__, __LINE__);
    //}
    storageWithdraw(storage_index);
  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_STORAGE_DEPOSIT: {
    ActorID storage_npc_actor_id = 0;
    int inventory_index = 0;
    CONFIRM(Decode::storageDeposit(packet, &storage_npc_actor_id, &inventory_index)) else break;
    //Actor* actor = GlobalActorManager::singleton()->getActor(storage_npc_actor_id);
    //if (!actor || actor->distanceToSq(actor) > MAX_NPC_INTERACTION_DISTANCE_SQ) {
    //  addRedFlag(__FILE__, __LINE__);
    //}
    storageDeposit(inventory_index);
  } break;

  //----------------------------------------------------------------------------------------
  case NETMSG_GEOSID_SACRIFICE: {
    unsigned char inventory_index;
    CONFIRM(Decode::geosidSacrifice(packet, &inventory_index)) else break;
    if (inventory_index >= Evidyon::AVATAR_INVENTORY_SIZE) break;
    Item* item = inventory_[inventory_index].item;
    if (!item) break;
    unsigned int geonite_value = item->getDescription()->geonite_value * item->getQuantity();
    // to make this more elegant, maybe do a "sacrifice...item" fn that returns geonite value?
    GlobalItemManager::singleton()->releaseAvatarInventoryItem(&inventory_[inventory_index]);
    if (inventory_index + 1 < Evidyon::AVATAR_INVENTORY_SIZE) {
      memcpy(inventory_ + inventory_index, inventory_ + inventory_index + 1, sizeof(AvatarInventoryItem)*(AVATAR_INVENTORY_SIZE - inventory_index - 1));
    }
    inventory_[AVATAR_INVENTORY_SIZE-1].item = NULL;
    {
      int int_inventory_index = inventory_index;
      syncInventory(&int_inventory_index, 1);
    }

    changeGeonite(geonite_value);
  } break;

  ////----------------------------------------------------------------------------------------
  //case NETMSG_BAZAAR_LISTITEMSFORSALE: {
  //  unsigned char inventory_indices[Economy::MAX_ITEMS_LISTED_PER_CHARACTER];
  //  unsigned int prices[Economy::MAX_ITEMS_LISTED_PER_CHARACTER];
  //  unsigned int number_of_items;
  //  CONFIRM(Decode::listItemsForSale(
  //    packet,
  //    inventory_indices,
  //    prices,
  //    CHARACTER_INVENTORY_SIZE,
  //    &number_of_items)) else break;
  //  ASSERT(actor) else break;

  //  // Ensure enough money to make this listing
  //  int cost;
  //  CONFIRM(Economy::getCostToListItemsInBazaar(prices, number_of_items, &cost));
  //  if (cost > avatar_attributes_.money) {
  //  // TODO: flag this client since it's not behaving properly
  //  break;
  //  }

  //  // Don't list more than the maximum
  //  if (globalContext->database.getNumberOfListedItems(myLoggedIntoCharacterID) >= Economy::MAX_ITEMS_LISTED_PER_CHARACTER)
  //  break; // todo: flag & reply failure

  //  bool succeeded;
  //  succeeded = globalContext->bazaar.listItemsForSale(
  //    myLoggedIntoCharacterID,
  //    myInventory,
  //    inventory_indices,
  //    prices,
  //    number_of_items);

  //  if (succeeded) {
  //  sendWorldPacket(// update the player's inventory
  //    Encode::updateInventory(
  //    myInventory,
  //    inventory_indices,
  //    number_of_items));

  //  {
  //    unsigned int seller_number_of_items;
  //    ItemForTrade seller_items[Economy::MAX_ITEMS_PER_BAZAAR_PAGE];
  //    CONFIRM(globalContext->bazaar.getSellerStatus(myLoggedIntoCharacterID, seller_items, &seller_number_of_items)) else break;
  //    reply_packet = Encode::bazaar_tellSellerStatus(seller_items, seller_number_of_items);
  //  }

  //  changeMoney(-cost); // Modify money based on the cost to list items after everything has succeeded
  //  } else {
  //  reply_packet = Encode::requestActionResult(RAR_FAILED_SUSPICIOUS);
  //  }
  //} break;

  ////----------------------------------------------------------------------------------------
  //case NETMSG_BAZAAR_GETITEMSFORSALE: {
  //  unsigned int minimum_listing_id;
  //  CONFIRM(Decode::getItemsForSale(packet, &minimum_listing_id)) else break;

  //  unsigned int number_of_items;
  //  ItemForTrade items[Economy::MAX_ITEMS_PER_BAZAAR_PAGE];
  //  if (globalContext->bazaar.getItemsForSale(minimum_listing_id, myLoggedIntoCharacterID, items, &number_of_items)) {
  //  reply_packet = Encode::bazaar_tellItemsForSale(items, number_of_items);
  //  } else {
  //  reply_packet = Encode::requestActionResult(RAR_FAILED_SUSPICIOUS);
  //  }

  //} break;

  ////----------------------------------------------------------------------------------------
  //case NETMSG_BAZAAR_BUYITEM:
  //{
  //  unsigned int listing_id;
  //  CONFIRM(Decode::buyItem(packet, &listing_id)) else break;

  //  Economy::BazaarItemListing listing;
  //  CONFIRM(globalContext->bazaar.getItemListing(listing_id, &listing)) else break;
  //  if (listing.trade_listing.price > avatar_attributes_.money) break;
  //  if (listing.listing_character == myLoggedIntoCharacterID) { // withdraw instead of purchase
  //  unsigned int withdraw_listing_id[] = {listing.trade_listing.listing_id};
  //  globalContext->bazaar.withdrawSellerItems(this, withdraw_listing_id, 1);
  //  } else {

  //  // this could fail if person A buys the item that is on person B's screen
  //  if (globalContext->bazaar.buyItem(listing_id, myLoggedIntoCharacterID)) {
  //    changeMoney(-listing.trade_listing.price);
  //    unsigned short hash_index;
  //    Item* item = globalContext->itemManager.acquireStructForExistingItem(
  //      listing.item_id,
  //      listing.trade_listing.quantity,
  //      listing.trade_listing.type,
  //      &hash_index);
  //    ASSERT(item) else break;
  //    unsigned char inventory_index = 0;
  //    ASSERT(bestowItem(hash_index, item, &inventory_index));
  //    unsigned char indices[] = {inventory_index};
  //    reply_packet = Encode::updateInventory(myInventory, indices, 1);
  //  }

  //  // Erase this item from the client's bazaar listing
  //  unsigned int listing_id_reply[] = {listing.trade_listing.listing_id};
  //  sendWorldPacket(Encode::bazaar_removeItemListings(listing_id_reply,1));
  //  }

  //} break;

  ////----------------------------------------------------------------------------------------
  //case NETMSG_BAZAAR_GETSELLERSTATUS:
  //{
  //  CONFIRM(Decode::getSellerStatus(packet)) else break;

  //  unsigned int number_of_items;
  //  ItemForTrade items[Economy::MAX_ITEMS_PER_BAZAAR_PAGE];
  //  CONFIRM(globalContext->bazaar.getSellerStatus(myLoggedIntoCharacterID, items, &number_of_items)) else break;

  //  reply_packet = Encode::bazaar_tellSellerStatus(items, number_of_items);

  //} break;

  ////----------------------------------------------------------------------------------------
  //case NETMSG_BAZAAR_WITHDRAWSELLERITEMS:
  //{
  //  unsigned int number_of_items;
  //  unsigned int listing_ids[Economy::MAX_ITEMS_LISTED_PER_CHARACTER];
  //  CONFIRM(Decode::withdrawSellerItems(
  //    packet,
  //    listing_ids,
  //    Economy::MAX_ITEMS_LISTED_PER_CHARACTER,
  //    &number_of_items)) else break;
  //  if (!number_of_items) {
  //  CONFIRM(globalContext->bazaar.withdrawSellerProfits(this));
  //  } else {
  //  CONFIRM(globalContext->bazaar.withdrawSellerItems(this, listing_ids, number_of_items));
  //  }
  //  { // reply with a message telling the seller about what items are left
  //  unsigned int number_of_items;
  //  ItemForTrade items[Economy::MAX_ITEMS_PER_BAZAAR_PAGE];
  //  CONFIRM(globalContext->bazaar.getSellerStatus(myLoggedIntoCharacterID, items, &number_of_items)) else break;
  //  reply_packet = Encode::bazaar_tellSellerStatus(items, number_of_items);

  //  // Erase this item from the client's bazaar listing
  //  sendWorldPacket(Encode::bazaar_removeItemListings(listing_ids, number_of_items));
  //  }
  //} break;


  default:
    DEBUG_WARNING("Received invalid packet");
    break;
  }

  // If the reply packet is valid, send it to the connected peer
  if (reply_packet != 0) sendPacket(reply_packet);
}


void Client::leaveWorld() {
  if (state_ == LOGGED_INTO_CHARACTER) {

    StatisticsLogger::characterLoggedOut(account_id_,
                                         character_id_,
                                         avatar_attributes_.name);

    GlobalClientManager::singleton()->leftWorld(this);

    // Save the client's player information to the database
    commitToDatabase(GlobalDatabaseManager::singleton());

    // Free all of the items in the inventory
    GlobalItemManager* item_manager = GlobalItemManager::singleton();
    item_manager->releaseAvatarInventory(inventory_);

    magic_controller_.reset(NULL);
    magic_generator_.reset(NULL);
    enchantment_controller_.reset(NULL);

    Actor* actor = actor_.dereference();
    if (actor != NULL) {
      // Erase the actor
      GlobalActorManager* actor_manager = GlobalActorManager::singleton();
      WorldRegion* local_region = actor->getRegion();
      local_region->broadcastOrphanedNetworkPacket(actor->getDestroyActorPacket());
      actor_manager->releaseActor(&actor_);
      local_region->leaveWorld(this); // Remove client from the world
    }

    // Log out of the world and retreat to account
    state_ = LOGGED_INTO_ACCOUNT;
    character_id_ = 0;
  }
}



void Client::logoutAccount() {
  if (state_ == LOGGED_INTO_ACCOUNT) {

    active_accounts_.erase(account_id_);

    // Log out of the account
    state_ = CONNECTED;
    account_id_ = 0;
  }
}

void Client::changeHP(Actor* source, int amount) {
  Actor* actor = actor_.dereference();
  CONFIRM(actor) else return;

  // this code is complicated because it lets changeHP(0) fall through
  // and re-kill the client
  int hp = avatar_attributes_.hp;
  int max_hp = avatar_attributes_.current.max_hp;
  int hp_after = hp;
  if (hp > max_hp) {
    if (amount < 0) { // can only lose HP
      if (source != NULL && (source->inSameGroupAs(actor))) return; // group members can't damage us
      hp_after += amount;
      const MapLocationInfo* current_location_info = current_location_info_;
      if (current_location_info != NULL && // prevent hp loss on the safe zone
          current_location_info->navigability == MAPNAV_SAFEZONE) return;
    }
  } else if (hp > 0) { // don't change HP while dead
    if (amount > 0) {
      hp_after = hp + amount;
      hp_after = min(max_hp, hp_after);
    } else {
      if (source != NULL && (source->inSameGroupAs(actor))) return; // group members can't damage us
      const MapLocationInfo* current_location_info = current_location_info_;
      if ((amount != 0) &&
          current_location_info != NULL && // prevent hp loss on the safe zone
          current_location_info->navigability == MAPNAV_SAFEZONE) return;
      hp_after = hp + amount;
    }
  }

  // check to see if the client died
  if (hp_after <= 0) {
    avatar_attributes_.hp = 0;
    if (actor->isDead()) return;
    actor->setCorporealState(DEAD);
    actor->setCombatState(false);
    time_to_stop_moving_ = Time::current_frame + TIME_DEAD_BEFORE_RESPAWN;

    // reset the current state
    melee_combat_controller_.stop();
    magic_controller_.stop();
    magic_generator_.stop();
    enchantment_controller_.clear();

    // Force update
    actor->broadcastUpdateDescriptionPacket();

    // drop items
    WorldRegion* region = actor->getRegion();
    Map* map = NULL;
    if ((region != NULL) && ((map = region->getMap()) != NULL)) {
      int x, y;
      actor->getMapLocation(&x, &y);
      GlobalItemManager::singleton()->dropAvatarItemsOnDeath(map, x, y, inventory_);
      sendFullInventoryUpdate();
      buildCurrentAvatarAttributes(Time::current_frame);
      commitToDatabase(GlobalDatabaseManager::singleton());
    }

  } else {
    avatar_attributes_.hp = hp_after;
  }
}




void Client::changeMP(int amount) {
  int mp = avatar_attributes_.mp;
  int max_mp = avatar_attributes_.current.max_mp;
  int mp_after = mp;
  if (mp > max_mp) {
    if (amount < 0) { // can only lose MP
      mp_after += amount;
    }
  } else {
    mp_after = mp + amount;
    mp_after = min(max_mp, mp_after);
  }
  avatar_attributes_.mp = mp_after;
}


void Client::buildCurrentAvatarAttributes(double time) {
  Actor* actor = actor_.dereference();
  CONFIRM(actor) else return;

  // Does the actor's description need to be updated?
  bool update_actor_description = false;
  ActorTypeIndex old_actor_type = avatar_attributes_.current.actor_type;

  memcpy(&avatar_attributes_.current,
         &avatar_attributes_.base,
         sizeof(avatar_attributes_.current));

  SpellAvailabilityMask current_spell_availability_mask;
  memcpy(&current_spell_availability_mask,
         &avatar_attributes_.spell_availability,
         sizeof(current_spell_availability_mask));
  avatar_attributes_.spell_availability.zero();

  { // update enchantments
    CorporealState state = actor->isAlive() ? NORMAL : DEAD;
    update_actor_description = enchantment_controller_.apply(time, &avatar_attributes_.current, &state);
    actor->setCorporealState(state);
  }

  update_actor_description = reequipEquipment(&avatar_attributes_.current) || update_actor_description;
  update_actor_description = update_actor_description ||
                             old_actor_type != avatar_attributes_.current.actor_type;

  { // update the spell availability based on the current level
    // and preserving any additional spells granted by items
    GlobalGamefileManager* gamefile_manager =
        GlobalGamefileManager::singleton();
    avatar_attributes_.spell_availability.unionWith(
        gamefile_manager->getClassSpellAvailability(
            avatar_attributes_.classIndex,
            avatar_attributes_.current.level));

    // remove spells that aren't ever castable because the character can't have enough MP
    int max_mp = avatar_attributes_.current.max_mp;
    for (int i = 0; i < MAX_SPELL_TYPES; ++i) {
      if (current_spell_availability_mask.isBitSet(i)) {
        const SpellDescription* spell = gamefile_manager->getSpell(i);
        if (spell->mp_cost > max_mp) {
          current_spell_availability_mask.clearBit(i);
        }
      }
    }
  }

  { // Calculate the damage factors
    avatar_attributes_.physical_damage_factor = GameplayDesign::armorDamageFactor(avatar_attributes_.current.physical_defense);
    avatar_attributes_.magical_damage_factor = GameplayDesign::armorDamageFactor(avatar_attributes_.current.magical_defense);
  }

  {
    melee_combat_controller_.setActionDelay(avatar_attributes_.current.attack_delay_time);
  }

  // if we are only logging into the account, there is no need to broadcast
  // this information
  if (state_ == LOGGED_INTO_CHARACTER) {
    if (update_actor_description) {
      Actor* actor = actor_.dereference();
      CONFIRM(actor) else return;
      actor->broadcastUpdateDescriptionPacket();
    }
    if (0 != memcmp(&current_spell_availability_mask,
                    &avatar_attributes_.spell_availability,
                    sizeof(current_spell_availability_mask))) {
      using namespace Evidyon::Network::Packet::Server;
      NetworkPacket* packet = Encode::updateYourEffectAvailabilityMask(
        &avatar_attributes_.spell_availability);
      sendPacket(packet);
    }
    updateClientAvatarStats();
  }
}


bool Client::validateSpellParameters(Actor* client_actor,
                                     float x,
                                     float y,
                                     Actor* target_actor) {
  return ((target_actor == NULL) ||
      ((client_actor->distanceToSq(target_actor) < MAX_VISIBLE_DISTANCE_SQ)) &&
      (client_actor->distanceToSq(x, y) < MAX_VISIBLE_DISTANCE_SQ));
}


void Client::applySpecialFunctionOfCurrentLocation() {
  if (!current_location_info_) return;
  Actor* actor = actor_.dereference();
  ASSERT(actor != NULL) else return;
  WorldRegion* region = actor->getRegion();
  if (region == NULL) return;
  const MapLocationSpecialFunction* function = 
      region->getMap()->getSpecialFunction(
        current_location_info_->special_function_index);
  if (function == NULL) return;
  switch (function->type) {
    case MAP_LINK: {
      actor->stopMoving();
      client_request_mask_ = 0;
      time_to_stop_moving_ = 0.0;
      actor->changeMap(function->map_link.target_map,
                       function->map_link.x,
                       function->map_link.y);
      //DEBUG_INFO("Moving to map %i", function->map_link.target_map->getIDNumber());
    } break;

    case HP_POOL: {
      avatar_attributes_.hp = avatar_attributes_.current.max_hp;
    } break;

    case MP_POOL: {
      avatar_attributes_.mp = avatar_attributes_.current.max_mp;
    } break;
  }
}


bool Client::reequipEquipment(AvatarAttributes::Enchantable* avatar_attributes) {
  unsigned short old_equipment[ACTORATTACHMENTPOINT_COUNT];
  old_equipment[ACTORATTACHMENTPOINT_HELMET] = equipment_[ACTORATTACHMENTPOINT_HELMET];
  old_equipment[ACTORATTACHMENTPOINT_SHIELD] = equipment_[ACTORATTACHMENTPOINT_SHIELD];
  old_equipment[ACTORATTACHMENTPOINT_WEAPON] = equipment_[ACTORATTACHMENTPOINT_WEAPON];
  equipment_[ACTORATTACHMENTPOINT_HELMET] = (unsigned short)~0;
  equipment_[ACTORATTACHMENTPOINT_SHIELD] = (unsigned short)~0;
  equipment_[ACTORATTACHMENTPOINT_WEAPON] = (unsigned short)~0;

  // get this character's hair type
  equipment_[ACTORATTACHMENTPOINT_HELMET] =
    GlobalGamefileManager::singleton()->getHair(avatar_attributes_.raceIndex, avatar_attributes_.genderIndex, character_id_);

  ServerEquipmentSlotMask equipment;
  equipment.equipment_initialize();

  int physical_defense = 0;

  bool return_value = true;

  for (int i = 0; i < AVATAR_INVENTORY_SIZE; ++i) {
    if (false == inventory_[i].equipped) continue;

    Item* item = inventory_[i].item;
    const ServerItemDescription* description = item->getDescription();

    // Add to the mask
    bool valid = true;
    if (description->fills_multiple_slots) {
      valid = equipment.equipment_equipMultipleSlotItem(&description->equipment_slot_mask);
    } else {
      valid = equipment.equipment_equipSingleSlotItem(&description->equipment_slot_mask);
    }

    // handle whether or not it is equipped
    if (valid) {
      ActorAttachmentPoint point = description->equipped_attachment_point;
      switch (point) { // set the visual attachment of this item
        case ACTORATTACHMENTPOINT_WEAPON: {
          equipment_[ACTORATTACHMENTPOINT_WEAPON] = description->equipped_scenery_index;
        } break;

        case ACTORATTACHMENTPOINT_SHIELD: {
          equipment_[ACTORATTACHMENTPOINT_SHIELD] = description->equipped_scenery_index;
        } break;

        case ACTORATTACHMENTPOINT_HELMET: {
          equipment_[ACTORATTACHMENTPOINT_HELMET] = description->equipped_scenery_index;
        } break;
      }

      // add this item's attributes to the avatar
      unsigned int always_applied_modifiers =
          description->avatar_modifier.always_applied.value;
      unsigned int prop_modifiers =
          description->avatar_modifier.props.value;
      unsigned int item_props =
          item->getSpecialPropertyFlags();
      const CompiledItemAvatarModifier* modifier =
        &description->avatar_modifier;
      CompiledItemAvatarModifierMask::FieldType field_type =
        (CompiledItemAvatarModifierMask::FieldType)(0);
      do {
        bool is_special_property = (prop_modifiers & 1);
        bool apply = (always_applied_modifiers & 1) ||
                     (is_special_property && (item_props & 1));

        if (apply) {
          switch (field_type) {
            case CompiledItemAvatarModifierMask::MIN_ATTACK_DAMAGE: avatar_attributes->min_attack_damage    += modifier->min_attack_damage; break;
            case CompiledItemAvatarModifierMask::MAX_ATTACK_DAMAGE: avatar_attributes->random_attack_damage += modifier->max_attack_damage; break;
            case CompiledItemAvatarModifierMask::PHYSICAL_DEFENSE:  avatar_attributes->physical_defense += modifier->physical_defense; break;
            case CompiledItemAvatarModifierMask::MAGICAL_DEFENSE:   avatar_attributes->magical_defense  += modifier->magical_defense; break;
            case CompiledItemAvatarModifierMask::HP_REGEN_RATE:     avatar_attributes->hp_regen_rate += modifier->hp_regen_rate; break;
            case CompiledItemAvatarModifierMask::MP_REGEN_RATE:     avatar_attributes->mp_regen_rate += modifier->mp_regen_rate; break;
            case CompiledItemAvatarModifierMask::MAX_HP_MULTIPLIER: avatar_attributes->max_hp *= modifier->max_hp_multiplier; break;
            case CompiledItemAvatarModifierMask::MAX_MP_MULTIPLIER: avatar_attributes->max_mp *= modifier->max_mp_multiplier; break;
            case CompiledItemAvatarModifierMask::MAX_HP_ADDED:      avatar_attributes->max_hp += modifier->max_hp_added; break;
            case CompiledItemAvatarModifierMask::MAX_MP_ADDED:      avatar_attributes->max_mp += modifier->max_mp_added; break;
            case CompiledItemAvatarModifierMask::LEVEL:             avatar_attributes->level += modifier->level; break;
            case CompiledItemAvatarModifierMask::MOVEMENT_SPEED:    avatar_attributes->speed *= modifier->movement_speed_multiplier; break;
            case CompiledItemAvatarModifierMask::SPELL0:            avatar_attributes_.spell_availability.setBit(modifier->spells_to_make_available[0]); break;
            case CompiledItemAvatarModifierMask::SPELL1:            avatar_attributes_.spell_availability.setBit(modifier->spells_to_make_available[1]); break;
            case CompiledItemAvatarModifierMask::ATTACK_SPEED:      avatar_attributes->attack_delay_time *= modifier->attack_speed_multiplier; break;
            case CompiledItemAvatarModifierMask::ACTOR_TYPE:        avatar_attributes->actor_type =
                                                                    modifier->actor_indices[avatar_attributes_.genderIndex]
                                                                                           [avatar_attributes_.raceIndex]
                                                                                           [avatar_attributes_.classIndex];
                                                                    break;
            case CompiledItemAvatarModifierMask::STEAL_HP:          DEBUG_INFO("unimplemented property"); break;
            case CompiledItemAvatarModifierMask::STEAL_MP:          DEBUG_INFO("unimplemented property"); break;
            default:
              DEBUG_WARNING("Invalid item property field type");
              break;
          }
        }

        if (is_special_property) {
          item_props >>= 1;
        }

        // advance to the next field
        prop_modifiers >>= 1;
        always_applied_modifiers >>= 1;
        field_type =
          (CompiledItemAvatarModifierMask::FieldType)((int)field_type + 1);

      } while ((field_type < CompiledItemAvatarModifierMask::NUMBER_OF_FIELDS) &&   // there are fields left to be scanned, and either:
               ((always_applied_modifiers != 0) ||          // (1) there are std. attributes left to apply
                (prop_modifiers != 0 && item_props != 0))); // (2) there are props left to apply

    } else { // !valid
      inventory_[i].equipped = false;
    }
  }


  // Copy into the saved state
  equipment_slot_mask_.mask = equipment.mask;

  // did anything change?
  return 0 != memcmp(equipment_, old_equipment, sizeof(old_equipment));
}




void Client::rebuildBaseAvatarAttributes() {
  GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
  /*gamefile_manager->calculateAvatarAttributes(avatar_attributes_.classIndex,
                                              avatar_attributes_.base.level,
                                              avatar_attributes_.stats,
                                             &avatar_attributes_.base.max_hp,
                                             &avatar_attributes_.base.max_mp,
                                             &avatar_attributes_.base.hp_regen_rate,
                                             &avatar_attributes_.base.mp_regen_rate,
                                             &avatar_attributes_.base.speed,
                                             &avatar_attributes_.base.min_attack_damage,
                                             &avatar_attributes_.base.random_attack_damage);*/
  gamefile_manager->calculateCoreAvatarAttributes(&avatar_attributes_);

  // Regenerate attributes
  buildCurrentAvatarAttributes(Time::current_frame);
}


bool Client::validTargetOfEnemyAt(float x, float y) const {
  const Actor* actor = actor_.dereference();
  return (actor != NULL) &&
         (actor->isAlive()) &&
         (actor->distanceToSq(x, y) < MAX_VISIBLE_DISTANCE_SQ);
}

void Client::addRedFlag(const char* file, unsigned int line) {
  //do nothing atm
  //StatisticsLogger::
  DEBUG_INFO("WARNING:  client red flag at \"%s\"(%lu)", file, line);
}

void Client::updateClientAvatarStats() {
  CONFIRM(peer_) else return;
  using namespace Evidyon::Network::Packet::Server::Encode;
  unsigned char abilities[5] = {
    avatar_attributes_.stats[0],
    avatar_attributes_.stats[1],
    avatar_attributes_.stats[2],
    avatar_attributes_.stats[3],
    avatar_attributes_.stats[4],
  };
  int base_level_experience =
    GlobalGamefileManager::singleton()->getExperienceToAchieveLevel(avatar_attributes_.base.level);
  NetworkPacket* packet = tellYourAvatarStats(avatar_attributes_.current.level,
                                              abilities,
                                              avatar_attributes_.availableStatPoints,
                                              avatar_attributes_.hp,
                                              avatar_attributes_.current.max_hp,
                                              avatar_attributes_.mp,
                                              avatar_attributes_.current.max_mp,
                                              base_level_experience + avatar_attributes_.currentEXP,
                                              base_level_experience + avatar_attributes_.next_level_EXP,
                                              avatar_attributes_.current.physical_defense,
                                              (AvatarRace)avatar_attributes_.raceIndex,
                                              (AvatarClass)avatar_attributes_.classIndex);
  GlobalNetworkManager::sendAsyncPacket(peer_, packet);
}


void Client::storageOpen() {
  int number_of_items;
  CONFIRM(GlobalDatabaseManager::DBR_SUCCESS ==
          GlobalDatabaseManager::singleton()->storageGet(account_id_, storage_, &number_of_items)) else return;

  // build the storage
  ItemInStorage storage[Evidyon::MAX_ITEMS_IN_STORAGE];
  for (int i = 0; i < number_of_items; ++i) {
    storage[i].index = i;
    storage[i].quantity = storage_[i].quantity;
    storage[i].type = storage_[i].type;
  }

  // send the message
  using namespace Evidyon::Network::Packet::Server;
  sendPacket(Encode::storageTell(storage, number_of_items));
}


void Client::storageDeposit(int inventory_index) {
  if (inventory_index < 0 || inventory_index >= AVATAR_INVENTORY_SIZE) return;
  AvatarInventoryItem* inventory_item = &inventory_[inventory_index];
  Item* item = inventory_item->item;
  CONFIRM(item) else {
    addRedFlag(__FILE__, __LINE__);
    return;
  }
  if(GlobalDatabaseManager::DBR_SUCCESS !=
     GlobalDatabaseManager::singleton()->storageDeposit(account_id_, inventory_item)) {
    return;
  }

  // Copy down to fill gaps
  if (inventory_index + 1 < AVATAR_INVENTORY_SIZE) {
    memcpy(inventory_ + inventory_index,
           inventory_ + inventory_index + 1,
           sizeof(AvatarInventoryItem)*(AVATAR_INVENTORY_SIZE - inventory_index - 1));
  }
  inventory_[AVATAR_INVENTORY_SIZE-1].item = NULL;

  syncInventory(&inventory_index, 1);
  storageOpen();  // TODO: update storage more efficiently
}


void Client::storageWithdraw(int storage_index) {
  if (storage_index < 0 || storage_index >= MAX_ITEMS_IN_STORAGE) return;

  // find a location for this item
  int assigned_inventory_index = 0;
  if (!giveItem(NULL, 0, &assigned_inventory_index)) return;

  ServerStorageItem* storage_item = &storage_[storage_index];
  Item* item = NULL;
  CONFIRM(GlobalDatabaseManager::DBR_SUCCESS ==
          GlobalDatabaseManager::singleton()->storageWithdraw(account_id_,
                                                              storage_item->id,
                                                             &inventory_[assigned_inventory_index])) else {
    return;
  }

  syncInventory(&assigned_inventory_index, 1);
  storageOpen(); // TODO: update storage more efficiently
}

bool Client::changeGeonite(int amount) {
  int new_geonite = avatar_attributes_.geonite + amount;
  if (amount < 0 && new_geonite < 0) return false;
  avatar_attributes_.geonite = new_geonite;
  NetworkPacket* update_packet = Evidyon::Network::Packet::Server::Encode::geosidTellGeonite(new_geonite);
  sendAsyncPacket(update_packet);
  return true;
}



void Client::openActorGroup() {
  Actor* client_actor = actor_.dereference();
  CONFIRM(client_actor) else return;
  ActorGroup* group = client_actor->getGroup();
  if (group == NULL) {
    ActorGroup* group = ActorGroup::createGroup(client_actor); // groups default to open
  } else {
    group->setGroupOpen(true);
  }
}


void Client::closeActorGroup() {
  Actor* client_actor = actor_.dereference();
  CONFIRM(client_actor) else return;
  ActorGroup* group = client_actor->getGroup();
  if (group == NULL) return;
  group->setGroupOpen(false);
}


void Client::leaveActorGroup() {
  Actor* client_actor = actor_.dereference();
  CONFIRM(client_actor) else return;
  client_actor->setGroup(NULL);
}


bool Client::joinActorGroup(Actor* actor) {
  Actor* client_actor = actor_.dereference();
  CONFIRM(client_actor) else return false;
  ActorGroup* group = actor->getGroup();
  if (group == NULL) return false;
  return client_actor->setGroup(group);
}

}
