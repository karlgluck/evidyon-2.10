//------------------------------------------------------------------------------------------------
// File:  debugstaterendermap.cpp
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "../common/unseenskinnedmesh/unseenskinnedmesh.h"
#include <dc/filestream>
#include "../common/gui/guicanvasrowarray.h"
#include "../common/gui/guiscrollablelist.h"

//------------------------------------------------------------------------------------------------
// Name:  debugStateRenderMap
// Desc:  
//------------------------------------------------------------------------------------------------
void VolucrisClient::debugStateSandbox()
{
  // Display the main window
  ShowWindow(myMainWindow, SW_SHOWDEFAULT);
  UpdateWindow(myMainWindow);

  myStateMachine.queueState(VCS_SHUTDOWN);

}