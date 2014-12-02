#include "specialfxcontroller_trailingparticle.h"
#include "../shared/compiledspecialfx.h"
#include "globalspecialfxmanager.h"
#include "actor.h"
#include "specialfxbinding.h"
#include <math.h>
#include <dc/debug>


namespace Evidyon {

SpecialFXController_TrailingParticle::SpecialFXController_TrailingParticle(
    double time,
    SpecialFXBinding* binding,
    double duration,
    const CompiledVisualFXParams_TrailingParticles* params) {
  memset(particles_, 0, sizeof(particles_));
  number_of_particles_ = 0;
  stop_generation_time_ = time + duration;
  next_generation_time_ = 0.0;
  (binding_ = binding)->addReference();
  params_ = params;
}

void SpecialFXController_TrailingParticle::destroy(GlobalSpecialFXManager* manager) {
  if (binding_) {
    binding_->removeReference();
    binding_ = NULL;
  }

  {
    const int number_of_particles = number_of_particles_;
    for (int particle_index = 0;
       particle_index < number_of_particles; 
       ++particle_index) {
      manager->releaseTrailingParticleData(particles_[particle_index]);
    }
    number_of_particles_ = 0;
  }
}


bool SpecialFXController_TrailingParticle::update(double time,
                          double time_since_last_update,
                          GlobalSpecialFXManager* manager) {
  if (!binding_) return true;

  // Advance particles
  int number_of_particles = number_of_particles_;
  for (int particle_index = 0; particle_index < number_of_particles;) {
    TrailingParticleData* particle = particles_[particle_index];

    double time_until_dying = particle->dying_time - time;
    bool dying = time_until_dying < 0.0;
    if (dying && (particle->rendering_particle.point_size <= 0.0f)) {
      manager->releaseTrailingParticleData(particle);

      // Swap down into this location
      --number_of_particles;
      particles_[particle_index] = particles_[number_of_particles];
      particles_[number_of_particles] = NULL;

      continue;
    }
    else
    {
      float dx = time_since_last_update * particle->vx;
      float dy = time_since_last_update * particle->vy;
      float dz = time_since_last_update * particle->vz;

      // Move this particle
      particle->rendering_particle.x += dx;
      particle->rendering_particle.y += dy;
      particle->rendering_particle.z += dz;

      // Shrink the particle if it is dying.  Since time_until_dying is
      // negative, we add the rate-scaled value (which will be negative)
      // to the particle size.
      if (dying) {
        particle->rendering_particle.point_size =
          params_->particleSize + params_->decayRate * time_until_dying;
      }

      ++particle_index; // advance to the next particle index
    }
  }

  // Check to see if new particles need to be generated
  {
    bool finished_generating = binding_->getExpired() || time > stop_generation_time_;
    if (time < next_generation_time_ || finished_generating) {
      number_of_particles_ = number_of_particles;
      return finished_generating && (number_of_particles == 0);
    }
    next_generation_time_ = time + params_->spawnPeriod;
  }

  float src_x, src_y;
  binding_->get(&src_x, &src_y, NULL);

  // Generate new particles
  for (int j = 0; j < params_->particlesPerPeriod &&
          number_of_particles < MAX_PARTICLES; ++j) {
    TrailingParticleData* particle = manager->acquireTrailingParticleData();
    CONFIRM(particle) else {
      break;
    }

    float particle_angle = (rand()%RAND_MAX)/((float)RAND_MAX) * 2 * 3.14;
    float particle_inclination = ((rand()%RAND_MAX)*((float)RAND_MAX) - 0.5f) * params_->half_vertical_spread_angle;
    float initial_distance = params_->initial_radial_distance;
    float velocity = params_->radialVelocity;
    float dx = cosf(particle_angle) * cosf(particle_inclination);
    float dy = sinf(particle_inclination);
    float dz = sinf(particle_angle) * cosf(particle_inclination);

    particle->rendering_particle.x = dx * initial_distance + src_x;
    particle->rendering_particle.y = dy * initial_distance + params_->initial_height;
    particle->rendering_particle.z = dz * initial_distance + src_y;
    particle->rendering_particle.color = params_->particleColors[rand()%3];
    particle->dying_time = time + params_->lifetime;
    particle->vx = velocity * dx;
    particle->vy = velocity * dy + params_->risingSpeed;
    particle->vz = velocity * dz;
    particle->rendering_particle.point_size = params_->particleSize;

    // Add to particle list
    particles_[number_of_particles++] = particle;
  }

  number_of_particles_ = number_of_particles;
  if (number_of_particles == MAX_PARTICLES) {
    next_generation_time_ = time; // generating too fast; do this to prevent jerkiness
  }

  return false;
}



bool SpecialFXController_TrailingParticle::renderFX(
  int image_id,
  IsometricCamera* camera,
  GlobalSpecialFXManager* manager) {

  TrailingParticleData** particles = particles_;
  int particles_to_draw = number_of_particles_;
  while (particles_to_draw > 0) {
    RenderingParticle* rendering_particles = NULL;
    int particles_available = 0;
    if (!manager->lockParticles(particles_to_draw,
                               &rendering_particles,
                               &particles_available))
      return false;
    for (int i = 0; i < particles_available; ++i) {
      TrailingParticleData* particle = *particles;
      memcpy(rendering_particles,
            &particle->rendering_particle,
             sizeof(RenderingParticle));
      ++particles;
      ++rendering_particles;
    }
    particles_to_draw -= particles_available;
    manager->unlockParticles();
  }

  return true;
}






}