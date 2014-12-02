#ifndef __GLOBALCLIENTMANAGER_H__
#define __GLOBALCLIENTMANAGER_H__

#include <dcx/memorypool>
#include "../shared/networkdecl.h"
#include <vector>
#include "clientconfig.h"

namespace Evidyon {

class Client;
class WorldRegion;
class Map;


class GlobalClientManager {
public:
  static const int MAX_CLIENTS = 64;

private:
  typedef dcx::dcxMemoryPool<Client, MAX_CLIENTS> ClientMemoryPool;
  typedef std::vector<Client*> ClientsInWorld;

public:
  static GlobalClientManager* singleton();

public:

  // Sets up the manager class
  bool create();
  void destroy();
  Client* acquire(NetworkPeer* peer);
  void wasDisconnected(Client* client);
  void enteredWorld(Client* client);
  void leftWorld(Client* client);
  void update(double time,
              double time_since_last_update);
  bool accountIsActive(unsigned int account_id);


public: // admin commands and utility methods
  void respawnAll();
  void disconnectAll();
  bool getCoordinatesOf(const char* avatar_name, Map** map, float* x, float* y);
  bool getClientOnAccount(unsigned int account_id, Client** client);

private:
  ClientMemoryPool client_memory_pool_;

  Client* clients_in_world_[MAX_CLIENTS];
  size_t number_of_clients_in_world_;

  size_t next_slow_sync_group_;
  double last_slow_sync_times_[ClientConfig::SYNC_GROUPS];
  size_t next_fast_sync_group_;
  double last_fast_sync_times_[ClientConfig::SYNC_GROUPS];
  int next_fast_sync_id_;

private:
  static GlobalClientManager* singleton_;
};

}

#endif