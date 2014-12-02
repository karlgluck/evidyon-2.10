#include "globalclientmanager.h"
#include "globalnetworkmanager.h"
#include "../common/null.h"
#include "worldregion.h"
#include "../shared/evidyon_network.h"
#include "clientconfig.h"


// This is necessary for slowsync flag grouping.  It would be nice to get rid
// of this dependancy.
#include "globalactormanager.h"

#include "globalgamefilemanager.h"


#include <math.h>
#include <dc/debug>

#include "uniqueavatarnamechecker.h"

namespace Evidyon {


GlobalClientManager* GlobalClientManager::singleton_ = NULL;

GlobalClientManager* GlobalClientManager::singleton() {
  return singleton_;
}


bool GlobalClientManager::create() {
  if (!client_memory_pool_.create()) return false;
  singleton_ = this;
  memset(clients_in_world_, 0, sizeof(clients_in_world_));
  number_of_clients_in_world_ = 0;
  next_slow_sync_group_ = 0;
  next_fast_sync_group_ = 0;
  next_fast_sync_id_ = 0;

  memset(last_slow_sync_times_, 0, sizeof(last_slow_sync_times_));
  memset(last_fast_sync_times_, 0, sizeof(last_fast_sync_times_));

  return true;
}

void GlobalClientManager::destroy() {
  client_memory_pool_.destroy();
  singleton_ = NULL;
}

Client* GlobalClientManager::acquire(NetworkPeer* peer) {
  Client* client = client_memory_pool_.acquire();
  if (client == NULL) return NULL;
  client->acquire(peer);
  return client;
}

void GlobalClientManager::wasDisconnected(Client* client) {
  client->release();
  client_memory_pool_.release(client);
}

void GlobalClientManager::enteredWorld(Client* client) {
  clients_in_world_[number_of_clients_in_world_++] = client;
}

void GlobalClientManager::leftWorld(Client* client) {
  const size_t number_of_clients_in_world = number_of_clients_in_world_;
  for (size_t i = 0; i < number_of_clients_in_world; ++i) {
    if (clients_in_world_[i] == client) {
      clients_in_world_[i] = clients_in_world_[--number_of_clients_in_world_];
      clients_in_world_[number_of_clients_in_world_] = NULL;
      return;
    }
  }
}



void GlobalClientManager::update(double time, double time_since_last_update) {
  using namespace ::Evidyon::ClientConfig;

  const size_t number_of_clients_in_world = number_of_clients_in_world_;
  for (size_t i = 0; i < number_of_clients_in_world; ++i) {
    Client* client = clients_in_world_[i];
    client->update(time,
             time_since_last_update);
  }

  { // slow sync
    float time_since_last_slow_sync = time - last_slow_sync_times_[next_slow_sync_group_];
    if (time_since_last_slow_sync > Network::SLOW_SYNC_PERIOD) {
      size_t slow_sync_group = next_slow_sync_group_;

      { // Change the actor sync group
        GlobalActorManager* actor_manager = GlobalActorManager::singleton();
        actor_manager->setSyncGroup(slow_sync_group);
      }

      for (size_t i = slow_sync_group; i < number_of_clients_in_world; i += SYNC_GROUPS) {
        Client* client = clients_in_world_[i];
        client->sendSlowSync(time_since_last_slow_sync);
      }
      last_slow_sync_times_[slow_sync_group] = time;
      next_slow_sync_group_ = (slow_sync_group + 1) % SYNC_GROUPS;
    }
  }

  { // fast sync
    float time_since_last_fast_sync = time - last_fast_sync_times_[next_fast_sync_group_];
    if (time_since_last_fast_sync > Network::FAST_SYNC_PERIOD) {
      size_t fast_sync_group = next_fast_sync_group_;
      for (size_t i = fast_sync_group; i < number_of_clients_in_world; i += SYNC_GROUPS) {
        Client* client = clients_in_world_[i];
        client->sendFastSync(time_since_last_fast_sync, next_fast_sync_id_);
      }
      last_fast_sync_times_[fast_sync_group] = time;
      next_fast_sync_id_ += fast_sync_group == 0 ? 1 : 0;
      next_fast_sync_group_ = (fast_sync_group + 1) % SYNC_GROUPS;
    }
  }
}


bool GlobalClientManager::accountIsActive(unsigned int account_id) {
  for (ClientMemoryPool::Iterator i(&client_memory_pool_);
                  i.hasNext();
                  i.advance()) {
  Client* client = i.get();
  if (client->getLoggedIntoAccountID() == account_id)
    return true;
  }

  return false;
}


void GlobalClientManager::respawnAll() {
  GlobalGamefileManager* gamefile_manager = GlobalGamefileManager::singleton();
  float x, y;
  gamefile_manager->getPlayerRespawnLocation(&x, &y);

  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client = clients_in_world_[i];
    client->moveTo(0, x, y);
  }
}

void GlobalClientManager::disconnectAll() {
  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client = clients_in_world_[i];
    client->sendDisconnectionPacket();
  }

  GlobalNetworkManager* network_manager = GlobalNetworkManager::singleton();
  int time_waiting = 0;
  static const int MAXIMUM_MS_TO_WAIT = 5000;
  static const int SLEEP_MS_PER_CYCLE = 100;
  while ((false == client_memory_pool_.empty()) &&
         (time_waiting++ < MAXIMUM_MS_TO_WAIT/SLEEP_MS_PER_CYCLE)) {
    Sleep(SLEEP_MS_PER_CYCLE);
    network_manager->updateDisconnectionsOnly(this);
  }

  // Everyone should now be disconnected
  CONFIRM(client_memory_pool_.empty());
  CONFIRM(number_of_clients_in_world_ == 0);
}


bool GlobalClientManager::getCoordinatesOf(const char* avatar_name, Map** map, float* x, float* y) {
  std::string reduced_name;
  UniqueAvatarNameChecker::reduceName(avatar_name, &reduced_name);
  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    const Client* client = clients_in_world_[i];
    if (client->loggedIntoAvatar(reduced_name)) {
      const Actor* actor = client->getActor()->dereference();
      CONFIRM(actor) else return false;
      Map* actor_map = actor->getMap();
      if (!actor_map) return false;
      actor->getPosition(x, y);
      *map = actor_map;
      return true;
    }
  }
  return false;
}

bool GlobalClientManager::getClientOnAccount(unsigned int account_id, Client** client) {
  for (size_t i = 0; i < number_of_clients_in_world_; ++i) {
    Client* client_in_world = clients_in_world_[i];
    if (client_in_world->getLoggedIntoAccountID() == account_id) {
      *client = client_in_world;
      return true;
    }
  }
  return false;
}

}