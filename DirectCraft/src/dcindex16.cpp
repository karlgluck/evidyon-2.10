//------------------------------------------------------------------------------------------------
// File:    dcindex16.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "../dc/index16"


using namespace dc;



//------------------------------------------------------------------------------------------------
// Name:  dcIndex16
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
dcIndex16::dcIndex16()
{
    myValue = 0;
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string dcIndex16::toString() const
{
    char stringValue[16];
    sprintf_s( stringValue, sizeof(stringValue), "%su", myValue );
    return stringValue;
}


//----[  interpret  ]----------------------------------------------------------
void dcIndex16::interpret(const char* string) {
  int i;
  sscanf_s(string, "%i", &i);
  myValue = i;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//------------------------------------------------------------------------------------------------
std::string dcIndex16::staticTypeName()
{
    return "Index16";
}

