//------------------------------------------------------------------------------------------------
// File:    stateconnect.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"




//------------------------------------------------------------------------------------------------
// Name:  stateConnect
// Desc:  
//------------------------------------------------------------------------------------------------
void VolucrisClient::stateLostD3DDevice()
{
    // Get rid of everything
    global_actor_manager_.onLostDevice();
    global_rendering_manager_.resetCurrentTexture();
    freeResources();
    mySceneryRenderer.destroy();
    global_map_manager_.clearCache();
    global_sound_manager_.destroy();

    // Wait for a connection reply from the server
    HRESULT hr;
    while(D3DERR_DEVICELOST == (hr = myD3DDevice->TestCooperativeLevel()))
    {
        if (!windowsMessagePump())
        {
            myStateMachine.jumpState(VCS_SHUTDOWN);
            return;
        }

        // Poll the connection to keep it alive.  Watch for disconnection.
        ENetEvent netEvent;
        while(enet_host_service(myENetClient, &netEvent, 0) > 0) {
            if (netEvent.type == ENET_EVENT_TYPE_DISCONNECT) {
                myStateMachine.jumpState(VCS_SHUTDOWN);
                return;
            }
            // Free packet memory
            if (netEvent.packet) enet_packet_destroy(netEvent.packet);
        }

        // Don't overload the program
        Sleep(200);
    }

    CONFIRM(hr == D3DERR_DEVICENOTRESET && SUCCEEDED(myD3DDevice->Reset(&myD3DParams))) else {
        myStateMachine.jumpState(VCS_SHUTDOWN);
        return;
    }

    // Reset everything
    if (APP_FATAL(!acquireResources())("The game couldn't load its resource file")) {
        myStateMachine.jumpState(VCS_DISCONNECT);
        myStateMachine.jumpState(VCS_SHUTDOWN);
        return;
    }

    mySceneryRenderer.create(myD3DDevice, myUsingSoftwareDeviceFlag);
    global_sound_manager_.create(myMainWindow);
}