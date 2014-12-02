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
void VolucrisClient::stateConnect()
{
using namespace ::Evidyon::Network::Packet;
using namespace ::Evidyon::Network::Packet::Client;

  // Get the list of server IPs
  const char* serverAddress = myConfig.getSetting("Network", "Server1", "66.90.87.83");

  ENetAddress netAddress;
  ENetEvent netEvent;

  // Connect to the server
  enet_address_set_host(&netAddress, serverAddress);
  netAddress.port = Evidyon::Network::SERVER_PORT;

  Sleep(150); // Wait a little bit (just incase the server is restarting, so we don't get in by accident)

  // Initiate the connection with the remote host
  if (APP_ERROR(NULL == (myENetServer = enet_host_connect(myENetClient, &netAddress, 2)))
          ("No available peers for initiating an ENet connection"))
  {
    // Exit
    myStateMachine.jumpState(VCS_SHUTDOWN);
    return;
  }

  // Display the main window
  ShowWindow(myMainWindow, SW_SHOWDEFAULT);
  UpdateWindow(myMainWindow);

  // Clear the screen
  GUIFont font(VolucrisGUIDefs::WINDOW_FONT_TEXTURE_INDEX);
  if (APP_ERROR(!font.create(myD3DDevice, "Trebuchet MS", 12))("Couldn't create font")) return;
  gui_primary_layer_.setTexture(font.getTextureID(), font.getTexture());


  VolucrisGUIButton quit;
  {
    quit.setFont(&font);
    quit.setText("Exit");
    quit.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    quit.getCanvas()->alignXLeft(+10);
    quit.getCanvas()->alignYBottom(-10);
    quit.getCanvas()->absoluteHeight(35);
    quit.getCanvas()->absoluteWidth(200);
    quit.getCanvas()->absoluteDepth(0.5f);
    quit.registerElements(&gui_primary_layer_);
    quit.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());
    quit.enable();
  }
  ////////////////=======================///////////////////////

  int currentStatBarWidth[4] = { 256, 512, 512, 768 };
  float currentStatBarPosition[4] = { 128, -312, 560, 300 };
  const float statBarSpeed[4] = { 30, 80, 120, -70 };

  // If we fail to connect to the server, go to the correct state
  myStateMachine.jumpState(VCS_SHUTDOWN);

  // Wait for a connection reply from the server
  dcxWin32Clock clock;
  double originalStartTime = clock.getAccurateSystemTime();
  double startTime = clock.getAccurateSystemTime() - 10.0, currentTime = startTime, lastTime = startTime;
  while(windowsMessagePump() && !quit.wasPushed())
  {
    // Get the current time
    currentTime = clock.getAccurateSystemTime();
    int number_of_dots = (int)floorf(2.0f*(float)(currentTime - originalStartTime));

    // Check to see if we got a reply
    if ((enet_host_service(myENetClient, &netEvent, 0) > 0) &&
        netEvent.type == ENET_EVENT_TYPE_CONNECT) {

      // Go to the next state
      myStateMachine.jumpState(VCS_ACCOUNT_LOGIN);

      // Exit, and proceed to the next state
      break;

    } else if (currentTime - startTime > 2.0) {
      // Try connecting again
      enet_peer_reset(myENetServer);
      if (APP_ERROR(NULL == (myENetServer = enet_host_connect(myENetClient, &netAddress, 2)))
          ("No available peers for initiating an ENet connection")) {
        // Exit
        myStateMachine.jumpState(VCS_SHUTDOWN);
        return;
      }
      startTime = currentTime;
    }

    if (allowFrameToBeRendered(dcxWin32Clock::getAccurateSystemTime())) {
      // Clear the backbuffer and the zbuffer
      myD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                              D3DCOLOR_XRGB(93, 109, 82), 1.0f, 0);


      // Begin the scene
      if (SUCCEEDED(myD3DDevice->BeginScene())) {
        CONFIRM(gui_primary_layer_.render());

        renderLoginBackground();

        global_rendering_manager_.changeCurrentD3DStage0Texture(myWindowFont.getTexture());
        myWindowFont.renderText(&gui_primary_layer_,
                                NULL,
                                50.0f,
                                30.0f,
                                0.1f,
                                "connecting...",
                                -1,
                                D3DCOLOR_XRGB(255,255,255));
        gui_primary_layer_.flush();

        // End the scene
        myD3DDevice->EndScene();
      }

      // Present the backbuffer contents to the display
      myD3DDevice->Present(NULL, NULL, NULL, NULL);
    }
  }

  quit.unregisterElements(&gui_primary_layer_);
  gui_primary_layer_.removeTexture(font.getTextureID());
  font.destroy();
}