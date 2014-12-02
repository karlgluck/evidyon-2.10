//------------------------------------------------------------------------------------------------
// File:    dcxclock.cpp
//
// Desc:    Provides a generic interface for timing methods
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "../dcx/dcxclock.h"

// TODO: get rid of reliance on windows!!!!
#include <windows.h>


using namespace dcx;


//------------------------------------------------------------------------------------------------
// Name:  getWallTime
// Desc:  Gets the current wall time in seconds
//------------------------------------------------------------------------------------------------
double dcxClock::getWallTime() const
{
    return getSystemTime() / 1000.0;
}



//------------------------------------------------------------------------------------------------
// Name:  getSystemTime
// Desc:  Low-resolution, global method for obtaining a time value in milliseconds
//------------------------------------------------------------------------------------------------
unsigned long dcxClock::getSystemTime()
{
    return timeGetTime();
}

