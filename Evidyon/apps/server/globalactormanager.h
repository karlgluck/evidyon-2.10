#ifndef __GLOBALACTORMANAGER_H__
#define __GLOBALACTORMANAGER_H__

#include <map>
#include <hash_map>
#include <dcx/managedmemorypool>
#include "actor.h"
#include "../shared/actorid.h"

struct _iobuf;
typedef struct _iobuf FILE;

namespace Evidyon {

class WorldRegion;
class Map;

struct ServerActorDescriptionTemplate;

class GlobalActorManager {

  // The maximum number of simultaneous actors is defined by the field in the
  // actor ID reserved for their index in this pool.
  typedef dcx::dcxManagedMemoryPool<Actor, MAX_ACTORS_IN_WORLD> ActorMemoryPool;

  // It is safe to use the raw Actor* pointer here, since this class controls
  // access to all of the Actor* instances.  As a result, we can be 100%
  // confident that no actor will be removed without the pointers being
  // erased from these tables.

  // Contains all of the "spread" regions that the actor is inside
  typedef std::multimap<WorldRegion*,Actor*> Regions;
  typedef std::map<Regions::value_type,Regions::iterator> RegionValueIterators;

  typedef stdext::hash_map<ActorID,Actor*> ActiveActors;

public:
  // This is a globally-standardized number of entries that should be allowed
  // in a search for actors; i.e. creating a static buffer of this size when
  // doing a search for actors is usually adequate.
  static const int STD_ACTOR_SEARCH_BUFFER_SIZE = 16;

public:
  typedef ActorMemoryPool::Pointer ActorPointer;

public:
  static GlobalActorManager* singleton();

public:

  GlobalActorManager();
  bool create(FILE* game_file);
  void destroy();

  // All non-sequential uses of any Actor pointer should use the 'ActorPointer'
  // type.  Using this method allows such pointers to be automatically
  // invalidated when the actor is released.
  // The description_template parameter is a reference to the client-side
  // ClientActorDescriptionTemplate on which this actor is based.
  bool acquireActor(ActorPointer* pointer,
                    ActorController* controller,
                    unsigned int description_template,
                    Map* map, float x, float y);
  void releaseActor(ActorPointer* pointer);
  void getPointer(Actor* actor, ActorPointer* pointer);
  void getPointer(int actor_id, ActorPointer* pointer);

  Actor* getActor(ActorID id);

  void setSyncGroup(int sync_group);

  const ServerActorDescriptionTemplate* getDescriptionTemplate(size_t template_index) const;

private:

  ActorMemoryPool actor_memory_pool_;
  int next_available_actor_id_;

  ActiveActors active_actors_;

  // Actors are based on one of these template types.  This data
  // is read from the game file.
  ServerActorDescriptionTemplate* actor_description_templates_;
  size_t number_of_actor_description_templates_;

private:
  static GlobalActorManager* singleton_;
};


}

#endif