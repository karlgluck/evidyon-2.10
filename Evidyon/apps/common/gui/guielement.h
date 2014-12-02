//------------------------------------------------------------------------------------------------
// File:  guielement.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIELEMENT_H__
#define __GUIELEMENT_H__


class GUILayer;
class GUIGenericCanvas;
struct GUIRect;

/**
 * A GUI element is something that creates geometry in a GUILayer during rendering
 */
class GUIElement
{
  public:

    /**
     * Initializes the class
     */
    GUIElement();

    /**
     * Sets the canvas that this class uses for position and size references
     */
    void setSourceCanvas(GUIGenericCanvas* sourceCanvas);

    /** 
     * Sets the canvas outside of which this picture element will not draw
     */
    void setClippingArea(const GUIRect* clippingArea);

    /**
     * Gets the clipping area canvas
     */
    const GUIRect* getClippingArea();


  public:

    /**
     * Updates this element
     */
    virtual void advance(double currentTime, double timeSinceLastUpdate);

    /**
     * Renders this element into the given layer
     */
    virtual bool render(GUILayer* layer);

    /**
     * This method queries the element for the texture that it uses when rendering.  It
     * is invoked by the GUILayer class when the element is registered with the GUI.
     */
    virtual int getTexture() = 0;


  protected:

    /// This defines the rendering target for this element
    GUIGenericCanvas* mySourceCanvas;

    /// Defines the area into which to clip this element; if it doesn't require
    /// clipping, this is set to null.
    const GUIRect* myClippingArea;
};

#endif