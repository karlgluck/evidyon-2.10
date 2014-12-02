//------------------------------------------------------------------------------------------------
// File:    wavsound.h
//
// Desc:    Controls audio playing and loading
//
// Copyright (c) Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __WAVSOUND_H__
#define __WAVSOUND_H__

#include <dc/filename>
//#include <dc/stream>


/**
 * Holds sound information
 */
class WAVSound : public dc::dcResource<WAVSound>
{
    public:

        /**
         * Initializes this class
         */
        WAVSound();

        /**
         * Compiles this sound so that the client can load it
         */
        bool compileForClient(dc::dcStreamOut* clientStream) const;

        dc::dcFileName* getSourceFile() { return &mySourceFile; }

    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The file from which to load the sound
        dc::dcFileName mySourceFile;
};





#endif