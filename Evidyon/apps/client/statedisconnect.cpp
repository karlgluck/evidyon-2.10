//------------------------------------------------------------------------------------------------
// File:    statedisconnect.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"




//------------------------------------------------------------------------------------------------
// Name:  stateDisconnect
// Desc:  
//------------------------------------------------------------------------------------------------
void VolucrisClient::stateDisconnect()
{
using namespace ::Evidyon::Network::Packet;
using namespace ::Evidyon::Network::Packet::Client;
    // We only need to run this state if the server is still valid
    if (!myENetServer) return;

    // Holds the event pulled by the ENet library
    ENetEvent netEvent;

    // Disconnect from the server
    enet_peer_disconnect(myENetServer, 0);

    // Allow up to 3 seconds for the disconnect to succeed, and drop any packets
    // that are received in the mean time
    bool disconnected = false;
    while(!disconnected && (enet_host_service(myENetClient, &netEvent, 3000) > 0))
    {
        switch(netEvent.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(netEvent.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                disconnected = true;
                //DEBUG_INFO("Disconnection succeeded");
                break;
        }
    }

    // The disconnect attempt hasn't succeeded yet, so force the connection to close
    if (!disconnected)
        enet_peer_reset(myENetServer);

    // Exit
    myENetServer = NULL;
}