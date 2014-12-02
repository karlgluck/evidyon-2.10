//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VISUALFX_SWIRLINGPARTICLES_H__
#define __VISUALFX_SWIRLINGPARTICLES_H__


#include <dc/float>
#include <dc/integer>
#include "visualfx.h"
#include "visualfximage.h"
#include <dcx/d3dcolor>
#include <dc/boolean>



class VisualFX_SwirlingParticles : public dc::dcResource<VisualFX_SwirlingParticles,VisualFX> {
public:

  // Initializes this class
  VisualFX_SwirlingParticles();

  // Enters data into the client effect structure
  virtual bool compileForClient(Evidyon::CompiledVisualFXParams* params) const;


public:
  // Returns the type name of this class
  static std::string staticTypeName();


protected:
  dc::dcList<VisualFXImage>::Reference particle_image_;  /// The image for this particle
  dc::dcFloat duration_;
  dc::dcBoolean target_source_;
  dcx::dcxD3DColor particle_colors_[3];  /// The different colors for the particles
  dc::dcInteger number_of_rings_; //
  dc::dcFloat min_ring_height_, max_ring_height_;
  dc::dcInteger particles_per_ring_;
  dc::dcInteger particle_trail_length_;
  dc::dcFloat orbit_radius_;
  dc::dcFloat minimum_speed_, maximum_speed_;
  dc::dcFloat particle_size_;


protected:
  static dc::dcClass<VisualFX_SwirlingParticles>::Implements<VisualFX> implements_visual_fx;
};




#endif