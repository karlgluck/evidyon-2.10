//------------------------------------------------------------------------------------------------
// File:    volucrisclient.cpp
//
// Desc:    The Project Volucris client state machine
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"




//------------------------------------------------------------------------------------------------
// Name:  VolucrisClient
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
VolucrisClient::VolucrisClient() : myStateMachine(VCS_STARTUP), myWindowFont(25)
{
    myMainWindow = NULL;
    ZeroMemory(&myD3DParams, sizeof(myD3DParams));
    myDirect3D = NULL;
    myD3DDevice = NULL;
    myNextRenderingTime = 0.0;
    myENetClient = NULL;
    myENetServer = NULL;
    ZeroMemory(&myResources, sizeof(myResources));
    ZeroMemory(&myCharacter, sizeof(myCharacter));
    //myDirectSound = NULL;
    //ZeroMemory(mySoundBuffers, sizeof(mySoundBuffers));
    myUsingSoftwareDeviceFlag = false;
    myUsingMixedDeviceFlag = false;
    myTargetFPS = 0.0;
    time_of_day_adjustment_ = 0.0;
}



//------------------------------------------------------------------------------------------------
// Name:  execute
// Desc:  Runs the client application
//------------------------------------------------------------------------------------------------
void VolucrisClient::execute()
{
    // After startup, go to to the "connect" state
    myStateMachine.queueState(VCS_CONNECT);
    //myStateMachine.queueState(VCS_DEBUGSTATESANDBOX);

    // Execute until we run out of states
    while(myStateMachine.advance())
    {
        // Tell the debugger what's going on
        //DEBUG_INFO("Executing state \"%s\"", stateName(myStateMachine.currentState()));

        // Execute the state
        run(myStateMachine.currentState());
    }
}



//------------------------------------------------------------------------------------------------
// Name:  execute
// Desc:  Runs the client application
//------------------------------------------------------------------------------------------------
void VolucrisClient::handleWindowsMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool returnResult = false;
    myKeyboardInputSource.handleWindowsMessage(hWnd, uMsg, wParam, lParam, &returnResult);
    myMouseInputSource.handleWindowsMessage(hWnd, uMsg, wParam, lParam, &returnResult);
}



//------------------------------------------------------------------------------------------------
// Name:  cleanup
// Desc:  Shuts down all of the program components
//------------------------------------------------------------------------------------------------
void VolucrisClient::cleanup()
{
    global_sound_manager_.destroy();

    // Close the window and shut down
    if (myMainWindow)
    {
        CloseWindow(myMainWindow);
        DestroyWindow(myMainWindow);
        myMainWindow = NULL;
    }

    // Clear state variables
    global_actor_manager_.destroy();

    // Get rid of all of the resources
    freeResources();

    // Free the scenery renderer
    mySceneryRenderer.destroy();

    // Erase entries in the map chunk cache
    global_map_manager_.clearCache();

    //// Go through the sound buffer array and release everything
    //for (int i = 0; i < NUMBER_OF_SOUND_BUFFERS; ++i)
    //{
    //    if (mySoundBuffers[i])
    //    {
    //        mySoundBuffers[i]->Stop();
    //        mySoundBuffers[i]->Release();
    //        mySoundBuffers[i] = NULL;
    //    }
    //}

    // Free the DirectSound object
    //SAFE_RELEASE(myDirectSound);

    // Get rid of the server connection, if it still exists
    if (APP_WARNING(myENetServer != NULL)("Server was still connected when program closed"))
    {
        enet_peer_reset(myENetServer);
        myENetServer = NULL;
    }

    // Get rid of Direct3D
    SAFE_RELEASE(myD3DDevice);
    SAFE_RELEASE(myDirect3D);

    // Close the window and shut down
    if (myMainWindow)
    {
        DestroyWindow(myMainWindow);
        myMainWindow = NULL;
    }

    // Reset variables
    ZeroMemory(&myD3DParams, sizeof(myD3DParams));

    // Get rid of the client object
    if (myENetClient != NULL)
    {
        enet_host_destroy(myENetClient);
        myENetClient = NULL;
    }

    // Close the enet library
    enet_deinitialize();
}



//------------------------------------------------------------------------------------------------
// Name:  windowsMessagePump
// Desc:  Executes windows messages and returns when the next frame is to be run
//------------------------------------------------------------------------------------------------
bool VolucrisClient::windowsMessagePump()
{
    // Allow Windows to have some processing time
    //Sleep(0);

    // Run the message loop until it's empty
    MSG msg; 
    ZeroMemory(&msg, sizeof(msg));
    while(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // If we get a quit message, exit!
        if (APP_INFO(msg.message == WM_QUIT)("Quit message received")) return false;

        // Clear out the message structure
        ZeroMemory(&msg, sizeof(msg));
    }

    bool mainWindowExists = FALSE != IsWindow(myMainWindow);
    if (!mainWindowExists)
        DEBUG_INFO("Main window no longer exists");

    // Handle the next frame as long as the window still exists
    return mainWindowExists;
}




