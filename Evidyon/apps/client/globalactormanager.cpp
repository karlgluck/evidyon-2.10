#include "../shared/client-server/actorsync.h"
#include "./packets/tellactorfulldescriptions-decoder.h"
#include "globalactormanager.h"
#include "gamefiledata_skinnedmesh.h"
#include "encodepackets.h"

#include "../shared/client-server/askactorfulldescriptions.h"
#include "globalrenderingmanager.h"
#include "../shared/evidyon_network.h"

#include <dc/debug>

#include "globalsoundmanager.h"

#include "sceneryrenderer.h"

#include "globalspecialfxmanager.h"


namespace Evidyon {

static const int FASTSYNCS_TO_WAIT_BEFORE_REQUESTING_DESCRIPTION = 3;


bool GlobalActorManager::create(SceneryRenderer* scenery_renderer,
                                GlobalSoundManager* sound_manager,
                                GlobalSpecialFXManager* special_fx_manager) {
  reset();
  client_actor_ = NULL;
  fastsyncs_until_description_request_ =
    FASTSYNCS_TO_WAIT_BEFORE_REQUESTING_DESCRIPTION;
  scenery_renderer_ = scenery_renderer;
  sound_manager_ = sound_manager;
  special_fx_manager_ = special_fx_manager;
  return true;
}

void GlobalActorManager::destroy() {
  client_actor_ = NULL;
  rendering_table_.clear();
  for (ActorMap::iterator i = actor_map_.begin(); i != actor_map_.end(); ++i) {
    Actor* actor = i->second;
    actor->release(scenery_renderer_);
    delete actor;
  }
  scenery_renderer_ = NULL;
  actor_map_.clear();
  actor_type_descriptions_.clear();
}

bool GlobalActorManager::addActorType(
    int index,
    const ActorTypeDescription& actor_type) {
  CONFIRM(actor_type_descriptions_.size() == index) else return false;
  actor_type_descriptions_.push_back(actor_type);
  return true;
}

bool GlobalActorManager::addActorDescriptionTemplate(int index,
                                                     const ClientActorDescriptionTemplate& description_template) {
  CONFIRM(actor_description_templates_.size() == index) else return false;
  actor_description_templates_.push_back(description_template);
  return true;
}

void GlobalActorManager::assignSoundIndices(unsigned int footstep_sound) {
  footstep_sound_ = footstep_sound;
}

void GlobalActorManager::assignFXIndices(unsigned int physical_hit_magic_fx) {
  physical_hit_magic_fx_ = physical_hit_magic_fx;
}


void GlobalActorManager::reset() {
  client_actor_ = NULL;
  rendering_table_.clear();
  actors_without_descriptions_.clear();
  for (ActorMap::iterator i = actor_map_.begin(); i != actor_map_.end(); ++i) {
    Actor* actor = i->second;
    actor->release(scenery_renderer_);
    delete actor;
  }
  actor_map_.clear();
}

void GlobalActorManager::onLostDevice() {
  reset();
  actor_type_descriptions_.clear();
  actor_description_templates_.clear();
}



void GlobalActorManager::onChangeMap() {
  Actor* client_actor = client_actor_;
  for (ActorMap::iterator i = actor_map_.begin(); i != actor_map_.end(); ++i) {
    Actor* actor = i->second;
    if (actor == client_actor) continue; // don't erase the client's actor
    eraseFromRenderingTable(actor);
    actor->release(scenery_renderer_);
    delete actor;
  }
  actor_map_.clear();
  actors_without_descriptions_.clear();

  // make sure the client's actor is still there
  actor_map_.insert(ActorMap::value_type(client_actor->getID(), client_actor));
}



void GlobalActorManager::setActorDescription(int id,
                                             int actor_type_index) {
  Actor* actor = get(id);

  if (actor) {
    CONFIRM(actor_type_index >= 0 &&
        actor_type_index < actor_type_descriptions_.size()) else return;
    eraseFromRenderingTable(actor);

    const ActorTypeDescription* actor_type =
      &actor_type_descriptions_.at(actor_type_index);

    actor->setDescription(actor_type);

    rendering_table_.insert(
      TextureIndexToActorMeshTextureSubsetTable::value_type(
      actor_type->texture_id, std::pair<Actor*,size_t>(actor, 0)));
  }
}



Actor* GlobalActorManager::setActorDescription(
  ::Evidyon::Network::Packet::DecodedFullActorDescription* description) {
  Actor* actor = NULL;
  bool created = acquire(description->data.sync_.id,
                        &actor);
  if (actor) {

    // Only jerk the actor to the sync location if it was just created
    if (created) {
      actor->setPosition(description->data.sync_.x_packed * Evidyon::Network::FLOAT_UNPACK_MULTIPLIER,
                         description->data.sync_.y_packed * Evidyon::Network::FLOAT_UNPACK_MULTIPLIER);
    }

    const ClientActorDescriptionTemplate* description_template =
        actor->getDescriptionTemplate();

    // Add this actor to the renderer
    setActorType(actor,
                 description_template->description_can_change ? 
                     description->data.state_.actor_type :
                     description_template->actor_type);

    actors_without_descriptions_.erase(actor);

    actor->setCorporealState(
      static_cast<CorporealState>(description->data.state_.corporeal_state));
  }

  return actor;
}


void GlobalActorManager::update(double time,
                                double time_since_last_update) {
  const float VISIBLE_DISTANCE_SQ = 8.0f*8.0f+8.0f*8.0f;

  static size_t members = rendering_table_.size();
  if (rendering_table_.size() != members) {
    //DEBUG_INFO("%.02f : actor map has %lu members", (float)time, members);
    members = rendering_table_.size();
  }

  for (ActorMap::iterator i = actor_map_.begin(); i != actor_map_.end();) {
    Actor* actor = i->second;

    // has this actor not been updated in a while?  if so, erase it
    if (actor != client_actor_ && actor->expired(time)) {
      ActorMap::iterator next = i; ++next;
      eraseFromRenderingTable(actor);
      actors_without_descriptions_.erase(actor);
      actor->release(scenery_renderer_);
      delete actor;
      actor_map_.erase(i);
      i = next;
      continue;
    } else {
      ++i;
    }

    bool is_possibly_visible =
      (actor->getCorporealState() != SPIRIT_FORM) &&
      (actor->distanceToSq(client_actor_) < VISIBLE_DISTANCE_SQ);

    //if (is_possibly_visible) {
      actor->update(scenery_renderer_, time, time_since_last_update);
    /*} else {
      actor->updateInvisible(time);
    }

    if (actor->isRendering()) {
      if (!is_possibly_visible) {
        eraseFromRenderingTable(actor);
      }
    } else if (is_possibly_visible && actor->hasValidDescription()) {
      const ActorTypeDescription* actor_type = actor->getActorTypeDescription();
      actor->setRendering();
      std::pair<Actor*,size_t> actor_subset(actor, 0);
      rendering_table_.insert( // make sure the actor is available in the world
        TextureIndexToActorMeshTextureSubsetTable::value_type(
            actor_type->texture_id, actor_subset));
    }*/
    if (actor->isRendering()) {
      if (!is_possibly_visible) {
        // problem: scenery still shows up
        //eraseFromRenderingTable(actor);
      }
    } else {
     if (is_possibly_visible && actor->hasValidDescription()) {
        const ActorTypeDescription* actor_type = actor->getActorTypeDescription();
        actor->setRendering();
        std::pair<Actor*,size_t> actor_subset(actor, 0);
        rendering_table_.insert( // make sure the actor is available in the world
          TextureIndexToActorMeshTextureSubsetTable::value_type(
              actor_type->texture_id, actor_subset));
      }
    }
  }
}


bool GlobalActorManager::render(LPDIRECT3DDEVICE9 d3d_device,
                int texture_id,
                IsometricCamera* camera,
                GlobalRenderingManager* global_rendering_manager) {
  CONFIRM(d3d_device) else return false;

  // Get the sequence of meshes to render
  TextureIndexToActorMeshTextureSubsetTable::iterator end = rendering_table_.upper_bound(texture_id);
  TextureIndexToActorMeshTextureSubsetTable::iterator i = rendering_table_.lower_bound(texture_id);

  // Make sure this texture is in the device if we are going to render
  if (i == end)
    return true;

  // Initialize rendering
  d3d_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
  global_rendering_manager->changeCurrentTexture(texture_id);

  // Repeat for all of the mesh texture subsets using this texture
  for (; i != end;)
  {
    // Build a list of controllers using a single mesh type
    GameFileDataSkinnedMesh* skinnedMesh = i->second.first->getSkinnedMesh();
    size_t textureGroupIndex = i->second.second;
    std::vector<AnimatedMeshController*> controllers;
    while(i != end && i->second.first->getSkinnedMesh() == skinnedMesh) {
      //if (i->second.first->state == ACTORSTATE_SPIRITFORM) { ++i; continue; }
      controllers.push_back(i->second.first->getMeshController());
      ++i;
    }

    CONFIRM(skinnedMesh) else { ++i; continue; }

    // Draw the meshes
    GameFileDataSkinnedMesh::TextureGroup* textureGroup = &skinnedMesh->textureGroups[textureGroupIndex];
    for (size_t infl = 0; infl < textureGroup->numberOfBlendingGroups; ++infl)
    {
      // Set up for rendering this group
      GameFileDataSkinnedMesh::TextureGroup::BlendingGroup* blendingGroup = &textureGroup->blendingGroups[infl];
      d3d_device->SetRenderState(D3DRS_VERTEXBLEND, blendingGroup->blendFlags);
      d3d_device->SetFVF(blendingGroup->fvf);
      d3d_device->SetStreamSource(0, blendingGroup->vertexBuffer, 0, blendingGroup->vertexStride);
      d3d_device->SetIndices(blendingGroup->indexBuffer);

      // TODO: blending group 0 (no indexed blending) needs indexed blending set to FALSE.

      //if (myUsingMixedDeviceFlag)
      //  d3d_device->SetSoftwareVertexProcessing(blendingGroup->renderInSoftware ? TRUE : FALSE);

      // For each of the controllers, draw the mesh
      for (std::vector<AnimatedMeshController*>::iterator controllerIterator = controllers.begin();
        controllerIterator != controllers.end();
        ++controllerIterator) {

      // Get the controller being rendered
      AnimatedMeshController* controller = *controllerIterator;
      CONFIRM(controller) else continue;

      // Save the bone matrices
      controller->setBoneMatrices(d3d_device, camera);

      d3d_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
        blendingGroup->lowestReferencedVertexIndex,
        blendingGroup->numberOfVertices,
        blendingGroup->triangleStartIndex,
        blendingGroup->numberOfTriangles);
      }
    }
  }

