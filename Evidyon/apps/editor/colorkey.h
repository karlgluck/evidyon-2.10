// File:    colorkey.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __COLORKEY_H__
#define __COLORKEY_H__


/**
 * Stores an RGB color key, which is used by the Image class and its derivatives to make a color
 * in the file being loaded transparent.
 */
class ColorKey : public dc::dcResource<ColorKey>
{
public:

  /**
   * Sets up this color key with the provided name
   */
  ColorKey();

  /**
   * Determines whether or not this color key is enabled
   */
  bool isEnabled() const;

  /**
   * Obtains the COLORREF-formatted color of this color key
   */
  COLORREF getRGBColor() const;

  /**
   * Gets the D3DCOLOR_XRGB-formatted color of this color key
   */
  D3DCOLOR getD3DColor() const;

  /**
   * Sets the color of this colorkey by a COLORREF value
   */
  void setRGBColor(COLORREF color);

  /**
   * Enables or disables this color key
   */
  void enable(bool en);


public:

  /**
   * Gets the name of this class (needed for DC library)
   */
  static std::string staticTypeName();


protected:

  /// Whether or not the color key is enabled
  dc::dcBoolean myEnableFlag;

  /// The color of this color key
  dcxVector3<dcByte> myColor;
};




#endif