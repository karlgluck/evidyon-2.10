//------------------------------------------------------------------------------------------------
// File:    stateshutdown.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"




//------------------------------------------------------------------------------------------------
// Name:  stateShutdown
// Desc:  
//------------------------------------------------------------------------------------------------
void VolucrisClient::stateShutdown()
{
    // Make sure the state machine doesn't try to invoke any more operations
    myStateMachine.clear();

    // Erase all of the used objects in the application
    cleanup();
}