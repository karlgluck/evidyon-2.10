//------------------------------------------------------------------------------------------------
// File:    dcstring.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "../dc/string"


using namespace dc;





//------------------------------------------------------------------------------------------------
// Name:  dcString
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
dcString::dcString()
{
    myValue = "";
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string dcString::toString() const
{
    return myValue;
}



//----[  interpret  ]----------------------------------------------------------
void dcString::interpret(const char* string) {
  myValue = string;
}


//------------------------------------------------------------------------------------------------
// Name:  save
// Desc:  Saves this resource
//------------------------------------------------------------------------------------------------
bool dcString::save( dcStreamOut* stream )
{
    return stream->writeString( myValue );
}


//------------------------------------------------------------------------------------------------
// Name:  load
// Desc:  Loads this resource
//------------------------------------------------------------------------------------------------
bool dcString::load( dcStreamIn* stream )
{
    return stream->readString( &myValue );
}


//------------------------------------------------------------------------------------------------
// Name:  compareValue
// Desc:  Compares the value stored in this class to another value
//------------------------------------------------------------------------------------------------
int dcString::compareValue( const std::string& otherValue ) const
{
    return myValue.compare( otherValue );
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//------------------------------------------------------------------------------------------------
std::string dcString::staticTypeName()
{
    return "String";
}