  // Reset the device state
  d3d_device->SetRenderState(D3DRS_VERTEXBLEND, 0);
  d3d_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
  //if (myUsingMixedDeviceFlag)
  //  d3d_device->SetSoftwareVertexProcessing(FALSE);

  return true;
}

void GlobalActorManager::release(ActorID id) {
  ActorMap::iterator i = actor_map_.find(id);
  if (i == actor_map_.end()) return;
  Actor* actor = i->second;
  actors_without_descriptions_.erase(actor);
  eraseFromRenderingTable(actor);
  if (actor != client_actor_) {
    actor_map_.erase(i);
    actor->release(scenery_renderer_);
    delete actor;
  }
}

Actor* GlobalActorManager::get(ActorID id) {
  ActorMap::iterator i = actor_map_.find(id);
  return i == actor_map_.end() ? NULL : i->second;
}

bool GlobalActorManager::findActorNearLocation(float x,
                         float y,
                         ActorID actor_to_ignore,
                         ActorID* id,
                         bool find_living_only) {
  int closest_actor_id = 0;
  float closest_distance_sq = 1.0f; // this is the maximum search radius
  for (ActorMap::iterator i = actor_map_.begin(); i != actor_map_.end(); ++i)
  {
    if (i->second->getID() == actor_to_ignore) continue;
    float dx = i->second->getX() - x, dy = i->second->getY() - y;
    float dist_sq = dx*dx + dy*dy;
    if (dist_sq < closest_distance_sq)
    {
      if (!find_living_only || !i->second->isDead())
      {
        closest_distance_sq = dist_sq;
        closest_actor_id = i->first;
      }
    }
  }

  // Assign the ID
  *id = closest_actor_id;
  return closest_actor_id != 0;
}

