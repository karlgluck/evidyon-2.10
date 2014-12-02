#include "globalmagicmanager.h"
#include "../common/null.h"
#include "randf.h"
#include "instantiatedmagic.h"
#include "actor.h"

#include <memory.h> // memset
#include <dc/debug>

namespace Evidyon {

GlobalMagicManager* GlobalMagicManager::global = NULL;

GlobalMagicManager* GlobalMagicManager::singleton() {
  return GlobalMagicManager::global;
}

bool GlobalMagicManager::create() {
  if (global != NULL || !instances_.create() ||
    !projectiles_.create() || !timers_.create()) {
  destroy();
  return false;
  }

  random_floats_.randomize();

  global = this;

  return true;
}

void GlobalMagicManager::destroy() {
  instances_.destroy();
  projectiles_.destroy();
  timers_.destroy();
  global = NULL;
}

void GlobalMagicManager::update(GlobalWorldManager* world_manager,
                                GlobalActorManager* actor_manager,
                                double time, double time_since_last_update) {

  for (ProjectilesIter i(&projectiles_); i.hasNext(); i.advance()) {
    MagicTargeterProjectile* projectile = i.get();
    if (projectile->update(this,
                           time, time_since_last_update)) {
      projectile->reset();
      i.remove();
    }
  }

  for (TimersIter i(&timers_); i.hasNext(); i.advance()) {
    MagicTargeterTimer* timer = i.get();
    if (timer->update(this, time)) {
      timer->reset();
      i.remove();
    }
  }

  // Get rid of instances without references
  for (InstancesIter i(&instances_); i.hasNext(); i.advance()) {
    InstantiatedMagic* instance = i.get();
    if (instance->references <= 0) {
      i.remove();
    }
  }
}

InstantiatedMagic* GlobalMagicManager::createInstance() {
  InstantiatedMagic* instance = instances_.acquire();
  if (instance == NULL) return NULL;
  memset(instance, 0, sizeof(InstantiatedMagic));
  return instance;
}

void GlobalMagicManager::spawnProjectileTargeters(
  Actor* visualization_broadcaster, float src_x, float src_y,
  int count, float direction, float half_spread, double expiration_time,
  InstantiatedMagic* source, int stage) {
  for (int i = 0; i < count; ++i) {
  float projectile_angle = random_floats_.next()*half_spread + direction;
  MagicTargeterProjectile* projectile = projectiles_.acquire();
  if (!projectile) break;
  projectile->initializeMagic(source, stage);
  projectile->initializeProjectile(
      src_x, src_y, projectile_angle, expiration_time,
      visualization_broadcaster->getRegion(),
      source->magic.description->stages[stage].per_targeter_fx_type
     );
  }
}

void GlobalMagicManager::spawnProjectileTargeter(Actor* actor_source,
  float half_spread, double expiration_time,
  InstantiatedMagic* source_primary_stage) {
  float projectile_angle = random_floats_.next()*half_spread + actor_source->getDirection();
  MagicTargeterProjectile* projectile = projectiles_.acquire();
  if (projectile) {
    float src_x, src_y;
    actor_source->getPosition(&src_x, &src_y);
    projectile->initializeMagic(source_primary_stage, 0);
    projectile->initializeProjectile(
        src_x, src_y, projectile_angle, expiration_time,
        actor_source->getRegion(),
        source_primary_stage->magic.description->stages[0].per_targeter_fx_type);
    }
}

void GlobalMagicManager::spawnTimerTargeter(
  Actor* visualization_source,
  Actor* targets[MagicTargeterTimer::MAX_TARGETS],
  int number_of_targets, InstantiatedMagic* source,
  double time_to_apply, int stage) {
  MagicTargeterTimer* timer = timers_.acquire();
  if (timer) {
    targets[0]->broadcastMagicFX(
      source->magic.description->stages[stage].per_targeter_fx_type,
      visualization_source,
      source->magic.x,
      source->magic.y,
      source->visualization_power_parameter,
      targets,
      number_of_targets);
    timer->initializeMagic(source, stage);
    timer->initializeTimer(targets, number_of_targets, time_to_apply);
  }
}

void GlobalMagicManager::spawnTimerTargeter(
  Actor* visualization_source,
  Actor* target,
  InstantiatedMagic* source,
  double time_to_apply, int stage) {
  MagicTargeterTimer* timer = timers_.acquire();
  if (timer) {
    target->broadcastMagicFX(
      source->magic.description->stages[stage].per_targeter_fx_type,
      visualization_source,
      source->magic.x,
      source->magic.y,
      source->visualization_power_parameter,
      &target, 1);
    timer->initializeMagic(source, stage);
    timer->initializeTimer(&target, 1, time_to_apply);
  }
}

}