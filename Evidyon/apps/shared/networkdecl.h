#ifndef __NETWORKDECL_H__
#define __NETWORKDECL_H__

// Instead of having to include the whole ENet header file,
// just include this file to declare the network types.
struct _ENetPacket;
typedef struct _ENetPacket ENetPacket;
struct _ENetPeer;
typedef struct _ENetPeer ENetPeer;
struct _ENetHost;
typedef struct _ENetHost ENetHost;

namespace Evidyon {
typedef ENetPacket NetworkPacket;
typedef ENetPeer NetworkPeer;
typedef ENetHost NetworkHost;
}

#endif