bool GlobalActorManager::findActorNearLocation(float x, float y, ActorID actor_to_ignore, Actor** actor, bool find_living_only) {
  ActorID id;
  if (findActorNearLocation(x, y, actor_to_ignore, &id, find_living_only)) {
    *actor = get(id);
    return true;
  }

  return false;
}

bool GlobalActorManager::findLivingNonclientActorNearLocation(float x, float y, Actor** actor) {
  return findActorNearLocation(x, y, client_actor_->getID(), actor, true);
}

void GlobalActorManager::slowSync(float time_since_last_sync,
                                  double time,
                                  ::Evidyon::Network::Packet::ActorSyncData* slow_sync,
                                  std::set<ActorID>* actors_without_names) {
  Actor* actor;
  ActorID actor_id = slow_sync->id;
  bool created = acquire(actor_id,
                         &actor);

  CONFIRM(actor) else return;
  CONFIRM(actor != client_actor_) else return;

  handleActorSyncAction(time, actor, slow_sync->flags);

  // If we just made this actor, snap it to the correct location by faking a
  // "teleport" command.
  if (created) slow_sync->flags.teleport = 1;
  actor->sync(time_since_last_sync,
              time,
              slow_sync->x_packed * Network::FLOAT_UNPACK_MULTIPLIER,
              slow_sync->y_packed * Network::FLOAT_UNPACK_MULTIPLIER,
              slow_sync->flags);

  if (created) {
    const ClientActorDescriptionTemplate* description_template =
      actor->getDescriptionTemplate();
    setActorType(actor,  // add this actor to the renderer
                 description_template->actor_type);
    if (description_template->description_can_change) {
      addDescriptionlessActor(actor);
    }
    if ('\0' == description_template->name[0]) {
      actors_without_names->insert(actor_id);
    }
  }
}

