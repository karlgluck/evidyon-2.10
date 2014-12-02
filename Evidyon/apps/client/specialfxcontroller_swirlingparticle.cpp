#include "specialfxcontroller_swirlingparticle.h"
#include "../shared/compiledspecialfx.h"
#include "globalspecialfxmanager.h"
#include "actor.h"
#include "specialfxbinding.h"
#include <math.h>
#include <dc/debug>



namespace Evidyon {

SpecialFXController_SwirlingParticle::SpecialFXController_SwirlingParticle(
    double time,
    SpecialFXBinding* binding,
    double duration,
    const CompiledVisualFXParams_SwirlingParticles* params) {
  memset(particles_, 0, sizeof(particles_));
  number_of_particles_ = 0;
  stop_generation_time_ = time + duration;
  (binding_ = binding)->addReference();
  params_ = params;
}

void SpecialFXController_SwirlingParticle::destroy(GlobalSpecialFXManager* manager) {
  if (binding_) {
    binding_->removeReference();
    binding_ = NULL;
  }

  {
    const int number_of_particles = number_of_particles_;
    for (int particle_index = 0;
       particle_index < number_of_particles; 
       ++particle_index) {
      manager->releaseSwirlingParticleData(particles_[particle_index]);
    }
    number_of_particles_ = 0;
  }
}


bool SpecialFXController_SwirlingParticle::update(double time,
                                                  double time_since_last_update,
                                                  GlobalSpecialFXManager* manager) {
  if (!binding_) return true;

  float src_x, src_y;
  binding_->get(&src_x, &src_y, NULL);

  double time_after_killed = time - stop_generation_time_;
  bool kill_particles = time_after_killed > 0.0;

  if (!kill_particles && binding_->getExpired()) {
    time_after_killed = 0.0;
    stop_generation_time_ = time;
    kill_particles = true;
  }

  // Advance particles
  int number_of_particles = number_of_particles_;
  for (int particle_index = 0; particle_index < number_of_particles;) {
    SwirlingParticleData* particle = particles_[particle_index];
    const float angle = particle->angle_from_source + particle->speed * time_since_last_update;
    particle->angle_from_source = angle;
    particle->rendering_particle.x = src_x + cosf(angle) * params_->orbit_radius;
    particle->rendering_particle.y = particle->height;
    particle->rendering_particle.z = src_y + sinf(angle) * params_->orbit_radius;


    if (kill_particles) {
      if (particle->rendering_particle.point_size <= 0.0f) {
        manager->releaseSwirlingParticleData(particle);

        // Swap down into this location
        --number_of_particles;
        particles_[particle_index] = particles_[number_of_particles];
        particles_[number_of_particles] = NULL;

        continue;
      }
      else
      {
        static const float DECAY_RATE = -0.05f;
          particle->rendering_particle.point_size =
            params_->particle_size + DECAY_RATE * time_after_killed;

        ++particle_index; // advance to the next particle index
      }
    } else {
      ++particle_index;
    }
  }

  // Check to see if new particles need to be generated
  if (number_of_particles > 0) return false;
  if (kill_particles) return true; // are we done with this effect?


  // Generate the particles
  const int number_of_rings = params_->number_of_rings;
  for (int ring = 0; ring < number_of_rings; ++ring) {
    const int particles_per_ring = params_->particles_per_ring;
    const float ring_offset = ring == 0 ? 0.0f : (((ring%3)*(D3DX_PI/3.0f*2.0f))+(((rand()/(float)RAND_MAX)*D3DX_PI/3.0f-D3DX_PI/6.0f)));
    const float speed = params_->min_speed;//+ (rand()/(float)RAND_MAX) * params_->speed_range;
    for (int p = 0; p < particles_per_ring; ++p) {
      SwirlingParticleData* particle = manager->acquireSwirlingParticleData();
      CONFIRM(particle) else break;

      particle->rendering_particle.color = params_->particle_colors[rand()%3];
      particle->angle_from_source = ring_offset + (p == 0 ? 0.0f : (p / (float)(particles_per_ring - 1)));
      particle->height = params_->lowest_ring_height + ring * params_->added_height_per_ring;
      particle->speed = speed;
      particle->rendering_particle.x = src_x;
      particle->rendering_particle.y = 0.0f;
      particle->rendering_particle.z = src_y;
      particle->rendering_particle.point_size = params_->particle_size;

      // Add to particle list
      particles_[number_of_particles++] = particle;
    }
  }

  number_of_particles_ = number_of_particles;

  return false;
}



bool SpecialFXController_SwirlingParticle::renderFX(
  int image_id,
  IsometricCamera* camera,
  GlobalSpecialFXManager* manager) {

  SwirlingParticleData** swirling_particles = particles_;
  int particles_to_draw = number_of_particles_;
  while (particles_to_draw > 0) {
    RenderingParticle* rendering_particles = NULL;
    int particles_available = 0;
    if (!manager->lockParticles(particles_to_draw,
                  &rendering_particles,
                  &particles_available))
      return false;
    for (int i = 0; i < particles_available; ++i) {
      SwirlingParticleData* swirling_particle = *swirling_particles;
      memcpy(rendering_particles,
        &swirling_particle->rendering_particle,
         sizeof(RenderingParticle));
      ++swirling_particles;
      ++rendering_particles;
    }
    particles_to_draw -= particles_available;
    manager->unlockParticles();
  }

  return true;
}






}