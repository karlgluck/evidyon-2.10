//------------------------------------------------------------------------------------------------
// File:    unseenkeyframeanimation.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "unseenkeyframeanimation.h"
#include <dc/filestream>



//------------------------------------------------------------------------------------------------
// Name: UnseenKeyframeAnimation
// Desc: Initializes this resource
//------------------------------------------------------------------------------------------------
UnseenKeyframeAnimation::UnseenKeyframeAnimation()
{
    member("Source File", &mySourceFile);
}



//------------------------------------------------------------------------------------------------
// Name: loadAnimation
// Desc: Loads the referenced animation from the source file
//------------------------------------------------------------------------------------------------
bool UnseenKeyframeAnimation::loadAnimation(UnseenSkeletalAnimation* animation) const
{
    // Open the source stream
    dc::dcFileStream fs;
    if (APP_ERROR(!fs.open(mySourceFile.getValue().c_str(), 0))("Couldn't open source keyframe animation file %s", mySourceFile.getValue().c_str()))
        return false;

    // Load the mesh
    bool failed = APP_ERROR(!loadUnseenSkeletalAnimation(&fs, animation))("Invalid Unseen keyframe animation file %s", mySourceFile.getValue().c_str());

    // Close the stream
    fs.close();

    // Return the result
    return !failed;
}



//------------------------------------------------------------------------------------------------
// Name: getFileName
// Desc: Gets the source file name for this skeletal animation
//       @return Pointer to the internal file name object
//------------------------------------------------------------------------------------------------
dcFileName* UnseenKeyframeAnimation::getFileName()
{
    return &mySourceFile;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Gets the string name of this resource type
//       @return Class name
//------------------------------------------------------------------------------------------------
std::string UnseenKeyframeAnimation::staticTypeName()
{
    return "UnseenKeyframeAnimation";
}

