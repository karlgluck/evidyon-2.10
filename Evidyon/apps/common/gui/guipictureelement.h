//------------------------------------------------------------------------------------------------
// File:  guipictureelement.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIPICTUREELEMENT_H__
#define __GUIPICTUREELEMENT_H__

#include "guielement.h"
typedef unsigned long D3DCOLOR;
struct GUIRect;

/**
 * Puts a picture on the screen without distorting it
 */
class GUIPictureElement : public GUIElement
{
  public:

    /**
     * Initializes the class
     */
    GUIPictureElement();

    /**
     * Stores the texture for this element
     */
    void setTexture(int texture);

    /**
     * Calculates the texture coordinates for the the source area for this element using
     * the dimensions of the current texture.  This method will not function unless
     * the internal texture is currently valid.
     */
    void setSourceRegion(const GUIRect* sourceArea, GUILayer* layer);

    /**
     * Calculates the texture coordinates for the the source area for this element
     */
    void setSourceRegion(const GUIRect* sourceArea, GUISize textureDimensions);

  public:

    /**
     * Renders this text list into the given layer.
     */
    bool render(GUILayer* layer);

    /**
     * Gets the texture used by this fill element
     */
    int getTexture();

    /**
     * Gets the width of the picture this element represents
     */
    float getPictureWidth() const;

    /**
     * Gets the height of the picture this element represents
     */
    float getPictureHeight() const;


  protected:

    /**
     * Draws this picture at the provided coordinates
     */
    bool render(GUILayer* layer, float x, float y, float depth);

    /**
     * Draws this picture centered around the given coordinates
     */
    bool renderCentered(GUILayer* layer, float centerX, float centerY, float depth);


  protected:

    /// The texture used by this element to render
    int myTexture;

    /// The diffuse color for this element
    D3DCOLOR myDiffuseColor;

  private:

    /// The size of this picture
    float myPictureWidth, myPictureHeight;

    /// The element's texture coordinates
    ///  indexed by:  vertex row, vertex column, u/v
    float myTexcoords[2][2][2];
};




#endif