//------------------------------------------------------------------------------------------------
// File:    credits.h
//
// Desc:    Contains a list of credits!
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __CREDITS_H__
#define __CREDITS_H__


class Credits : public dc::dcResource<Credits>
{
    public:

        /**
         * Initializes this class with the given name
         */
        Credits(std::string name);


    public:

        /**
         * Gets the name of this resource
         */
        static std::string staticTypeName();


    protected:

        /// Set of credits
        dc::dcList<Contributor> myCredits;
};



#endif