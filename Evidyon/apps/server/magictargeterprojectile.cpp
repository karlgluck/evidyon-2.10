#include "magictargeterprojectile.h"
#include <math.h>
#include "globalworldmanager.h"
#include "../common/null.h"
#include "actor.h"
#include "../shared/evidyon_magic.h"
#include "globalactormanager.h"
#include "worldregion.h"
#include "encodepackets.h"

#include <dc/debug>

namespace Evidyon {

int MagicTargeterProjectile::next_projectile_id_ = 0;

MagicTargeterProjectile::MagicTargeterProjectile() {
  projectile_id_ = 0;
  current_region_ = NULL;
}

void MagicTargeterProjectile::initializeProjectile(float x,
                                                   float y,
                                                   float direction,
                                                   double expiration_time,
                                                   WorldRegion* local_region,
                                                   int projectile_type) {
  projectile_id_ = ++next_projectile_id_;
  expiration_time_ = expiration_time;
  current_region_ = local_region;
  dx_ = cosf(direction);
  dy_ = sinf(direction);

  // advance the projectile a short distance in the given direction
  x = x + dx_ * 0.5f;
  y = y + dy_ * 0.5f;

  {
    x_ = x;
    y_ = y;
    dx_ *= MAGIC_PROJECTILE_SPEED;
    dy_ *= MAGIC_PROJECTILE_SPEED;
  }

  {
    using namespace ::Evidyon::Network::Packet::Server::Encode;
    CONFIRM(local_region) else return;
    NetworkPacket* packet = createProjectile(projectile_id_,
                                             projectile_type,
                                             x,
                                             y,
                                             dx_,
                                             dy_);
    local_region->broadcastOrphanedNetworkPacket(packet);
  }
}


void MagicTargeterProjectile::broadcastErasure() {
  using namespace ::Evidyon::Network::Packet::Server::Encode;
  CONFIRM(current_region_) else return;
  NetworkPacket* packet = destroyProjectile(projectile_id_);
  current_region_->broadcastOrphanedNetworkPacket(packet);
}

bool MagicTargeterProjectile::update(GlobalMagicManager* magic_manager,
                                     double time, double time_delta) {
  float dx = (float)(dx_ * time_delta);
  float dy = (float)(dy_ * time_delta);
  Actor* actor_hit = NULL;
  bool erase = false;
  float x = x_ + dx;
  float y = y_ + dy;

  // Find the region that this projectile is in
  WorldRegion* region = current_region_;

  if (time > expiration_time_ || !region) {
    erase = true;
  } else {
    region->getMap()->updateRegion(x, y, current_region_, &region);
    current_region_ = region; // this could be NULL!

    if (region) {
      actor_hit = region->collideProjectileWithActor(x_, y_, x, y);
      if (NULL == actor_hit) {
        //erase = region->collideProjectileWithMap(x, y);
        const MapLocationInfo* location = region->getMap()->getMapLocationInfo(x, y);
        if ((location != NULL) &&
            (false == location->pass_by_spirit)) {
          erase = true;
        }
      } else {
        erase = true;

        const MagicDescriptionStageEffect::Parameters* params;
        const MagicDescriptionStageEffect* effect =
            MagicTargeter::getStageEffect(&params);
        actor_hit->applyMagicEffect(getCaster(), time, getMagic(), effect, params);
        actor_hit->setAction(ACTORACTION_GOTHIT);
      }

      if (erase) {
        spawnNextStageTargeters(time, magic_manager, x, y);
      }
    }
  }


  if (erase) { // Remove the projectile?
    broadcastErasure();
    return true;
  }

  // Advance the location
  x_ = x;
  y_ = y;
  return false;
}

}