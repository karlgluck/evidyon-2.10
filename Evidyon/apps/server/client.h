#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "../shared/networkdecl.h"
#include "avatarattributes.h"
#include "../shared/evidyon_avatarinventory.h"
#include "actorcontroller.h"
#include "actorpointer.h"
#include "magiccontroller.h"
#include "magicgenerator.h"
#include "./packets/slowsync-encoder.h"
#include "clientenchantmentcontroller.h"
#include "meleecombatcontroller.h"
#include "avatarinventoryitem.h"

// this is necessary for the ServerEquipmentSlotMask struct
#include "../shared/server-editor/server_gamefile.h"

#include "../shared/actorid.h"
#include "serverstorage.h"

#include <set>

namespace Evidyon {

class GlobalDatabaseManager;
class Actor;
struct SpellDescription;
struct MapLocationInfo;




// TODO: make storage load on login, like inventory
// also, make storage per-character so that if
// multiple chars log in from same acct, items can't be duped.


class Client : public ActorController {
  enum State {
    INVALID,
    CONNECTED,
    CONNECTED_VERSIONCONFIRMED,
    LOGGED_INTO_ACCOUNT,
    ENTERING_WORLD, // between account and character states, waiting for client
    LOGGED_INTO_CHARACTER,
  };

  enum ClientRequest {
    REQUEST_MOVE,
    REQUEST_SPELL,
    REQUEST_USE_ITEM,
  };

  struct ClientRequestMove {
    float x, y;
    bool streaming;
    ActorID attack_target_actor_id;
  };

  struct ClientRequestSpell {
    int spell_id;
    ActorID actor_id;
    float x, y;
  };

  struct ClientRequestUseItem {
    unsigned int inventory_index;
    ActorID actor_id;
    float x, y;
  };


public:
  Client();

  void acquire(NetworkPeer* peer);
  void release();

  void sendDisconnectionPacket();

  void update(double time, double time_since_last_update);
  void sendSlowSync(float time_since_last_sync);
  void sendFastSync(float time_since_last_sync, int sync_id);

  void handleNetworkPacket(NetworkPacket* packet);

  void sendPacket(NetworkPacket* packet);
  void sendAsyncPacket(NetworkPacket* packet);

  void changeEXP(int amount);

  void changeMoney(int amount);


  // Adds an item to this client's inventory.  If this method returns 'true',
  // the item was added successfully, and the calling method should call
  // syncInventory with *assigned_inventory_index as one of the locations
  // to update.
  // Note: it is valid to pass NULL for an item!  This will not affect the
  // inventory, but it will return 'true' if there is space to hold the
  // item and *assigned_inventory_index will hold a valid location at
  // which it can be placed.
  bool giveItem(Item* item,
                int start_search_index,
                int* assigned_inventory_index);

  // Looks through the given inventory indices and compares them with other
  // entries in the inventory to see if they can be stacked.
  // Sends an update to the client defining the state of all affected indices.
  void syncInventory(int* inventory_indices,
                     int number_of_indices);

// administrative commands
public:
  void moveTo(int map, float x, float y);
  bool loggedIntoAvatar(const std::string& reduced_name) const;

private:

  // Sends an inventory update message to the client.  This method has no effect if the
  // client isn't logged into the game world.
  void sendPartialInventoryUpdate(int* indices,
                                  int number_of_indices);
  void sendFullInventoryUpdate();


public:
  virtual bool isClient() const;
  // Implementation of ActorController::applyMagicEffect
  virtual void applyMagicEffect(
      Actor* source,
      double time, const Magic* magic,
      const MagicDescriptionStageEffect* effect,
      const MagicDescriptionStageEffect::Parameters* effect_parameters);

  // Implementation of ActorController::instantiateMagic
  virtual bool instantiateMagic(int source, const Magic* magic,
    int* times_to_generate, double* motion_delay, double* action_delay,
    int* visualization_power_parameter,
    MagicDescriptionStageParameters stage_parameters[MagicDescription::NUM_STAGES]);

  virtual Client* getClient() const;
  virtual void fillStateDescription(ActorStateDescriptionData* state);
  virtual const char* getName();
  virtual int instantiateMeleeAttack(Actor* target,
                                     MeleeAttack* attack);
  virtual bool block(Actor* attacker,
                     const MeleeAttack* attack);
  virtual void applyMeleeAttack(Actor* attacker,
                                MeleeAttack* attack);


public:
  inline NetworkPeer* getPeer();
  inline bool isValid() const;
  inline unsigned int getLoggedIntoAccountID() const;
  inline unsigned int getLoggedIntoCharacterID() const;
  inline void addToSlowSyncPacket(Evidyon::Network::Packet::ActorSyncData* actor_data);
  inline void dispatchSlowSyncPacket();
  inline const ActorPointer* getActor() const;


public:
  bool validTargetOfEnemyAt(float x, float y) const; // used by enemies searching for targets


private:
  void zero();
  void commitToDatabase(GlobalDatabaseManager* database);

