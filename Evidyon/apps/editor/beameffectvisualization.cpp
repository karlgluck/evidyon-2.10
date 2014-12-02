//------------------------------------------------------------------------------------------------
// File:    beameffectvisualization.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "wavsound.h"

#include "beameffectvisualization.h"

/// This class implements a special effect
dcClass<BeamEffectVisualization>::Implements<EffectVisualization> BeamEffectVisualization::implementsEffectVisualization;



//------------------------------------------------------------------------------------------------
// Name: BeamEffectVisualization
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
BeamEffectVisualization::BeamEffectVisualization()
{
    member("Source Image", &mySourceImage);
    member("Terminal Image", &myTerminalImage);
    member("Beam Fill Image", &myBeamFillImage);
    member("Beam Width", &myBeamWidth);
    member("Ends Image Size", &myEndsImageSize);
    member("Beam Height", &myHeight);
    member("Cast From Hands", &myCastFromHandsFlag);
}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Enters data into the client effect structure
//------------------------------------------------------------------------------------------------
bool BeamEffectVisualization::compileForClient(ClientEffectVisualizationDescription* description) const
{
    description->soundEffectIndex = getSoundEffectIndex();

    description->duration = myDuration.getValue();
    description->type = CEVISUALIZATION_BEAM;
    description->Beam.beamWidth = myBeamWidth.getValue();
    description->Beam.height = myHeight.getValue();
    description->Beam.sourceTerminalImageSize = myEndsImageSize.getValue();
    description->Beam.cast_from_hands = myCastFromHandsFlag.getValue();

    // Get the particle's image index
    {
        const dc::dcTable<Image>::Element* imageElement = mySourceImage.getReferencedResource();
        description->Beam.sourceImageIndex = imageElement ? imageElement->getIndex() : 0xFFFFFFFF;
    }

    // Get the particle's image index
    {
        const dc::dcTable<Image>::Element* imageElement = myTerminalImage.getReferencedResource();
        description->Beam.terminalImageIndex = imageElement ? imageElement->getIndex() : 0xFFFFFFFF;
    }

    // Get the particle's image index
    {
        const dc::dcTable<Image>::Element* imageElement = myBeamFillImage.getReferencedResource();
        description->Beam.beamImageIndex = imageElement ? imageElement->getIndex() : 0xFFFFFFFF;
    }

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string BeamEffectVisualization::staticTypeName()
{
    return "BeamEffectVisualization";
}

