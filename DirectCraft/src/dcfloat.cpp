//------------------------------------------------------------------------------------------------
// File:    dcfloat.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "../dc/float"


using namespace dc;




//------------------------------------------------------------------------------------------------
// Name:  dcFloat
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
dcFloat::dcFloat()
{
    myValue = 0.0f;
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string dcFloat::toString() const
{
    char stringValue[64];
    sprintf_s( stringValue, sizeof(stringValue), "%.04f", myValue );
    return stringValue;
}



//----[  serialize  ]----------------------------------------------------------
void dcFloat::serialize(std::string* string) {
  char value[64];
  sprintf_s(value, sizeof(value), "%f", myValue);
  string->assign(value);
}


//----[  interpret  ]----------------------------------------------------------
void dcFloat::interpret(const char* string) {
  sscanf_s(string, "%f", &myValue);
}

//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//------------------------------------------------------------------------------------------------
std::string dcFloat::staticTypeName()
{
    return "Float";
}