void GlobalActorManager::fastSync(float time_since_last_sync,
                                  double time,
                                  float x,
                                  float y,
                                  Evidyon::Network::Packet::ActorSyncData::Flags flags) {
  if (client_actor_ == NULL) return;
  --fastsyncs_until_description_request_;

  handleActorSyncAction(time, client_actor_, flags);

  client_actor_->sync(time_since_last_sync,
                      time,
                      x,
                      y,
                      flags);
}

void GlobalActorManager::eraseFromRenderingTable(Actor* actor) {
  if (actor && actor->hasValidDescription()) {
    actor->setNotRendering();

    const ActorTypeDescription* old_type = actor->getActorTypeDescription();

    TextureIndexToActorMeshTextureSubsetTable::iterator lower =
      rendering_table_.lower_bound(old_type->texture_id);
    TextureIndexToActorMeshTextureSubsetTable::iterator upper =
      rendering_table_.upper_bound(old_type->texture_id);

    for (TextureIndexToActorMeshTextureSubsetTable::iterator i = lower; i != upper; ++i) {
      if (i->second.first == actor) {
        rendering_table_.erase(i);
        break;
      }
    }
  }
}



Actor* GlobalActorManager::acquireClientActor(ActorID id,
                                              float x,
                                              float y,
                                              const char* name) {
  Actor* actor = NULL;
  CONFIRM(acquire(id, &actor));
  client_actor_ = actor;
  actor->setPosition(x, y);
  actor->setName(name);
  return actor;
}


bool GlobalActorManager::acquire(ActorID id,
                                 Actor** actor) {
  bool create_actor = false;
  Actor* actor_internal = get(id);
  create_actor = !actor_internal;

  if (create_actor) {
    //DEBUG_INFO("creating actor %lu", id);

    const ClientActorDescriptionTemplate* description_template;
    { // find the template on which the actor is based
      unsigned int description_template_index;
      UnpackActorID(id, &description_template_index);
      CONFIRM(description_template_index < actor_description_templates_.size()) else {
        DEBUG_INFO("Actor ID %lu has invalid description index %lu", id, description_template_index);
        *actor = NULL;
        return false;
      }
      description_template = &actor_description_templates_.at(description_template_index);
    }

    // create the actor using this template type and save it
    actor_internal = new Actor(id, description_template);
    actor_map_.insert(ActorMap::value_type(id, actor_internal));

    // Create default attachment point scenery
    for (int i = 0; i < ACTORATTACHMENTPOINT_COUNT; ++i) {
      unsigned int scenery_index = description_template->equipped_item_scenery[i];
      HSCENERY scenery;
      scenery_renderer_->addScenery(NULL,
                                    scenery_index,
                                   &scenery);
      actor_internal->setAttachmentPointScenery((ActorAttachmentPoint)i, scenery);
    }
  }

  *actor = actor_internal;

  return create_actor;
}


