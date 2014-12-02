//------------------------------------------------------------------------------------------------
// File:    unseenkeyframeanimation.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __UNSEENKEYFRAMEANIMATION_H__
#define __UNSEENKEYFRAMEANIMATION_H__




/**
 * This class references a .ka file containing a keyframe animation ready to be applied to a mesh
 */
class UnseenKeyframeAnimation : public dc::dcResource<UnseenKeyframeAnimation>
{
    public:

        /**
         * Initializes this resource
         */
        UnseenKeyframeAnimation();


    public:

        /**
         * Loads the referenced animation from the source file
         */
        bool loadAnimation(UnseenSkeletalAnimation* animation) const;

        /**
         * Gets the source file name for this skeletal animation
         * @return Pointer to the internal file name object
         */
        dc::dcFileName* getFileName();


    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The file from which to load the animation
        dc::dcFileName mySourceFile;
};



#endif