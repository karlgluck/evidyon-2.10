#ifndef __GLOBALNETWORKMANAGER_H__
#define __GLOBALNETWORKMANAGER_H__

#include "client.h"
#include "../shared/networkdecl.h"


namespace Evidyon {

namespace Network {
static unsigned __int8 NORMAL_PACKET_CHANNEL = 0;
static unsigned __int8 ASYNC_PACKET_CHANNEL = 1;
}

class GlobalClientManager;

class GlobalNetworkManager {
public:
  static GlobalNetworkManager* singleton();

public:
  bool create(size_t max_clients, unsigned short port);
  void destroy();

  // Handles all incoming network messages
  void update(GlobalClientManager* client_manager);

  void broadcast(NetworkPacket* packet);
  void broadcastOrphanedPacket(NetworkPacket* packet);

  // Processes incoming network messages, but only allows disconnections
  // to occur.  All other message types are ignored.  This is used when
  // the server is shutting down.
  void updateDisconnectionsOnly(GlobalClientManager* client_manager);

public:
  static void trimPacket(NetworkPacket* packet, int new_byte_length);
  static void sendPacket(Client* client, NetworkPacket* packet);
  static void sendPacket(NetworkPeer* peer, NetworkPacket* packet);
  static void sendAsyncPacket(Client* client, NetworkPacket* packet);
  static void sendAsyncPacket(NetworkPeer* peer, NetworkPacket* packet);
  static void handleOrphanedPacket(NetworkPacket* packet);

private:
  NetworkHost* host_;

private:
  static GlobalNetworkManager* singleton_;
};


}

#endif