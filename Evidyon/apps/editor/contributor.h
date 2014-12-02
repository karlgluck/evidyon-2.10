//------------------------------------------------------------------------------------------------
// File:    contributor.h
//
// Desc:    Contains information about someone who has contributed to a configuration file
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __CONTRIBUTOR_H__
#define __CONTRIBUTOR_H__


class Contributor : public dc::dcResource<Contributor>
{
    public:

        /**
         * Initializes this class with the given name
         */
        Contributor();


    public:

        /**
         * Gets the name of this resource
         */
        static std::string staticTypeName();


    protected:

        /// The name of the contributor
        dc::dcString myName;

        /// What this person has done
        dc::dcString myContributions;
};



#endif