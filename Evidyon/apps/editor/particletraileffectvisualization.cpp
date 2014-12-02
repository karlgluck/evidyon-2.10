//------------------------------------------------------------------------------------------------
// File:    particletraileffectvisualization.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "wavsound.h"

#include "particletraileffectvisualization.h"



/// This class implements a special effect
 dc::dcClass<ParticleTrailEffectVisualization>::Implements<EffectVisualization> ParticleTrailEffectVisualization::implementsEffectVisualization;


//------------------------------------------------------------------------------------------------
// Name: ParticleTrailEffectVisualization
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
ParticleTrailEffectVisualization::ParticleTrailEffectVisualization()
{
    member("Image", &myImage);
    member("Particle Color - 0", &myParticleColor[0]);
    member("Particle Color - 1", &myParticleColor[1]);
    member("Particle Color - 2", &myParticleColor[2]);
    member("Particle Size", &myParticleSize);
    member("Particles Per Period", &myParticlesPerPeriod);
    member("Spawn Period", &mySpawnPeriod);
    member("Update Period", &myUpdatePeriod);
    member("Particle Lifetime", &myLifetime);
    member("Particle Death Time", &myDeathTime);
    member("Radial Velocity", &myRadialVelocity);
    member("Directed Velocity", &myDirectedVelocity);
    member("Rising Speed", &myRisingSpeed);
    member("Initial Height Min", &myInitialHeightMin);
    member("Initial Height Max", &myInitialHeightMax);
    member("Initial Distance Min", &myInitialDistanceMin);
    member("Initial Distance Max", &myInitialDistanceMax);
    member("Gravity Factor", &myGravityFactor);
}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Enters data into the client effect structure
//------------------------------------------------------------------------------------------------
bool ParticleTrailEffectVisualization::compileForClient(ClientEffectVisualizationDescription* description) const
{
    description->soundEffectIndex = getSoundEffectIndex();

    description->duration = myDuration.getValue();
    description->type = CEVISUALIZATION_PARTICLETRAIL;
    description->ParticleTrail.lifetime = myLifetime.getValue();
    for (int color = 0; color < 3; ++color)
        description->ParticleTrail.particleColors[color] = D3DCOLOR_XRGB(myParticleColor[color].getX()->getValue(),
                                                                          myParticleColor[color].getY()->getValue(),
                                                                          myParticleColor[color].getZ()->getValue());
    
    // Get the particle's image index
    const dc::dcTable<Image>::Element* imageElement = myImage.getReferencedResource();
    description->ParticleTrail.particleImageIndex = imageElement ? imageElement->getIndex() : 0xFFFFFFFF;

    description->ParticleTrail.particleSize = myParticleSize.getValue();
    description->ParticleTrail.particlesPerPeriod = myParticlesPerPeriod.getValue();
    description->ParticleTrail.risingSpeed = myRisingSpeed.getValue();
    description->ParticleTrail.spawnPeriod = mySpawnPeriod.getValue();
    description->ParticleTrail.updatePeriod = myUpdatePeriod.getValue();
    description->ParticleTrail.radialVelocity = myRadialVelocity.getValue();
    description->ParticleTrail.directedVelocity = myDirectedVelocity.getValue();
    description->ParticleTrail.initialDistanceMin = myInitialDistanceMin.getValue();
    description->ParticleTrail.initialDistanceMax = myInitialDistanceMax.getValue();
    description->ParticleTrail.decayRate = myParticleSize.getValue() / myDeathTime.getValue();
    description->ParticleTrail.initialHeightMin = myInitialHeightMin.getValue();
    description->ParticleTrail.initialHeightMax = myInitialHeightMax.getValue();
    description->ParticleTrail.gravityFactor = myGravityFactor.getValue();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string ParticleTrailEffectVisualization::staticTypeName()
{
    return "ParticleTrailEffectVisualization";
}

