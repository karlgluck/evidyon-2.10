#ifndef __GLOBALMAGICMANAGER_H__
#define __GLOBALMAGICMANAGER_H__

#include "magictargeter.h"
#include "magictargetertimer.h"
#include "magictargeterprojectile.h"
#include <dcx/memorypool>
#include "randf.h"

namespace Evidyon {

class GlobalWorldManager;

class GlobalMagicManager {
  static const int MAX_INSTANCES = 1024;
  static const int MAX_PROJECTILE_TARGETERS = MAX_INSTANCES * 8;
  static const int MAX_DELAYED_TARGETERS = MAX_INSTANCES * 4;
  typedef dcx::dcxMemoryPool<InstantiatedMagic,MAX_INSTANCES> Instances;
  typedef Instances::Iterator InstancesIter;
  typedef dcx::dcxMemoryPool<MagicTargeterProjectile,MAX_PROJECTILE_TARGETERS> Projectiles;
  typedef Projectiles::Iterator ProjectilesIter;
  typedef dcx::dcxMemoryPool<MagicTargeterTimer,MAX_DELAYED_TARGETERS> Timers;
  typedef Timers::Iterator TimersIter;

public:

  // Obtains a pointer to the global magic manager singleton.  The manager
  // must be explicitly instantiated somewhere in the program before this
  // method will return anything valid.
  static GlobalMagicManager* singleton();

public:

  // Sets up the memory pools and initializes the singleton.  This method
  // fails if the singleton already exists.
  bool create();

  // Frees the memory pools
  void destroy();

  // Advances everything, then releases instances without references
  void update(GlobalWorldManager* world_manager,
        GlobalActorManager* actor_manager, double time,
        double time_since_last_update);

  // Generates a new instance for magic (starts with 1 reference--
  // be sure to remove it when the pointer is cleared)
  InstantiatedMagic* createInstance();

  // Creates 'count' projectile targeters and sends the effect visualization
  // messages to create those projectiles on clients.
  void spawnProjectileTargeters(Actor* visualization_broadcaster,
    float src_x, float src_y, int count,
    float direction, float half_spread, double expiration_time,
    InstantiatedMagic* source, int stage);

  // Creates a stage-zero projectile targeter emitted from the actor in the
  // direction that it is currently facing.
  void spawnProjectileTargeter(Actor* actor_source, float half_spread,
    double expiration_time, InstantiatedMagic* source_primary_stage);

  // Creates timer targeters for the targets.  Will create multiple targeters
  // as necessary to hold all of the targets.
  //void spawnMultipleTimerTargeter(
  //  Actor* visualization_source,
  //  Actor** targets,
  //  int number_of_targets, InstantiatedMagic* source,
  //  double time_to_apply, int stage);

  // Creates a timer targeter for all of the targets.  This will create
  // effect visualizations as necessary.
  void spawnTimerTargeter(
    Actor* visualization_source,
    Actor* targets[MagicTargeterTimer::MAX_TARGETS],
    int number_of_targets, InstantiatedMagic* source,
    double time_to_apply, int stage);

  void spawnTimerTargeter(
    Actor* visualization_source,
    Actor* target, InstantiatedMagic* source,
    double time_to_apply, int stage);

private:
  Instances instances_;
  Projectiles projectiles_;
  Timers timers_;

  // Iterates through a pregenerated set of random float values.  Used to
  // speed up the generation of projectile directions.
  RandSymmetricFloatTable<32> random_floats_;

private:
  static GlobalMagicManager* global;
};

}

#endif