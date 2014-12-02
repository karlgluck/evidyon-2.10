//------------------------------------------------------------------------------------------------
// File:    dcinteger.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "../dc/integer"


using namespace dc;



//------------------------------------------------------------------------------------------------
// Name:  dcFileName
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
dcInteger::dcInteger()
{
    myValue = 0;
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string dcInteger::toString() const
{
    char stringValue[16];
    sprintf_s( stringValue, sizeof(stringValue), "%i", myValue );
    return stringValue;
}


//----[  interpret  ]----------------------------------------------------------
void dcInteger::interpret(const char* string) {
  int i;
  if (1 == sscanf_s(string, "%i", &i)) {
    myValue = i;
  } else {
    myValue = 0;
  }
}

//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//------------------------------------------------------------------------------------------------
std::string dcInteger::staticTypeName()
{
    return "Integer";
}