//------------------------------------------------------------------------------------------------
// Name:  allowFrameToBeRendered
// Desc:  Executes windows messages and returns when the next frame is to be run
//------------------------------------------------------------------------------------------------
bool VolucrisClient::allowFrameToBeRendered(double frameTime)
{
    double timeBeyondNextRender = frameTime - myNextRenderingTime;
    if (timeBeyondNextRender > 0.0)
    {
        static double lastRenderTime = frameTime - 1.0/myTargetFPS;
        double CURRENT_PERIOD = (frameTime - lastRenderTime);
        lastRenderTime = frameTime;

        float scaler = (1.0/myTargetFPS) / CURRENT_PERIOD;
        scaler = min(1.1f, scaler);
        scaler = max(0.5f, scaler);
        myConfiguredFramePeriod *= scaler;
        myNextRenderingTime = myNextRenderingTime + myConfiguredFramePeriod;
        float timeUntilNextRender = myNextRenderingTime - frameTime;

        // Don't allow the frame-rate to drop below 10 FPS
        if (timeUntilNextRender > 1.0f / 10.0f) myNextRenderingTime = frameTime + 1.0f / 10.0f;

        //static int frames = 0;
        //++frames;
        //static double lastZeroTime = frameTime;
        //if (frames == 60)
        //{
        //    DEBUG_INFO("%.02f FPS (1/configured period:  %.02f)", frames / (frameTime - lastZeroTime), 1.0f/myConfiguredFramePeriod);
        //    frames = 0;
        //    lastZeroTime = frameTime;
        //}

        //myNextRenderingTime = frameTime + myConfiguredFramePeriod;
       // myNextRenderingTime += myConfiguredFramePeriod;

        // Make sure the mouse is in the correct location
        updateMouseRenderingLocation();


        return true;
    }
    else
        return false;
}


//------------------------------------------------------------------------------------------------
void VolucrisClient::updateMouseRenderingLocation() {
  GUIPoint mouseLocation = myMouseInputSource.getMousePosition();
  mouseLocation = gui_primary_layer_.getPrimaryCanvas()->getArea()->constrain(mouseLocation);
  mouse_position_canvas_.alignXLeft(mouseLocation.x);
  mouse_position_canvas_.alignYTop(mouseLocation.y);
}

//------------------------------------------------------------------------------------------------
void VolucrisClient::setMouseCursorNormal() {
  mouse_cursor_canvas_.alignXLeft(0);
  mouse_cursor_canvas_.alignYTop(0);
  mouse_cursor_.setSourceRegion(&VolucrisGUIDefs::MOUSE_CURSOR_NORMAL, VolucrisGUIDefs::TEXTURE_SIZE);
}



//------------------------------------------------------------------------------------------------
void VolucrisClient::setMouseCursorActive() {
  mouse_cursor_canvas_.alignXLeft(-4);
  mouse_cursor_canvas_.alignYTop(-4);
  mouse_cursor_.setSourceRegion(&VolucrisGUIDefs::MOUSE_CURSOR_ACTIVE, VolucrisGUIDefs::TEXTURE_SIZE);
}



//------------------------------------------------------------------------------------------------
void VolucrisClient::renderLoginBackground() {
  global_rendering_manager_.resetCurrentTexture();
  gui_primary_layer_.reinitializeD3DDeviceState();

  GUIVertex* vertices;

  const float centers_sizes[] = {
    0.0f, 0.0f, 500.0f/1600.0f*myD3DParams.BackBufferWidth,
    400.0f/1600.0f*myD3DParams.BackBufferWidth,  400.0f/1200.0f*myD3DParams.BackBufferHeight, 1024.0f/1600.0f*myD3DParams.BackBufferWidth,
    600.0f/1600.0f*myD3DParams.BackBufferWidth,  300.0f/1200.0f*myD3DParams.BackBufferHeight, 800.0f/1600.0f*myD3DParams.BackBufferWidth,
    1000.0f/1600.0f*myD3DParams.BackBufferWidth, 700.0f/1200.0f*myD3DParams.BackBufferHeight, 1400.0f/1600.0f*myD3DParams.BackBufferWidth,
    -100.0f/1600.0f*myD3DParams.BackBufferWidth, 700.0f/1200.0f*myD3DParams.BackBufferHeight, 900.0f/1600.0f*myD3DParams.BackBufferWidth,
  };

  size_t quads = 5;
  gui_primary_layer_.allocateQuadListGeometry(&quads, &vertices);

  myD3DDevice->SetTexture(0, myResources.images.d3dTextureEvidyonSymbol);

  for (size_t q = 0; q < 5 && q < quads; ++q) {
    GUIQuad quad;
    quad.initialize(0.75f, D3DCOLOR_XRGB(255,255,255));
    quad.createRotated(centers_sizes[q*3+0],
                       centers_sizes[q*3+1],
                       centers_sizes[q*3+2],
                       GetTickCount()/(10000.0f * q * (q == 1 ? -1.0f : 1.0f)));
    quad.copyTo(vertices);
    vertices += 4;
  }

  gui_primary_layer_.flush();
}