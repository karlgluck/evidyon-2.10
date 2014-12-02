//------------------------------------------------------------------------------------------------
// File:    dcboolean.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "../dc/boolean"


using namespace dc;



//------------------------------------------------------------------------------------------------
// Name:  dcBoolean
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
dcBoolean::dcBoolean()
{
    myValue = false;
}


//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//------------------------------------------------------------------------------------------------
std::string dcBoolean::toString() const
{
    return myValue ? "true" : "false";
}





//----[  interpret  ]----------------------------------------------------------
void dcBoolean::interpret(const char* string) {
  bool t = string[0] == 'T' || string[0] == 't' ||
           string[0] == '1';
  bool f = string[0] == 'F' || string[0] == 'f' ||
           string[0] == '0';
  if (t && !f) {
    myValue = true;
  } else if (f && !t) {
    myValue = false;
  } else {
    myValue = (stricmp(string, "false") != 0);
  }
}


//------------------------------------------------------------------------------------------------
// Name:  toggle
// Desc:  Swaps the value of this flag and returns the new value
//------------------------------------------------------------------------------------------------
bool dcBoolean::toggle()
{
    // Swap values
    myValue = !myValue;

    // Return the new value
    return myValue;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//------------------------------------------------------------------------------------------------
std::string dcBoolean::staticTypeName()
{
    return "Boolean";
}