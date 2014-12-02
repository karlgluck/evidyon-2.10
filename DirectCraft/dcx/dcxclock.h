//------------------------------------------------------------------------------------------------
// File:    dcxclock.h
//
// Desc:    Provides a generic interface for timing methods
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXTIME_H__
#define __DCXTIME_H__



namespace dcx {

/**
 * Interface implemented by a timing class
 */
class dcxClock {
public:

    /**
     * Gets the current wall time in seconds using the preferred method
     * of the implementing timer.  The default time interface uses the
     * CRT library's low-resolution timer.
     */
    virtual double getWallTime() const;


public:

    /**
     * Low-resolution, global method for obtaining a time value in milliseconds
     */
    static unsigned long getSystemTime();
};

}

#endif