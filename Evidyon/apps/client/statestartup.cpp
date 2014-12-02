//------------------------------------------------------------------------------------------------
// File:    statestartup.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include <direct.h>




//------------------------------------------------------------------------------------------------
// Name:  stateStartup
// Desc:  
//------------------------------------------------------------------------------------------------
void VolucrisClient::stateStartup()
{
    // Print the CWD
    {
        char currentDirectory[512];
        GetCurrentDirectory(512, currentDirectory);
        DEBUG_INFO("win32-cwd:  %s", currentDirectory);
        _getcwd(currentDirectory, 512);
        DEBUG_INFO("subsys-cwd: %s", currentDirectory);
    }

    // Load the settings file
    APP_INFO(!myConfig.read("config.ini"))("Config file not found");

    // Do all of the initialization process
    if (APP_FATAL(!acquireGraphics())("Unable to initialize graphics") ||
        APP_FATAL(!acquireInput())("Unable to initialize input") ||
        APP_FATAL(!acquireAudio())("Audio subsystem didn't initialize") ||
        APP_FATAL(!acquireNetwork())("Unable to initialize network") ||
        APP_FATAL(!acquireSystem())("System couldn't initialize correctly") ||
        APP_FATAL(!acquireResources())("The game couldn't load its resource file"))
    {
        // Display error message
        MessageBox(NULL, "Failed to initialize; check debug log for more info", "Error", MB_OK);

        // Exit
        myStateMachine.jumpState(VCS_SHUTDOWN);
    }
    else
    {
        // Move to the next state (determined by the calling method!)
        //myStateMachine.jumpState(VCS_CONNECT);
        //myStateMachine.jumpState(VCS_SHUTDOWN);
        //myStateMachine.jumpState(VCS_DEBUGSTATESANDBOX);
    }
}