//------------------------------------------------------------------------------------------------
// File:    location.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "location.h"


dcIntegerWrapper::dcIntegerWrapper()
{
    myReference = 0;
}

dcIntegerWrapper::dcIntegerWrapper(int* reference)
{
    myReference = reference;
}

void dcIntegerWrapper::setValue(int value)
{
    *myReference = value;
}

int& dcIntegerWrapper::getValue() const
{
    return *myReference;
}

std::string dcIntegerWrapper::toString() const
{
    char stringValue[16];
    sprintf_s(stringValue, sizeof(stringValue), "%i", *myReference);
    return stringValue;
}
bool dcIntegerWrapper::save(dc::dcStreamOut* stream) {
  return stream->write(myReference, sizeof(int));
}

bool dcIntegerWrapper::load(dc::dcStreamIn* stream) {
  return stream->read(myReference, sizeof(int));
}



/*
//------------------------------------------------------------------------------------------------
// Name:  Location
// Desc:  Initializes this resource
//------------------------------------------------------------------------------------------------
Location::Location() : myX(&myValue.first), myY(&myValue.second)
{
    myValue.first = 0;
    myValue.second = 0;

    member("x", &myX);
    member("y", &myY);
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string Location::toString() const
{
    char stringValue[32];
    sprintf_s(stringValue, sizeof(stringValue), "(%i, %i)", myValue.first, myValue.second);
    return stringValue;
}



//------------------------------------------------------------------------------------------------
// Name:  save
// Desc:  Saves this resource
//------------------------------------------------------------------------------------------------
bool Location::save(dcStreamOut* stream)
{
    // Write the coordinates
    if (!stream->write(&myValue.first, sizeof(myValue.first))) return false;
    if (!stream->write(&myValue.second, sizeof(myValue.second))) return false;

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  load
// Desc:  Loads this resource
//------------------------------------------------------------------------------------------------
bool Location::load(dcStreamIn* stream)
{
    // Read the coordinates
    if (!stream->read(&myValue.first, sizeof(myValue.first))) return false;
    if (!stream->read(&myValue.second, sizeof(myValue.second))) return false;

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  compareValue
// Desc:  Compares the value stored in this class to another value
//------------------------------------------------------------------------------------------------
int Location::compareValue(const Location::BasicType& otherValue) const
{
    if (myValue.first < otherValue.first) return -1;
    if (myValue.first > otherValue.first) return +1;
    if (myValue.second < otherValue.second) return -1;
    if (myValue.second > otherValue.second) return +1;
    return 0;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string Location::staticTypeName()
{
    return "Location";
}

*/






//------------------------------------------------------------------------------------------------
// Name:  Location
// Desc:  Initializes this resource
//------------------------------------------------------------------------------------------------
Location::Location()
{
    member("x", &myX);
    member("y", &myY);
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string Location::toString() const
{
    char stringValue[32];
    sprintf_s(stringValue, sizeof(stringValue), "(%i, %i)", getX(), getY());
    return stringValue;
}


//------------------------------------------------------------------------------------------------
// Name:  compareValue
// Desc:  Compares the value stored in this class to another value
//------------------------------------------------------------------------------------------------
int Location::compareTo( const dc::dcGenericResource* other ) const
{
    if (other->getManager()->uniqueID() != getManager()->uniqueID()) return dc::dcGenericResource::compareTo(other);

    const Location& otherLocation = *(Location*)other;
    if (myX.getValue() < otherLocation.myX.getValue()) return -1;
    if (myX.getValue() > otherLocation.myX.getValue()) return +1;
    if (myY.getValue() < otherLocation.myY.getValue()) return -1;
    if (myY.getValue() > otherLocation.myY.getValue()) return +1;
    return 0;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string Location::staticTypeName()
{
    return "Location";
}