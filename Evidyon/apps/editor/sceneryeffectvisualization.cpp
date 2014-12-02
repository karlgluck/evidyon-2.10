//------------------------------------------------------------------------------------------------
// File:    sceneryeffectvisualization.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "wavsound.h"

#include "image.h"
#include "texture.h"
#include "skin.h"
#include "mesh.h"
#include "scenery.h"
#include "sceneryeffectvisualization.h"

/// This class implements a special effect
dcClass<SceneryEffectVisualization>::Implements<EffectVisualization> SceneryEffectVisualization::implementsEffectVisualization;



//------------------------------------------------------------------------------------------------
// Name: SceneryEffectVisualization
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
SceneryEffectVisualization::SceneryEffectVisualization()
{
    member("Scenery", &myScenery);
    member("Translation", &myTranslation);
    member("Rotate Flag", &myRotateFlag);
}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Enters data into the client effect structure
//------------------------------------------------------------------------------------------------
bool SceneryEffectVisualization::compileForClient(ClientEffectVisualizationDescription* description) const
{
    description->soundEffectIndex = getSoundEffectIndex();

    description->duration = myDuration.getValue();
    description->type = CEVISUALIZATION_SCENERY;

    // Get the particle's image index
    {
        const dc::dcList<Scenery>::Element* sceneryElement = myScenery.getReferencedResource();
        description->Scenery.sceneryIndex = sceneryElement ? sceneryElement->getIndex() : 0xFFFFFFFF;
    }

    description->Scenery.x = myTranslation.getX()->getValue();
    description->Scenery.y = myTranslation.getY()->getValue();
    description->Scenery.z = myTranslation.getZ()->getValue();

    description->Scenery.rotate = myRotateFlag.getValue();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string SceneryEffectVisualization::staticTypeName()
{
    return "SceneryEffectVisualization";
}

