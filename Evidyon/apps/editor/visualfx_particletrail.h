//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VISUALFX_PARTICLETRAIL_H__
#define __VISUALFX_PARTICLETRAIL_H__


#include "visualfx.h"
#include "visualfximage.h"
#include <dcx/d3dcolor>

class VisualFX_ParticleTrail : public dc::dcResource<VisualFX_ParticleTrail,VisualFX> {
public:
  VisualFX_ParticleTrail(); // Initializes this class

  // Enters data into the client effect structure
  virtual bool compileForClient(Evidyon::CompiledVisualFXParams* params) const;


public:
  // Returns the type name of this class
  static std::string staticTypeName();


protected:
  dc::dcList<VisualFXImage>::Reference myImage;  /// The image for this particle
  dcx::dcxD3DColor myParticleColor[3];  /// The different colors for the particles
  dc::dcFloat myParticleSize; /// The size of the particles
  dc::dcFloat myDuration;
  dc::dcBoolean target_source_;
  dc::dcByte myParticlesPerPeriod;  /// How many particles to create each period
  dc::dcFloat myTotalLifetime; /// How long each particle lasts (total length!)
  dc::dcFloat myDecayTime;  /// How long it takes for a particle to decay
  dc::dcInteger myParticleDensity; // lifetime / [[spawn period]] * particles per period = density
  dc::dcFloat myRisingSpeed;  /// How quickly the particles drift upward (negative is downward)
  dc::dcFloat myRadialVelocity; /// How fast the particles move away from the center
  dc::dcFloat myInitialRadialDistance; /// Distance away from the source point
  dc::dcFloat myInitialHeight; /// The initial Y-location of the particles
  dc::dcFloat myInitialVerticalSpread; // The number of degrees between which particles can be generated (spherical)


protected:
  static dc::dcClass<VisualFX_ParticleTrail>::Implements<VisualFX> implementsVisualFX;
};




#endif