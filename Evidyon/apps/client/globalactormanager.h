#ifndef __GLOBALACTORMANAGER_H__
#define __GLOBALACTORMANAGER_H__


#include "actor.h"
#include "actortypedescription.h"
#include <hash_map>
#include <map>
#include <set>
#include "../shared/networkdecl.h"
#include "../shared/actorid.h"
#include "../shared/actordescriptiontemplate.h"


namespace Evidyon {
class GlobalRenderingManager;
struct ClientActorDescriptionTemplate;
class GlobalSoundManager;
class GlobalSpecialFXManager;

namespace Network {
namespace Packet {
struct ActorSyncData;
struct DecodedFullActorDescription;
}
}

class GlobalActorManager {
  typedef std::vector<ActorTypeDescription> ActorTypeDescriptions;
  typedef std::vector<ClientActorDescriptionTemplate> ActorDescriptionTemplates;
  typedef std::multimap<size_t,std::pair<Actor*,size_t>> TextureIndexToActorMeshTextureSubsetTable;
  typedef std::set<Actor*> ActorSet;
public:
  typedef stdext::hash_map<ActorID,Actor*> ActorMap;
  typedef std::set<ActorID> ActorIDSet;

public:

  // Initializes this class for the first time, or shuts it down when the class
  // is no longer needed.  Each should only be done once.
  bool create(SceneryRenderer* scenery_renderer,
              GlobalSoundManager* sound_manager,
              GlobalSpecialFXManager* special_fx_manager);
  void destroy();

  // Call this method when loading data from the game file to add a new actor
  // type to the class.  This should be called in ascending sequential order.
  // The 'index' parameter is only used for validation.
  bool addActorType(int index,
                    const ActorTypeDescription& actor_type);

  bool addActorDescriptionTemplate(int index,
                                   const ClientActorDescriptionTemplate& description_template);

  void assignSoundIndices(unsigned int footstep_sound);
  void assignFXIndices(unsigned int physical_hit_magic_fx);

  // Erases all of the actors and clears out the class
  void reset();
  void onLostDevice();

  // Called when the map changes.  This method erases all actors that are not
  // the client's actor and frees the data they used.
  void onChangeMap();

  void setActorDescription(int id, int actor_type_index);

  // Returns the actor for which the description was set
  Actor* setActorDescription(::Evidyon::Network::Packet::DecodedFullActorDescription* description);

  void update(double time, double time_since_last_update);
  bool render(LPDIRECT3DDEVICE9 d3d_device,
              int texture_id,
              IsometricCamera* camera,
              GlobalRenderingManager* global_rendering_manager);

  void release(ActorID id);
  Actor* get(ActorID id);

  bool findActorNearLocation(float x, float y, ActorID actor_to_ignore, ActorID* id, bool find_living_only);
  bool findActorNearLocation(float x, float y, ActorID actor_to_ignore, Actor** actor, bool find_living_only);
  bool findLivingNonclientActorNearLocation(float x, float y, Actor** actor);

  void slowSync(float time_since_last_sync,
                double time,
                ::Evidyon::Network::Packet::ActorSyncData* slow_sync,
                std::set<ActorID>* actors_without_names);

  // Sync this client's actor's location
  void fastSync(float time_since_last_sync, double time, float x, float y, Evidyon::Network::Packet::ActorSyncData::Flags flags);

  bool actorsNeedDescriptions() const;
  NetworkPacket* buildRequestActorDescriptionPacket();

  // This method is called after a slowsync has been received.  This method
  // makes sure that actors that are no longer being updated are erased from
  // the world.  Not doing this could create zombies!
  //void removeActorsNotInSet(const ActorIDSet* actors);


public: // debugging
  void printDebugStats(char* buffer, size_t buffer_size) const;


public:
  inline Actor* getClientActor() { return client_actor_; }

  // These iterators allow rendering of names and speech text
  inline ActorMap::const_iterator allActorsBegin() { return actor_map_.begin(); }
  inline ActorMap::const_iterator allActorsEnd() { return actor_map_.end(); }

  Actor* acquireClientActor(ActorID id, float x, float y, const char* name);

private:
  // Returns 'true' if the actor was created by this call
  bool acquire(ActorID id, Actor** actor);
  void addDescriptionlessActor(Actor* actor); // this actor needs its description updated
  void eraseFromRenderingTable(Actor* actor);
  void setActorType(Actor* actor, int type);

  void handleActorSyncAction(double time, Actor* actor, const ::Evidyon::Network::Packet::ActorSyncData::Flags sync_flags);

  // Starts playing the sound associated with the given action type
  void playActionSound(float x, float y, bool client_actor, const ActorTypeDescription* description, ActorAction action);

private:
  Actor* client_actor_;
  ActorMap actor_map_;
  TextureIndexToActorMeshTextureSubsetTable rendering_table_;
  ActorTypeDescriptions actor_type_descriptions_;
  ActorDescriptionTemplates actor_description_templates_;
  GlobalSoundManager* sound_manager_;

  // Used to draw scenery to the screen
  SceneryRenderer* scenery_renderer_;

  // Actors in this set don't yet have a description and one has not been requested
  ActorSet actors_without_descriptions_;

  // Deceremented each fastSync when need_descriptions_ has members.  When this
  // reaches zero, a "request actor descriptions" packet needs to be built and
  // sent to the server.
  int fastsyncs_until_description_request_;

  unsigned int footstep_sound_;
  unsigned int physical_hit_magic_fx_;

  GlobalSpecialFXManager* special_fx_manager_;
};

}



#endif