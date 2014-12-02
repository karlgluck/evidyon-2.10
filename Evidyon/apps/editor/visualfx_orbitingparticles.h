//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VISUALFX_ORBITINGPARTICLES_H__
#define __VISUALFX_ORBITINGPARTICLES_H__


#include <dc/float>
#include <dc/integer>
#include "visualfx.h"
#include "visualfximage.h"
#include <dcx/d3dcolor>
#include <dc/boolean>



class VisualFX_OrbitingParticles : public dc::dcResource<VisualFX_OrbitingParticles,VisualFX> {
public:

  // Initializes this class
  VisualFX_OrbitingParticles();

  // Enters data into the client effect structure
  virtual bool compileForClient(Evidyon::CompiledVisualFXParams* params) const;


public:
  // Returns the type name of this class
  static std::string staticTypeName();


protected:
  dc::dcList<VisualFXImage>::Reference myImage;  /// The image for this particle
  dc::dcFloat myDuration;
  dc::dcBoolean target_source_;
  dcx::dcxD3DColor myParticleColor[3];  /// The different colors for the particles
  dc::dcByte myParticlesPerPeriod;  /// How many particles to create each period
  dc::dcInteger myParticleDensity; // lifetime / [[spawn period]] * particles per period = density
  dc::dcFloat myRisingSpeed;  /// How quickly the particles drift upward (negative is downward)
  dc::dcFloat myInitialDistance;  /// Distance away in the ring from the source point
  dc::dcFloat mySpreadingSpeed; /// How fast the particles move away from the center
  dc::dcFloat myOrbitSpeed; /// How fast the particles turn around the central axis
  dc::dcFloat myTotalLifetime; /// Total lifetime of a particle
  dc::dcFloat myDecayTime;  /// How much of a particle's total lifetime is spent decaying
  dc::dcFloat myParticlesInitialHeight; /// The initial Y-location of the particles
  dc::dcFloat myParticleSize; /// The size of the particles before they start decaying


protected:
  static dc::dcClass<VisualFX_OrbitingParticles>::Implements<VisualFX> implementsVisualFX;
};




#endif