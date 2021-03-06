//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "wavsound.h"

#include "visualfx_orbitingparticles.h"

#include "../shared/compiledspecialfx.h"

/// This class implements a special effect
dcClass<VisualFX_OrbitingParticles>::Implements<VisualFX> VisualFX_OrbitingParticles::implementsVisualFX;




//------------------------------------------------------------------------------------------------
// Name: VisualFX_OrbitingParticles
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VisualFX_OrbitingParticles::VisualFX_OrbitingParticles() {
  member("Image", &myImage);
  member("Particle Color - 0", &myParticleColor[0]);
  member("Particle Color - 1", &myParticleColor[1]);
  member("Particle Color - 2", &myParticleColor[2]);
  member("Orbit Source", &target_source_);
  member("Particle Size", &myParticleSize);
  member("Duration", &myDuration);
  member("Particles Per Period", &myParticlesPerPeriod);
  member("Particle Total Lifetime", &myTotalLifetime);
  member("Particle Decay Time", &myDecayTime);
  member("Density", &myParticleDensity);
  member("Rising Speed", &myRisingSpeed);
  member("Spreading Speed", &mySpreadingSpeed);
  member("Orbiting Speed", &myOrbitSpeed);
  member("Initial Height", &myParticlesInitialHeight);
  member("Initial Radial Distance", &myInitialDistance);

  target_source_.setValue(true);

  for (int i = 0; i < 3; ++i) {
    myParticleColor[i].getR()->setValue(255);
    myParticleColor[i].getG()->setValue(255);
    myParticleColor[i].getB()->setValue(255);
  }
}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Enters data into the client effect structure
//------------------------------------------------------------------------------------------------
bool VisualFX_OrbitingParticles::compileForClient(Evidyon::CompiledVisualFXParams* params) const {
  {
    const dc::dcList<VisualFXImage>::Element* element = myImage.getReferencedResource();
    params->orbiting_particles.particle_sfx_image =
      element == NULL ? NULL : element->getIndex();
  }

  float decay_time = myDecayTime.getValue();
  {
    float lifetime = myTotalLifetime.getValue();
    decay_time = min(decay_time, lifetime);
  }
  params->orbiting_particles.duration = myDuration.getValue();
  params->type = target_source_.getValue() ?
      Evidyon::CompiledVisualFXParams::ORBITING_PARTICLES_SOURCE :
      Evidyon::CompiledVisualFXParams::ORBITING_PARTICLES_TARGET;
  params->orbiting_particles.initialHeight = myParticlesInitialHeight.getValue();
  params->orbiting_particles.lifetime = myTotalLifetime.getValue() - decay_time;
  params->orbiting_particles.orbitSpeed = D3DXToRadian(myOrbitSpeed.getValue());
  for (int color = 0; color < 3; ++color) {
    params->orbiting_particles.particleColors[color] = myParticleColor[color].getD3DColor();
  }
  params->orbiting_particles.particleSize = myParticleSize.getValue();
  params->orbiting_particles.particlesPerPeriod = myParticlesPerPeriod.getValue();
  params->orbiting_particles.risingSpeed = myRisingSpeed.getValue();
  params->orbiting_particles.spawnPeriod =
      (myTotalLifetime.getValue() * myParticlesPerPeriod.getValue()) /
       myParticleDensity.getValue();
  params->orbiting_particles.spreadingSpeed = mySpreadingSpeed.getValue();
  params->orbiting_particles.initialDistance = myInitialDistance.getValue();
  params->orbiting_particles.decayRate = myParticleSize.getValue() / decay_time;

  // Success
  return true;
}

//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string VisualFX_OrbitingParticles::staticTypeName()
{
    return "VisualFX_OrbitingParticles";
}


