//------------------------------------------------------------------------------------------------
// File:    dcxwin32clock.h
//
// Desc:    Provides a high-precision timer for the Win32 environment, using the
//          QueryPerformanceFrequency/QueryPerformanceTimer methods, if they are available.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXWIN32TIME_H__
#define __DCXWIN32TIME_H__


namespace dcx {

    /**
     * Implements the time interface for the Windows platform
     */
    class dcxWin32Clock : public dcxClock
    {
        public:

            /**
             * Gets the current wall time using the accurate Windows system timer
             */
            double getWallTime() const;


        public:

            /**
             * High-resolution (if available) global method for obtaining a time value.  The
             * time is returned in seconds, NOT milliseconds.
             * @return Current system time in seconds
             */
            static double getAccurateSystemTime();


        protected:

            /// Number of counts per second, if using high-resolution timing.
            static double theQPFrequency;

            /// The time at which getAccurateSystemTime() was first called.  All high-resolution timing
            /// values are offsets from this point.  If high-resolution timing is not available, this value
            /// is 1 and the default timing method will be used (which is not offset).  If the value is zero,
            /// the timer hasn't been initialized yet.
            static LARGE_INTEGER theStartTime;
    };
}




#endif