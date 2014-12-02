#ifndef __MAGICTARGETERPROJECTILE_H__
#define __MAGICTARGETERPROJECTILE_H__

#include "magictargeter.h"

namespace Evidyon {

class Actor;
class GlobalWorldManager;
class GlobalActorManager;
class WorldRegion;

// Note: when this targeter is created, the a projectile generation command
// is sent to the client.  It is important that the client not try to
// back-interpolate this projectile's position, because the targeter will
// send out a non-predictive collision when it hits a target.  This way,
// all clients (even with different RTTs) see the collision at the same
// time--and, if their latency is close to that when the targeter was
// created, it will be at the correct time!
class MagicTargeterProjectile : public MagicTargeter {
public:
  static const float SPEED;
  static const double DURATION;

public:
  static int next_projectile_id_;

public:
  MagicTargeterProjectile();

  // This automatically generates a projectile ID number
  void initializeProjectile(float x, float y, float direction,
                            double expiration_time,
                            WorldRegion* local_region,
                            int projectile_type);
  void broadcastErasure();

  // Advances this projectile.  If this method returns 'true',
  // the container class should remove the projectile from the world
  // and get rid of this class instance.
  // This method is responsible for applying the effect of the targeter via
  // the manager, and for creating subsequent targeters if necessary.
  bool update(GlobalMagicManager* magic_manager,
              double time, double time_delta);

private:
  int projectile_id_;  // The ID of this projectile reported to clients
  double expiration_time_;
  WorldRegion* current_region_;
  float x_, y_;
  float dx_, dy_;
};



}

#endif