void GlobalActorManager::setActorType(Actor* actor, int type) {
  //DEBUG_INFO("Setting actor %lu description", actor->getID());

  // Update this actor's complete description
  if (type >= 0 && type < actor_type_descriptions_.size()) {
    const ActorTypeDescription* actor_type =
      &actor_type_descriptions_.at(type);

    // Only set if it's different from the current one, since
    // this resets a lot of stuff.  In the future this won't
    // even be necessary because stuff like corporeal state
    // will be updated in a different message.
    if (actor->getActorTypeDescription() != actor_type) {
      eraseFromRenderingTable(actor);
      actor->setDescription(actor_type);
    }
  } else {
    eraseFromRenderingTable(actor);
  }
}

void GlobalActorManager::handleActorSyncAction(double time, Actor* actor, const ::Evidyon::Network::Packet::ActorSyncData::Flags sync_flags) {
  const ActorTypeDescription* description = actor->getActorTypeDescription();
  if (description != NULL) {
    ActorAction action;
    { // this is a hack to get the "death" sound to play
      if (actor->isDead() == false && sync_flags.mesh_state == 0) {
        action = ACTORACTION_DIE;
      } else {
        action = (ActorAction)sync_flags.action;
      }
    }

    float x = actor->getX(), y = actor->getY();
    playActionSound(x, y, actor == client_actor_, description, action);

    if (sync_flags.blood) { // create the universal physical hit effect
      special_fx_manager_->createMagicFX(time,
                                         physical_hit_magic_fx_,
                                         0,
                                         actor,
                                         x,
                                         y,
                                         NULL,
                                         0);
    }
  }

}


void GlobalActorManager::playActionSound(float x, float y, bool client_actor, const ActorTypeDescription* description, ActorAction action) {
  switch (action) {
    case ACTORACTION_ATTACK1:
    case ACTORACTION_ATTACK2:
    case ACTORACTION_ATTACK3:
    case ACTORACTION_ATTACK4:
      sound_manager_->playSound(client_actor ? SOUNDPRIORITY_SELF : SOUNDPRIORITY_NEARBY,
                                x, y,
                                description->sounds[ACTORSOUND_ATTACK1 + rand()%2]);
      break;
    case ACTORACTION_DIE:
      sound_manager_->playSound(client_actor ? SOUNDPRIORITY_SELF : SOUNDPRIORITY_NEARBY,
                                x, y,
                                description->sounds[ACTORSOUND_DIE1 + rand()%2]);
      break;
    case ACTORACTION_GOTHIT:
      sound_manager_->playSound(client_actor ? SOUNDPRIORITY_SELF : SOUNDPRIORITY_NEARBY,
                                x, y,
                                description->sounds[ACTORSOUND_GOTHIT1 + rand()%2]);
      break;
  }
}


void GlobalActorManager::addDescriptionlessActor(Actor* actor) {
  if (actors_without_descriptions_.empty()) {
    fastsyncs_until_description_request_ =
      FASTSYNCS_TO_WAIT_BEFORE_REQUESTING_DESCRIPTION;
  }
  actors_without_descriptions_.insert(actor);
}


bool GlobalActorManager::actorsNeedDescriptions() const {
  bool retval = !actors_without_descriptions_.empty() &&
     fastsyncs_until_description_request_ <= 0;
  if (retval) {
    DEBUG_INFO("%lu actors need descriptions", actors_without_descriptions_.size());
  }
  return retval;
}

NetworkPacket* GlobalActorManager::buildRequestActorDescriptionPacket() {
  using namespace ::Evidyon::Network::Packet;
  NetworkPacket* packet = Client::Encode::askActorFullDescriptions(&actors_without_descriptions_);
  fastsyncs_until_description_request_ = FASTSYNCS_TO_WAIT_BEFORE_REQUESTING_DESCRIPTION;
  //actors_without_descriptions_.clear();
  return packet;
}

void GlobalActorManager::printDebugStats(char* buffer, size_t buffer_size) const {
  float x, y;
  const char* name;
  if (client_actor_) {
    name = client_actor_->getName();
    x = client_actor_->getX();
    y = client_actor_->getY();
  } else {
    name = "(???)";
    x = 0.0f;
    y = 0.0f;
  }
  sprintf_s(buffer,
            buffer_size,
            "ActorMgr:  %lu templates, %lu types; %lu active actors (%lu render entries); "\
            "%lu missing full defn; client %s at %.02f, %.02f (%.02f)",
            actor_description_templates_.size(),
            actor_type_descriptions_.size(),
            actor_map_.size(),
            rendering_table_.size(),
            actors_without_descriptions_.size(),
            name,
            x,
            y);
}


}