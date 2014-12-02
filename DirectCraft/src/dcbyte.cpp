//------------------------------------------------------------------------------------------------
// File:    dcbyte.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "../dc/byte"


using namespace dc;




//------------------------------------------------------------------------------------------------
// Name:  dcByte
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
dcByte::dcByte()
{
    myValue = 0;
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string dcByte::toString() const
{
    char stringValue[16];
    sprintf_s( stringValue, sizeof(stringValue), "%hu", myValue );
    return stringValue;
}

//----[  interpret  ]----------------------------------------------------------
void dcByte::interpret(const char* string) {
  int i;
  sscanf_s(string, "%i", &i);
  myValue = i;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//------------------------------------------------------------------------------------------------
std::string dcByte::staticTypeName()
{
    return "Byte";
}
