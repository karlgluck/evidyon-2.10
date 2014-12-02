//------------------------------------------------------------------------------------------------
// File:    volucrisguisideiconselement.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"




//------------------------------------------------------------------------------------------------
// Name: render
// Desc: Renders this element to the target layer
//------------------------------------------------------------------------------------------------
bool VolucrisGUISideIconsElement::render(GUILayer* layer)
{
    if (!layer || !mySourceCanvas) return true;
    const GUIRect* destArea = mySourceCanvas->getArea();
    float centerY = (destArea->top + destArea->bottom)/2.0f;
    float depth = mySourceCanvas->getDepth() - 0.001f;

    if (!renderCentered(layer, (float)(destArea->left + 2),  centerY, depth) ||
        !renderCentered(layer, (float)(destArea->right - 2), centerY, depth))
        return false;

    return true;
}