  void handleNetworkPacketWhenConnected(NetworkPacket* packet);
  void handleNetworkPacketWhenLoggedIntoAccount(NetworkPacket* packet);
  void handleNetworkPacketWhenEnteringWorld(NetworkPacket* packet);
  void handleNetworkPacketWhenLoggedIntoCharacter(NetworkPacket* packet);

  inline void setRequest(ClientRequest type);

  void leaveWorld();
  void logoutAccount();

  // Handles death and max/min values
  void changeHP(Actor* source, int amount);
  void changeMP(int amount);

  // Applies enchantment and item effects to fill out the avatar's current
  // state (see AvatarAttributes::Enchantable).
  void buildCurrentAvatarAttributes(double time);

  // Called when casting a spell that the client wants to use to make sure
  // that the parameters don't violate any basic rules:  the biggest one is
  // that you can't target beyond your visible radius.
  // client_actor MUST be valid
  bool validateSpellParameters(Actor* client_actor,
                               float x,
                               float y,
                               Actor* target_actor);

  void applySpecialFunctionOfCurrentLocation();

  // Returns 'true' if the avatar's description needs to be updated.  Should
  // ONLY be called by buildCurrentAvatarAttributes (it assumes some state
  // variables that are set therein)
  bool reequipEquipment(AvatarAttributes::Enchantable* avatar_attributes);

  // sends the client's current stats
  void updateClientAvatarStats();

  // Rebuilds the avatar's HP/MP/speed and other attributes from those defined
  // in by the game file.  This should be called after adding a stat point,
  // gaining a level, etc.
  // This method will invoke buildCurrentAvatarAttributes.
  void rebuildBaseAvatarAttributes();

private:
  // These methods manage the client's storage.  They will send messages to
  // the client to update inventory and storage.
  void storageOpen();
  void storageDeposit(int inventory_index);
  void storageWithdraw(int storage_index);

private:
  // Always returns 'true' if amount > 0.  If amount < 0, this method
  // returns 'true' if adding amount won't cause the player's geonite to
  // become negative.  This method updates the remote client.
  bool changeGeonite(int amount);

private:
  void openActorGroup();
  void closeActorGroup();
  void leaveActorGroup();
  bool joinActorGroup(Actor* actor);

private:

  // This client has attempted to do something invalid.  This method is called
  // every time the client passes information that fails sanity/validity checks
  // of what is expected from the client.  It can be used to identify potential
  // hackers.
  void addRedFlag(const char* file, unsigned int line);


private:
  State state_;
  NetworkPeer* peer_;
  unsigned int account_id_, character_id_;
  AvatarAttributes avatar_attributes_;

  ActorPointer actor_; // Represents this client in the world

  const MapLocationInfo* current_location_info_;
  unsigned int client_request_mask_; // Requests that are currently valid
  ClientRequestMove client_request_move_;
  ClientRequestSpell client_request_spell_;
  ClientRequestUseItem client_request_use_item_;

  // the time at which the client will arrive at its target
  // also: if the character is dead, this is the respawn time
  double time_to_stop_moving_;

  // This buffer holds the next all-actor sync packet that is going
  // to be sent to the client
  Evidyon::Network::Packet::SlowSyncPacketEncoder slow_sync_encoder_;

  MagicController magic_controller_;
  MagicGenerator magic_generator_;

  ClientEnchantmentController enchantment_controller_;

  MeleeCombatController melee_combat_controller_;

  // The items that this client is carrying
  AvatarInventoryItem inventory_[AVATAR_INVENTORY_SIZE];
  unsigned short equipment_[ACTORATTACHMENTPOINT_COUNT];
  ServerEquipmentSlotMask equipment_slot_mask_;

  // When the client asks to open his/her storage, this is filled out
  ServerStorageItem storage_[MAX_ITEMS_IN_STORAGE];

  // The last location the player swapped to with the geosid
  float geosid_swap_x, geosid_swap_y;
  int geosid_swap_map;


private:
  static std::set<unsigned int> active_accounts_;
};

#include "client_inline.h"


}


#endif