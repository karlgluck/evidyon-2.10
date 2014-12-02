// This file holds the structures that are stored in the compiled game file
// by the editor to define types of special effects.
#ifndef __COMPILEDSPECIALFX_H__
#define __COMPILEDSPECIALFX_H__

namespace Evidyon {

struct CompiledVisualFXImage {
  int image_index;
};

struct CompiledMagicFXParams {
  static const int MAX_VISUAL_FX = 3;
  int sound_fx_index;
  int visual_fx_indices[MAX_VISUAL_FX];
};


struct CompiledVisualFXParams_OrbitingParticles {
  float duration;
  int particle_sfx_image;
  int particlesPerPeriod;
  unsigned long particleColors[3];
  float spawnPeriod;    // Time between creating new particles
  float risingSpeed;
  float spreadingSpeed;
  float orbitSpeed;
  float lifetime;   // How long a particle lasts at its current size
  float decayRate;  // How quickly particles decay (and disappear) after they die
  float initialHeight;
  float particleSize;
  float initialDistance;
};


struct CompiledVisualFXParams_ExplodingParticles {
  int particle_sfx_image;
};


struct CompiledVisualFXParams_TrailingParticles {
  float duration;
  int particle_sfx_image;
  int particlesPerPeriod;
  unsigned long particleColors[3];
  float spawnPeriod;
  float risingSpeed;
  float initial_height;
  float initial_radial_distance;
  float half_vertical_spread_angle;
  float radialVelocity;
  float lifetime;
  float decayRate;
  float particleSize;
};


struct CompiledVisualFXParams_SwirlingParticles {
  float duration;
  int particle_sfx_image;
  unsigned long particle_colors[3];
  int number_of_rings;
  float lowest_ring_height, added_height_per_ring;
  int particles_per_ring;
  int particle_trail_length;
  float orbit_radius;
  float min_speed, speed_range;
  float particle_size;
};



struct CompiledVisualFXParams {
  enum Type {
    ORBITING_PARTICLES_SOURCE,
    ORBITING_PARTICLES_TARGET,
    EXPLODING_PARTICLES,
    TRAILING_PARTICLES_SOURCE,
    TRAILING_PARTICLES_TARGET,
    SWIRLING_PARTICLES_SOURCE,
    SWIRLING_PARTICLES_TARGET,
  } type;
  
  union {
  CompiledVisualFXParams_OrbitingParticles orbiting_particles;
  CompiledVisualFXParams_ExplodingParticles exploding_particles;
  CompiledVisualFXParams_TrailingParticles trailing_particles;
  CompiledVisualFXParams_SwirlingParticles swirling_particles;
  };
};

}

#endif