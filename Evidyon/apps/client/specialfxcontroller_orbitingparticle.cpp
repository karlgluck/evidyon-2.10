#include "specialfxcontroller_orbitingparticle.h"
#include "../shared/compiledspecialfx.h"
#include "globalspecialfxmanager.h"
#include "actor.h"
#include "specialfxbinding.h"
#include <math.h>
#include <dc/debug>


namespace Evidyon {

SpecialFXController_OrbitingParticle::SpecialFXController_OrbitingParticle(
    double time,
    SpecialFXBinding* binding,
    double duration,
    const CompiledVisualFXParams_OrbitingParticles* params) {
  memset(particles_, 0, sizeof(particles_));
  number_of_particles_ = 0;
  stop_generation_time_ = time + duration;
  next_generation_time_ = 0.0;
  (binding_ = binding)->addReference();
  params_ = params;
}

void SpecialFXController_OrbitingParticle::destroy(GlobalSpecialFXManager* manager) {
  if (binding_) {
    binding_->removeReference();
    binding_ = NULL;
  }

  {
    const int number_of_particles = number_of_particles_;
    for (int particle_index = 0;
       particle_index < number_of_particles; 
       ++particle_index) {
      manager->releaseOrbitingParticleData(particles_[particle_index]);
    }
    number_of_particles_ = 0;
  }
}


bool SpecialFXController_OrbitingParticle::update(double time,
                                                  double time_since_last_update,
                                                  GlobalSpecialFXManager* manager) {
  if (!binding_ || binding_->getExpired()) return true;

  float src_x, src_y;
  binding_->get(&src_x, &src_y, NULL);

  // Advance particles
  int number_of_particles = number_of_particles_;
  for (int particle_index = 0; particle_index < number_of_particles;) {
    OrbitingParticleData* particle = particles_[particle_index];

    double time_until_dying = particle->dying_time - time;
    bool dying = time_until_dying < 0.0;
    if (dying && (particle->rendering_particle.point_size <= 0.0f)) {
      manager->releaseOrbitingParticleData(particle);

      // Swap down into this location
      --number_of_particles;
      particles_[particle_index] = particles_[number_of_particles];
      particles_[number_of_particles] = NULL;

      continue;
    }
    else
    {
      // Advance the particle's features
      float new_angle = particle->angle_from_source + params_->orbitSpeed * time_since_last_update;
      float new_height = particle->rendering_particle.y + params_->risingSpeed * time_since_last_update;
      float new_distance = particle->distance_from_source + params_->spreadingSpeed * time_since_last_update;

      // Move this particle
      particle->rendering_particle.y = new_height;
      particle->rendering_particle.x = src_x + cosf(new_angle) * new_distance;
      particle->rendering_particle.z = src_y + sinf(new_angle) * new_distance;
      particle->angle_from_source = new_angle;
      particle->distance_from_source = new_distance;

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
    bool finished_generating = time > stop_generation_time_;
    if (time < next_generation_time_ || finished_generating) {
      number_of_particles_ = number_of_particles;
      return finished_generating && (number_of_particles == 0);
    }
    next_generation_time_ = time + params_->spawnPeriod;
  }

  // Generate new particles
  for (int j = 0; j < params_->particlesPerPeriod &&
          number_of_particles < MAX_PARTICLES; ++j) {
    OrbitingParticleData* particle = manager->acquireOrbitingParticleData();
    CONFIRM(particle) else {
    break;
    }

    float particle_angle = (rand()%RAND_MAX)/((float)RAND_MAX) * 2 * 3.14;
    float initial_distance = params_->initialDistance;

    particle->rendering_particle.x = src_x + cosf(particle_angle) * initial_distance;
    particle->rendering_particle.y = params_->initialHeight;
    particle->rendering_particle.z = src_y + sinf(particle_angle) * initial_distance;
    particle->rendering_particle.color = params_->particleColors[rand()%3];
    particle->dying_time = time + params_->lifetime;
    particle->angle_from_source = particle_angle;
    particle->distance_from_source = initial_distance;
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



bool SpecialFXController_OrbitingParticle::renderFX(
  int image_id,
  IsometricCamera* camera,
  GlobalSpecialFXManager* manager) {

  OrbitingParticleData** orbiting_particles = particles_;
  int particles_to_draw = number_of_particles_;
  while (particles_to_draw > 0) {
    RenderingParticle* rendering_particles = NULL;
    int particles_available = 0;
    if (!manager->lockParticles(particles_to_draw,
                  &rendering_particles,
                  &particles_available))
      return false;
    for (int i = 0; i < particles_available; ++i) {
      OrbitingParticleData* orbiting_particle = *orbiting_particles;
      memcpy(rendering_particles,
        &orbiting_particle->rendering_particle,
         sizeof(RenderingParticle));
      ++orbiting_particles;
      ++rendering_particles;
    }
    particles_to_draw -= particles_available;
    manager->unlockParticles();
  }

  return true;
}






}