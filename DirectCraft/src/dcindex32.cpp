//------------------------------------------------------------------------------------------------
// File:    dcindex32.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "../dc/index32"

using namespace dc;



//------------------------------------------------------------------------------------------------
// Name:  dcIndex32
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
dcIndex32::dcIndex32()
{
    myValue = 0;
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string dcIndex32::toString() const
{
    char stringValue[16];
    sprintf_s( stringValue, sizeof(stringValue), "%lu", myValue );
    return stringValue;
}



//----[  interpret  ]----------------------------------------------------------
void dcIndex32::interpret(const char* string) {
  int i;
  sscanf_s(string, "%i", &i);
  myValue = i;
}

//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//------------------------------------------------------------------------------------------------
std::string dcIndex32::staticTypeName()
{
    return "Index32";
}

