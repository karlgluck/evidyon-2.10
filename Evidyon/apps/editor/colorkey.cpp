//------------------------------------------------------------------------------------------------
// File:    colorkey.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "colorkey.h"


//------------------------------------------------------------------------------------------------
// Name:  ColorKey
// Desc:  Sets up this color key with the provided name
//------------------------------------------------------------------------------------------------
ColorKey::ColorKey()
{
    member("Enable", &myEnableFlag);
    member("Color", &myColor);
}


//------------------------------------------------------------------------------------------------
// Name:  isEnabled
// Desc:  Determines whether or not this color key is enabled
//------------------------------------------------------------------------------------------------
bool ColorKey::isEnabled() const
{
    return myEnableFlag.getValue();
}



//------------------------------------------------------------------------------------------------
// Name:  getRGBColor
// Desc:  Obtains the COLORREF-formatted color of this color key
//------------------------------------------------------------------------------------------------
COLORREF ColorKey::getRGBColor() const
{
    return RGB(myColor.getX()->getValue(),
                myColor.getY()->getValue(),
                myColor.getZ()->getValue());
}



//------------------------------------------------------------------------------------------------
// Name:  getD3DColor
// Desc:  Gets the D3DCOLOR_XRGB-formatted color of this color key
//------------------------------------------------------------------------------------------------
D3DCOLOR ColorKey::getD3DColor() const
{
    return D3DCOLOR_XRGB( myColor.getX()->getValue(),
                           myColor.getY()->getValue(),
                           myColor.getZ()->getValue());
}



//------------------------------------------------------------------------------------------------
// Name:  setRGBColor
// Desc:  Sets the color of this colorkey by a COLORREF value
//------------------------------------------------------------------------------------------------
void ColorKey::setRGBColor(COLORREF color)
{
    myColor.getX()->setValue(GetRValue(color));
    myColor.getY()->setValue(GetGValue(color));
    myColor.getZ()->setValue(GetBValue(color));
}



//------------------------------------------------------------------------------------------------
// Name:  enable
// Desc:  Enables or disables this color key
//------------------------------------------------------------------------------------------------
void ColorKey::enable(bool en)
{
    myEnableFlag.setValue(en);
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class (needed for DC library)
//------------------------------------------------------------------------------------------------
std::string ColorKey::staticTypeName()
{
    return "ColorKey";
}

