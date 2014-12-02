#ifndef __SPECIALFXCONTROLLER_SWIRLINGPARTICLE_H__
#define __SPECIALFXCONTROLLER_SWIRLINGPARTICLE_H__

#include "specialfxcontroller.h"

namespace Evidyon {

struct SwirlingParticleData;
struct CompiledVisualFXParams_SwirlingParticles;

class SpecialFXBinding;


class SpecialFXController_SwirlingParticle : public SpecialFXController {
  static const int MAX_PARTICLES = 100;

public:
  SpecialFXController_SwirlingParticle(
      double time,
      SpecialFXBinding* binding,
      double duration, // use params->duration as default
      const CompiledVisualFXParams_SwirlingParticles* params);

  virtual void destroy(GlobalSpecialFXManager* manager);
  virtual bool update(double time,
            double time_since_last_update,
            GlobalSpecialFXManager* manager);
  virtual bool renderFX(int image_id,
            IsometricCamera* camera,
            GlobalSpecialFXManager* manager);


private:
  SwirlingParticleData* particles_[MAX_PARTICLES];
  int number_of_particles_; // all particles are generated at instantiation

  double stop_generation_time_;
  SpecialFXBinding* binding_;
  const CompiledVisualFXParams_SwirlingParticles* params_;
};



}



#endif