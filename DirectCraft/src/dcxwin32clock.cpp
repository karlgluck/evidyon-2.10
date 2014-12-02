//------------------------------------------------------------------------------------------------
// File:    dcxwin32clock.h
//
// Desc:    Provides a high-precision timer for the Win32 environment, using the
//          QueryPerformanceFrequency/QueryPerformanceTimer methods, if they are available.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include <windows.h>
#include "../dcx/dcxclock.h"
#include "../dcx/dcxwin32clock.h"
#include "../dc/debug"


using namespace dcx;

// Resolve the frequency
double dcx::dcxWin32Clock::theQPFrequency = -1.0;
LARGE_INTEGER dcx::dcxWin32Clock::theStartTime = { 0 };


//------------------------------------------------------------------------------------------------
// Name:  getWallTime
// Desc:  Gets the current wall time using the preferred method of the implementing timer
//------------------------------------------------------------------------------------------------
double dcxWin32Clock::getWallTime() const
{
    return getAccurateSystemTime();
}


//------------------------------------------------------------------------------------------------
// Name:  getAccurateSystemTime
// Desc:  High-resolution (if available) global method for obtaining a time value
//------------------------------------------------------------------------------------------------
double dcxWin32Clock::getAccurateSystemTime()
{
    if( !theStartTime.QuadPart )
    {
        // Determine if high-performance timing is enabled
        LARGE_INTEGER qpFrequency;
        if( QueryPerformanceFrequency( &qpFrequency ) )
        {
            // Output information
            DEBUG_INFO( "Application using high-performance timer (freq = %I64i)\n", qpFrequency.QuadPart );

            // Cast the frequency into a double
            theQPFrequency = (double)qpFrequency.QuadPart;

            // Get the timer start time
            LARGE_INTEGER time;
            QueryPerformanceCounter( &time );
            theStartTime.QuadPart = time.QuadPart;
        }
        else
        {
            // Print information
            DEBUG_INFO( "Application NOT using high-performance timer\n" );

            // Disable high resolution timing
            theQPFrequency = -1.0;

            // Set up the system timer to have the maximum resolution possible
            TIMECAPS timeCaps;
            APP_WARNING( TIMERR_NOERROR != timeGetDevCaps( &timeCaps, sizeof(timeCaps) ) ||
                         TIMERR_NOERROR != timeBeginPeriod( timeCaps.wPeriodMin ) )
                 ( "Unable to adjust system timer for maximum resolution" );

            // Enable low-resolution timing
            theStartTime.QuadPart = 1;
        }

        // Get the time
        return getAccurateSystemTime();
    }
    else if( theStartTime.QuadPart == 1 )
    {
        // Use the default timer
        return dcxClock::getSystemTime();
    }
    else
    {
        // Get the current time
        LARGE_INTEGER time;
        QueryPerformanceCounter( &time );

        // Offset time from the application start to prevent double-precision overflow
        time.QuadPart -= theStartTime.QuadPart;

        // Calculate the number of seconds represented by this value
        double quadPart = (double)time.QuadPart;
        double currentTime = quadPart / theQPFrequency;

        // Return the difference
        return currentTime;
    }
}

