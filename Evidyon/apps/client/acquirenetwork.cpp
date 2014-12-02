//------------------------------------------------------------------------------------------------
// File:  acquirenetwork.cpp
//
// Desc:  Initializes the network component of PV
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"





//------------------------------------------------------------------------------------------------
// Name: acquireNetwork
// Desc: 
//------------------------------------------------------------------------------------------------
bool VolucrisClient::acquireNetwork()
{
  // Load the ENet library
  if (APP_FATAL(0 > enet_initialize())("Unable to initialize the network library"))
    return false;

  // This creates a client "host" object that has one outgoing connection and no static
  // downstream/upstream bandwidth limitaion.  This causes the ENet library to rely on
  // its dynamic throttling algorithm alone.
  myENetClient = enet_host_create(NULL, 1, 0, 0);

  // Success
  return true;
}