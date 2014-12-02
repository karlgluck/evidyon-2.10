#ifndef __MAGICTARGETER_H__
#define __MAGICTARGETER_H__

#include "../shared/server-editor/magicdescription.h"

namespace Evidyon {

struct InstantiatedMagic;
class GlobalMagicManager;
class Actor;
struct Magic;

class MagicTargeter {
public:
  MagicTargeter();
  void initializeMagic(InstantiatedMagic* magic_instance, int stage);
  void reset();

protected:

  void spawnNextStageTargeters(double time, GlobalMagicManager* manager, Actor** sources, int num_sources);
  void spawnNextStageTargeters(double time, GlobalMagicManager* manager, float source_x, float source_y);

  // Gets the effect that the targeter should have on its target
  const MagicDescriptionStageEffect* getStageEffect(const MagicDescriptionStageEffect::Parameters** parameters);
  const Magic* getMagic();
  Actor* getCaster();

private:

  // The source instance of which this targeter is controlling the effect
  InstantiatedMagic* magic_instance_;

  // The stage of the source instance that this targeter was spawned by
  int stage_;
};


}

#endif