//------------------------------------------------------------------------------------------------
// File:    states.cpp
//
// Desc:    The Project Volucris client states
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"



//------------------------------------------------------------------------------------------------
// Name:  stateName
// Desc:  Gets the name of the address of a particular state
//------------------------------------------------------------------------------------------------
const char* VolucrisClient::stateName(State state)
{
    switch(state)
    {
        case VCS_STARTUP:       return "Startup";
        case VCS_CONNECT:       return "Connect";
        case VCS_ACCOUNT_LOGIN: return "Account Login";
//        case VCS_CREATE_ACCOUNT:return "Create Account";
        case VCS_LOGGED_INTO_ACCOUNT:   return "Logged Into Account";
        case VCS_WORLD_MAIN:    return "World - Main";
        case VCS_NO_CONNECTION: return "No Connection";
        case VCS_DISCONNECT:    return "Disconnect";
        case VCS_SHUTDOWN:      return "Shutdown";
        case VCS_CREATE_CHARACTER: return "Create Character";
        case VCS_LOST_D3DDEVICE: return "Lost D3D Device";

        case VCS_DEBUGSTATESANDBOX: return "DEBUG: sandbox";
    }

    // Who knows what happened!
    return "Unknown";
}

//------------------------------------------------------------------------------------------------
// Name:  run
// Desc:  Executes the state with the given address
//------------------------------------------------------------------------------------------------
void VolucrisClient::run(State state)
{
    switch(state)
    {
        case VCS_STARTUP:       stateStartup(); break;
        case VCS_CONNECT:       stateConnect(); break;
        case VCS_ACCOUNT_LOGIN: stateAccountLogin(); break;
//        case VCS_CREATE_ACCOUNT:stateCreateAccount(); break;
        case VCS_LOGGED_INTO_ACCOUNT:   stateLoggedIntoAccount(); break;
        case VCS_WORLD_MAIN:    stateWorldMain(); break;
        case VCS_NO_CONNECTION: stateNoConnection(); break;
        case VCS_DISCONNECT:    stateDisconnect(); break;
        case VCS_SHUTDOWN:      stateShutdown(); break;
        case VCS_CREATE_CHARACTER: stateCreateCharacter(); break;
        case VCS_LOST_D3DDEVICE: stateLostD3DDevice(); break;

        // Testing-only states
        case VCS_DEBUGSTATESANDBOX:   debugStateSandbox(); break;

        // Output a warning if we don't know what the state is
        default:
            DEBUG_ERROR("Tried to execute unknown state:  %i (%s)", (int)state, stateName(state));
            break;
    }
}