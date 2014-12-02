#ifndef __SPECIALFXCONTROLLER_ORBITINGPARTICLE_H__
#define __SPECIALFXCONTROLLER_ORBITINGPARTICLE_H__

#include "specialfxcontroller.h"

namespace Evidyon {

struct OrbitingParticleData;
struct CompiledVisualFXParams_OrbitingParticles;

class SpecialFXBinding;


class SpecialFXController_OrbitingParticle : public SpecialFXController {
  static const int MAX_PARTICLES = 500;

public:
  SpecialFXController_OrbitingParticle(
    double time,
    SpecialFXBinding* binding,
    double duration, // use params->duration as default
    const CompiledVisualFXParams_OrbitingParticles* params);

  virtual void destroy(GlobalSpecialFXManager* manager);
  virtual bool update(double time,
            double time_since_last_update,
            GlobalSpecialFXManager* manager);
  virtual bool renderFX(int image_id,
            IsometricCamera* camera,
            GlobalSpecialFXManager* manager);


private:
  OrbitingParticleData* particles_[MAX_PARTICLES];
  int number_of_particles_;

  double stop_generation_time_, next_generation_time_;
  SpecialFXBinding* binding_;
  const CompiledVisualFXParams_OrbitingParticles* params_;
};



}



#endif