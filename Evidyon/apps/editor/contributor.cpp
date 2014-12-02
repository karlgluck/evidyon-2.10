//------------------------------------------------------------------------------------------------
// File:    contributor.cpp
//
// Desc:    Contains information about someone who has contributed to a configuration file
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "contributor.h"




//------------------------------------------------------------------------------------------------
// Name:  Contributor
// Desc:  Initializes this class with the given name
//------------------------------------------------------------------------------------------------
Contributor::Contributor()
{
    member("Name", &myName);
    member("Contributions", &myContributions);
}




//------------------------------------------------------------------------------------------------
// Name:  TypeName
// Desc:  Gets the name of this resource
//------------------------------------------------------------------------------------------------
std::string Contributor::staticTypeName()
{
    return "Contributor";
}
