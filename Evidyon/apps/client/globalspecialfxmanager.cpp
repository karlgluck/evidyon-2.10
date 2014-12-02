#include "globalspecialfxmanager.h"
#include "globalrenderingmanager.h"
#include "../common/safemacros.h"
#include "specialfxcontroller.h"
#include "../shared/geometry.h"
#include "../common/isometriccamera.h"
#include "../shared/compiledspecialfx.h"

#include "specialfxcontroller_orbitingparticle.h"
#include "specialfxcontroller_trailingparticle.h"
#include "specialfxcontroller_swirlingparticle.h"

#include "specialfxbinding_actor.h"
#include "specialfxbinding_location.h"
#include "specialfxbinding_projectile.h"
#include "../shared/client-editor/clientenchantmentdescription.h"
#include "actor.h"

#include <dc/debug>

#include "globalsoundmanager.h"


namespace Evidyon {

// How many particles are in a rendering batch
static const int RENDERING_PARTICLE_CAPACITY = 1000;
static const int RENDERING_GEOMETRY_VERTEX_CAPACITY = 1000;
static const int RENDERING_GEOMETRY_INDEX_CAPACITY = 3 * RENDERING_GEOMETRY_VERTEX_CAPACITY;

static const unsigned long RENDERING_PARTICLE_FVF = D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE;




GlobalSpecialFXManager::GlobalSpecialFXManager() {
  d3d_device_ = NULL;
  sound_manager_ = NULL;
  memset(&particle_vb_, 0, sizeof(particle_vb_));
  memset(&geometry_vb_, 0, sizeof(geometry_vb_));
  memset(&geometry_ib_, 0, sizeof(geometry_ib_));
  current_image_ = -1;
  active_image_ = -1;
}



bool GlobalSpecialFXManager::create(LPDIRECT3DDEVICE9 d3d_device,
                                    GlobalSoundManager* sound_manager) {
  destroy();

  sound_manager_ = sound_manager;
  (d3d_device_ = d3d_device)->AddRef();

  CONFIRM(createDynamicVB(sizeof(RenderingParticle) * RENDERING_PARTICLE_CAPACITY,
              RENDERING_PARTICLE_FVF,
              &particle_vb_)) else { destroy(); return false; }
  CONFIRM(createDynamicVB(sizeof(GeometryVertex) * RENDERING_GEOMETRY_VERTEX_CAPACITY,
              D3DFVF_GEOMETRYVERTEX,
              &geometry_vb_)) else { destroy(); return false; }
  CONFIRM(createDynamicIB(sizeof(GeometryVertex) * RENDERING_GEOMETRY_INDEX_CAPACITY,
              D3DFMT_GEOMETRYINDEX,
              &geometry_ib_)) else { destroy(); return false; }

  ASSERT(particle_data_memory_pool_.create()) else { destroy(); return false; }

  return true;
}



bool GlobalSpecialFXManager::addVisualFXImage(size_t index,
                        LPDIRECT3DTEXTURE9 image_texture) {
  ASSERT(images_.size() == index) else return false;
  if (image_texture) {
    image_texture->AddRef();
  }
  images_.push_back(image_texture);
  return true;
}



bool GlobalSpecialFXManager::addMagicFXType(
  size_t index,
  const MagicFXParams* magic_fx_params) {
  ASSERT(magic_fx_types_.size() == index) else return false;
  magic_fx_types_.push_back(*magic_fx_params);
  return true;
}


bool GlobalSpecialFXManager::addProjectileFXType(
  size_t index,
  const ProjectileFXParams* projectile_fx_params) {
  ASSERT(projectile_fx_types_.size() == index) else return false;
  projectile_fx_types_.push_back(*projectile_fx_params);
  return true;
}


bool GlobalSpecialFXManager::addVisualFXType(size_t index,
                       const VisualFXParams* visual_fx_params) {
  ASSERT(visual_fx_types_.size() == index) else return false;
  visual_fx_types_.push_back(*visual_fx_params);
  return true;
}


bool GlobalSpecialFXManager::addEnchantmentFXType(
    size_t index,
    const EnchantmentFXParams* enchantment_fx_params) {
  ASSERT(enchantment_fx_types_.size() == index) else return false;
  enchantment_fx_types_.push_back(*enchantment_fx_params);
  return true;
}


bool GlobalSpecialFXManager::addEnchantmentClass(
    size_t index,
    const EnchantmentClassParams* enchantment_class_params) {
  ASSERT(enchantment_class_types_.size() == index) else return false;
  enchantment_class_types_.push_back(*enchantment_class_params);
  return true;
}





void GlobalSpecialFXManager::destroy() {
  particle_data_memory_pool_.destroy();
  clear();

  // Release all of the particle images
  for (Images::iterator i = images_.begin(); i != images_.end(); ++i) {
  LPDIRECT3DTEXTURE9 texture = (*i);
  SAFE_RELEASE(texture);
  }
  images_.clear();

  SAFE_RELEASE(particle_vb_.d3d_vb);
  SAFE_RELEASE(geometry_vb_.d3d_vb);
  SAFE_RELEASE(geometry_ib_.d3d_ib);
  SAFE_RELEASE(d3d_device_);
  sound_manager_ = NULL;
}




void GlobalSpecialFXManager::createMagicFX(double time,
                                           int magic_fx_type,
                                           int power,
                                           Actor* source_actor,
                                           float source_x,
                                           float source_y,
                                           Actor** targets,
                                           int number_of_targets) {
  if (magic_fx_type < 0 || magic_fx_type >= magic_fx_types_.size()) return;
  MagicFXParams* magic_fx_params = &magic_fx_types_.at(magic_fx_type);

  { // play the sound
    float x, y;
    if (source_actor) {
      x = source_actor->getX();
      y = source_actor->getY();
    } else {
      x = source_x;
      y = source_y;
    }
    sound_manager_->playSound(SOUNDPRIORITY_NEARBY, x, y, magic_fx_params->sound_fx_index);
  }

  if (number_of_targets > 0) {
    for (int i = 0; i < MagicFXParams::MAX_VISUAL_FX; ++i) {
      const VisualFXParams* visual_fx_params = getVisualFXParams(magic_fx_params->visual_fx_indices[i]);
      if (visual_fx_params == NULL) continue;

      switch (visual_fx_params->type) {
        case VisualFXParams::ORBITING_PARTICLES_SOURCE:
        case VisualFXParams::TRAILING_PARTICLES_SOURCE: {
          createVisualFX(time, visual_fx_params, getActorBinding(source_actor));
        } break;

        case VisualFXParams::ORBITING_PARTICLES_TARGET:
        case VisualFXParams::TRAILING_PARTICLES_TARGET: {
          for (int i = 0; i < number_of_targets; ++i) {
            createVisualFX(time, visual_fx_params, getActorBinding(targets[i]));
          }
        } break;
      }
    }
  } else { // no targets specified
    for (int i = 0; i < MagicFXParams::MAX_VISUAL_FX; ++i) {
      const VisualFXParams* visual_fx_params = getVisualFXParams(magic_fx_params->visual_fx_indices[i]);
      if (visual_fx_params == NULL) continue;

      switch (visual_fx_params->type) {
        case VisualFXParams::ORBITING_PARTICLES_SOURCE:
        case VisualFXParams::TRAILING_PARTICLES_SOURCE: {
          createVisualFX(time, visual_fx_params, getActorBinding(source_actor));
        } break;

        case VisualFXParams::ORBITING_PARTICLES_TARGET:
        case VisualFXParams::TRAILING_PARTICLES_TARGET: {
          createVisualFX(time, visual_fx_params, getLocationBinding(source_x, source_y));
        } break;
      }
    }
  }
}



void GlobalSpecialFXManager::createProjectileFX(double time,
                        int projectile_id,
                        int projectile_fx_type,
                        float x,
                        float y,
                        float dx,
                        float dy) {
  if (projectile_fx_type < 0 ||
    projectile_fx_type >= projectile_fx_types_.size()) return;
  ProjectileFXParams* projectile_fx_params =
  &projectile_fx_types_.at(projectile_fx_type);

  sound_manager_->playSound(SOUNDPRIORITY_NEARBY, x, y, projectile_fx_params->carrier_sound);

  const VisualFXParams* visual_fx_params = getVisualFXParams(projectile_fx_params->carrier_fx);
  if (visual_fx_params == NULL) return;

  // use 90% of the duration time so that, if this projectile expires without
  // being destroyed due to hitting something, it doesn't perform
  // its termination effect mistakenly
  double expiration_time = projectile_fx_params->duration * 0.9f + time;

  SpecialFXController* controller = NULL;
  SpecialFXBinding* binding = createProjectileBinding(projectile_id,
                            expiration_time,
                            x,
                            y,
                            dx,
                            dy,
                            projectile_fx_params->termination_fx,
                            projectile_fx_params->termination_sound);
  createVisualFX(time, visual_fx_params, binding);

  // Add to the complete list of controllers
  if (controller != NULL) {
    all_controllers_.insert(controller);
    allocated_controllers_.insert(controller);
  }
}


void GlobalSpecialFXManager::destroyProjectileFX(double time, int projectile_id) {
  ProjectileBindings::iterator i = projectile_bindings_.find(projectile_id);
  if (i == projectile_bindings_.end()) return;
  SpecialFXBinding_Projectile* binding = (SpecialFXBinding_Projectile*)i->second;
  binding->setExpired();
  {
    if (binding->isTimedOut(time) == false) {
      float x, y;
      binding->get(&x, &y, NULL);

      // create the termination FX
      sound_manager_->playSound(SOUNDPRIORITY_NEARBY, x, y, binding->getTerminationSound());

      const VisualFXParams* visual_fx_params = getVisualFXParams(binding->getTerminationFX());
      if (visual_fx_params != NULL) {
        createVisualFX(time, visual_fx_params, getLocationBinding(x, y));
      }
    } else {
      binding->removeReference();
      projectile_bindings_.erase(i);
    }
  }
}


void GlobalSpecialFXManager::setActorEnchantments(double time,
                                                  unsigned int enchantment_mask,
                                                  Actor* actor,
                                                  bool enchantment_active[Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES]) {
  int class_index = 0;
  for (EnchantmentClassTypes::iterator i = enchantment_class_types_.begin();
       i != enchantment_class_types_.end(); ++i, ++class_index) {
    const ClientEnchantmentClassDescription* class_desc = &(*i);
    int first_bit = class_desc->mask.first_bit;
    int type_mask = class_desc->mask.type_mask;
    int enchantment = type_mask & (enchantment_mask >> first_bit);
    SpecialFXBinding* binding = enchantment == 0 ? NULL :
      createEnchantmentFX(time,
                          (enchantment - 1) + class_desc->first_enchantment_index,
                          actor);
    actor->setEnchantment(class_index, binding);

    if (enchantment_active) {
      enchantment_active[class_index] = binding != NULL;
    }
  }
}


SpecialFXBinding* GlobalSpecialFXManager::createEnchantmentFX(
    double time,
    int enchantment_fx_type,
    Actor* enchanted_actor) {
  if (enchantment_fx_type < 0 ||
      enchantment_fx_type >= enchantment_fx_types_.size()) return NULL;
  EnchantmentFXParams* enchantment_fx_params = &enchantment_fx_types_.at(enchantment_fx_type);
  const VisualFXParams* visual_fx_params = getVisualFXParams(enchantment_fx_params->visual_fx);
  if (visual_fx_params == NULL) return NULL;

  //sound_manager_->playSound(SOUNDPRIORITY_NEARBY, enchantment_fx_params->sound_fx);

  //SpecialFXBinding* enchantment_binding = getActorBinding(enchanted_actor);
  SpecialFXBinding* enchantment_binding = new SpecialFXBinding_Actor(enchanted_actor);

  // Length of a enchantment
  static const double LONG_TIME = 100000000.0;

  SpecialFXController* controller = NULL;
  switch (visual_fx_params->type) {
    case VisualFXParams::ORBITING_PARTICLES_SOURCE:
    case VisualFXParams::ORBITING_PARTICLES_TARGET: {
      const CompiledVisualFXParams_OrbitingParticles* params =
          &visual_fx_params->orbiting_particles;
      controller =
          new SpecialFXController_OrbitingParticle(time,
                                                   enchantment_binding,
                                                   LONG_TIME,
                                                   params);
      fx_controllers_.insert(
          Controllers::value_type(params->particle_sfx_image, controller));
    } break;

    case VisualFXParams::TRAILING_PARTICLES_SOURCE:
    case VisualFXParams::TRAILING_PARTICLES_TARGET: {
      const CompiledVisualFXParams_TrailingParticles* params =
          &visual_fx_params->trailing_particles;
      controller =
        new SpecialFXController_TrailingParticle(time,
                                                 enchantment_binding,
                                                 LONG_TIME,
                                                 params);
      fx_controllers_.insert(
          Controllers::value_type(params->particle_sfx_image, controller));
    } break;

    case VisualFXParams::SWIRLING_PARTICLES_SOURCE:
    case VisualFXParams::SWIRLING_PARTICLES_TARGET: {
      const CompiledVisualFXParams_SwirlingParticles* params =
          &visual_fx_params->swirling_particles;
      controller =
        new SpecialFXController_SwirlingParticle(time,
                                                 enchantment_binding,
                                                 LONG_TIME,
                                                 params);
      fx_controllers_.insert(
          Controllers::value_type(params->particle_sfx_image, controller));
    } break;

    default:
      DEBUG_INFO("Invalid controller type");
      break;
  }

  // Add to the complete list of controllers
  if (controller != NULL) {
    all_controllers_.insert(controller);
    allocated_controllers_.insert(controller);
    bindings_.insert(enchantment_binding);
  } else {
    enchantment_binding->setExpired();
    enchantment_binding = NULL;
  }

  // Return the binding
  return enchantment_binding;
}


void GlobalSpecialFXManager::update(double time,
                                    double time_since_last_update) {
  for (Bindings::iterator i = bindings_.begin(); i != bindings_.end();) {
    SpecialFXBinding* binding = *i;
    binding->update(time, time_since_last_update);
    if (binding->hasNoReferences()) {
      delete binding;
      Bindings::iterator next = i; ++next;
      bindings_.erase(i);
      i = next;
    } else {
       ++i;
    }
  }

  SpecialFXControllerSet controllers_to_erase;
  for (SpecialFXControllerSet::iterator i = all_controllers_.begin();
                     i != all_controllers_.end(); ++i) {
    SpecialFXController* controller = *i;
    if (controller->update(time, time_since_last_update, this)) {
      controllers_to_erase.insert(controller);
    }
  }

  for (SpecialFXControllerSet::iterator i = controllers_to_erase.begin();
                     i != controllers_to_erase.end(); ++i) {
    SpecialFXController* controller = *i;
    deleteController(controller);
  }
}


void GlobalSpecialFXManager::render(int texture_id,
                  IsometricCamera* camera,
                  GlobalRenderingManager* global_rendering_manager) {
  Controllers::iterator lower = geometry_controllers_.lower_bound(texture_id);
  Controllers::iterator upper = geometry_controllers_.lower_bound(texture_id);
  for (Controllers::iterator i = lower; i != upper; ++i) {
    SpecialFXController* controller = i->second;
    CONFIRM(controller->render(texture_id, camera, this));
  }

  // Make sure any geometry that was added is rendered
  renderGeometry();
}


void GlobalSpecialFXManager::renderFX(IsometricCamera* camera) {

  { // reset the world matrix
    D3DXMATRIXA16 mat;
    D3DXMatrixIdentity(&mat);
    camera->setWorldMatrix(d3d_device_, &mat);
  }

  { // set up the special FX device state
    d3d_device_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    d3d_device_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    d3d_device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
    d3d_device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    d3d_device_->SetRenderState(D3DRS_LIGHTING, FALSE);
    d3d_device_->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  #define FtoDW(f) (*(reinterpret_cast<DWORD*>(&(dummy_float=f))))
    float dummy_float;
    d3d_device_->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);  // turn on point sprites
    d3d_device_->SetRenderState(D3DRS_POINTSCALEENABLE,  TRUE);  // allow sprites to be scaled with distance
    d3d_device_->SetRenderState(D3DRS_POINTSIZE,   FtoDW(0.1f)); // float value that specifies the size to use for point size computation in cases where point size is not specified for each vertex.
    d3d_device_->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.0f)); // float value that specifies the minimum size of point primitives. Point primitives are clamped to this size during rendering. 
    d3d_device_->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(128.0f));
    d3d_device_->SetRenderState(D3DRS_POINTSCALE_A,  FtoDW(1.0f));  // Default 1.0
    d3d_device_->SetRenderState(D3DRS_POINTSCALE_B,  FtoDW(0.0f));  // Default 0.0
    d3d_device_->SetRenderState(D3DRS_POINTSCALE_C,  FtoDW(0.0f));  // Default 0.0
  #undef FtoDW

    d3d_device_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    d3d_device_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    d3d_device_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  }

  // Render all of the FX controllers
  current_image_ = -1; // reset the current image
  for (Controllers::iterator i = fx_controllers_.begin(); i != fx_controllers_.end(); ++i) {
    int active_image = i->first;
    active_image_ = active_image;
    SpecialFXController* controller = i->second;
    CONFIRM(controller->renderFX(active_image, camera, this));
  }
  renderParticles();
  renderGeometry();


  { // restore the standard device state
    d3d_device_->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    d3d_device_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    d3d_device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3d_device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    d3d_device_->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);  // turn on point sprites
    d3d_device_->SetRenderState(D3DRS_POINTSCALEENABLE,  FALSE);  // allow sprites to be scaled with distance
  }

}


void GlobalSpecialFXManager::eraseActorReferences(Actor* actor) {
  ActorBindings::iterator i = actor_bindings_.find(actor);
  if (i == actor_bindings_.end()) return;
    SpecialFXBinding* binding = i->second;
    binding->setExpired();
  { // erase reference
    binding->removeReference();
    actor_bindings_.erase(i);
  }
}

void GlobalSpecialFXManager::onChangeMap(Actor* client_actor) {

  // Get rid of all projectile bindings
  for (ProjectileBindings::iterator i = projectile_bindings_.begin();
        i != projectile_bindings_.end(); ++i) {
    SpecialFXBinding* binding = i->second;
    binding->setExpired();
    binding->removeReference();
    bindings_.erase(binding);
  }
  projectile_bindings_.clear();

  { // Remove actor bindings
    SpecialFXBinding* client_actor_binding = NULL;
    for (ActorBindings::iterator i = actor_bindings_.begin();
         i != actor_bindings_.end(); ++i) {
      SpecialFXBinding* binding = i->second;
      if (i->first == client_actor) {
        client_actor_binding = binding;
      } else {
        binding->setExpired();
        binding->removeReference();
        bindings_.erase(binding);
      }
    }
    actor_bindings_.clear();

    // Recover the client's binding, if it exists
    if (client_actor_binding != NULL) {
      actor_bindings_.insert(ActorBindings::value_type(
          client_actor, client_actor_binding));
    }
  }
}

void GlobalSpecialFXManager::clear() {
  fx_controllers_.clear();
  geometry_controllers_.clear();
  all_controllers_.clear();

  // Free all the controllers that we allocated
  for (SpecialFXControllerSet::iterator i = allocated_controllers_.begin();
                                        i != allocated_controllers_.end(); ++i) {
    SpecialFXController* controller = *i;
    delete controller;
  }
  allocated_controllers_.clear();

  // Free all bindings
  for (Bindings::iterator i = bindings_.begin(); i != bindings_.end(); ++i) {
    SpecialFXBinding* binding = (*i);
    delete binding;
  }
  bindings_.clear();
  projectile_bindings_.clear();
  actor_bindings_.clear();
}


void GlobalSpecialFXManager::registerFXController(int particle_type,
                          SpecialFXController* controller) {
  fx_controllers_.insert(Controllers::value_type(particle_type, controller));
}


void GlobalSpecialFXManager::unregisterFXController(SpecialFXController* controller) {
  for (Controllers::iterator i  = fx_controllers_.begin();
               i != fx_controllers_.end();) {
    if (i->second == controller) {
      Controllers::iterator next = i; ++next;
      fx_controllers_.erase(i);
      i = next;
    } else {
      ++i;
    }
  }
}


void GlobalSpecialFXManager::registerGeometryController(int texture_id,
                            SpecialFXController* controller) {
  geometry_controllers_.insert(Controllers::value_type(texture_id, controller));
}


void GlobalSpecialFXManager::unregisterGeometryController(SpecialFXController* controller) {
  for (Controllers::iterator i  = geometry_controllers_.begin();
               i != geometry_controllers_.end();) {
  if (i->second == controller) {
    Controllers::iterator next = i; ++next;
    geometry_controllers_.erase(i);
    i = next;
  } else {
    ++i;
  }
  }
}


bool GlobalSpecialFXManager::lockParticles(int number_of_particles_requested,
                       RenderingParticle** particles,
                       int* number_of_particles_obtained) {
  ASSERT(particle_vb_.d3d_vb != NULL) else return false;

  DWORD lock_flags = D3DLOCK_NOOVERWRITE;
  UINT lock_offset = 0;

  int elements_consumed_before = particle_vb_.elements_consumed;
  int elements_consumed_after = // check to see how many elements are needed
  elements_consumed_before + number_of_particles_requested;
  if (elements_consumed_after >= RENDERING_PARTICLE_CAPACITY) {
    CONFIRM(number_of_particles_requested < RENDERING_PARTICLE_CAPACITY) else return false;
    renderParticles(); // render the set of current particles

    particle_vb_.elements_consumed = number_of_particles_requested;
    particle_vb_.elements_rendered = 0;

    //lock_offset = 0;
    lock_flags = D3DLOCK_DISCARD;
  } else { // lock the next chunk of the buffer
    particle_vb_.elements_consumed = elements_consumed_after;
    lock_offset = elements_consumed_before * sizeof(RenderingParticle);
    //lock_flags = D3DLOCK_NOOVERWRITE;
  }

  UINT bytes_to_lock = sizeof(RenderingParticle) * number_of_particles_requested;
  *number_of_particles_obtained = number_of_particles_requested;
  return SUCCEEDED(particle_vb_.d3d_vb->Lock(lock_offset,     // offset from start of buffer (bytes)
                                             bytes_to_lock,   // size to lock (bytes)
                                             (void**)particles, // output buffer pointer
                                             lock_flags));    // dynamic locking flags
}

void GlobalSpecialFXManager::unlockParticles() {
  ASSERT(particle_vb_.d3d_vb != NULL) else return;
  particle_vb_.d3d_vb->Unlock();
  renderParticles();
}



bool GlobalSpecialFXManager::lockGeometry(int number_of_vertices,
                      int number_of_indices,
                      GeometryVertex** vertices,
                      GeometryIndex** indices) {
  return false;
}



void GlobalSpecialFXManager::unlockGeometry() {
  ASSERT(geometry_vb_.d3d_vb != NULL && geometry_ib_.d3d_ib != NULL) else return;
  geometry_vb_.d3d_vb->Unlock();
  geometry_ib_.d3d_ib->Unlock();
  renderGeometry();
}

OrbitingParticleData* GlobalSpecialFXManager::acquireOrbitingParticleData() {
  ParticleData* data = particle_data_memory_pool_.acquire();
  return data == NULL ? NULL : &data->orbiting_particle;
}

void GlobalSpecialFXManager::releaseOrbitingParticleData(OrbitingParticleData* particle) {
  particle_data_memory_pool_.release(reinterpret_cast<ParticleData*>(particle));
}

TrailingParticleData* GlobalSpecialFXManager::acquireTrailingParticleData() {
  ParticleData* data = particle_data_memory_pool_.acquire();
  return data == NULL ? NULL : &data->trailing_particle;
}

void GlobalSpecialFXManager::releaseTrailingParticleData(
    TrailingParticleData* particle) {
  particle_data_memory_pool_.release(reinterpret_cast<ParticleData*>(particle));
}

SwirlingParticleData* GlobalSpecialFXManager::acquireSwirlingParticleData() {
  ParticleData* data = particle_data_memory_pool_.acquire();
  return data == NULL ? NULL : &data->swirling_particle;
}

void GlobalSpecialFXManager::releaseSwirlingParticleData(SwirlingParticleData* particle) {
  particle_data_memory_pool_.release(reinterpret_cast<ParticleData*>(particle));
}


void GlobalSpecialFXManager::renderParticles() {
  int elements_rendered = particle_vb_.elements_rendered;
  int elements_consumed = particle_vb_.elements_consumed;
  if (elements_rendered >= elements_consumed) return;

  // Draw all elements that haven't been rendered
  initializeParticleRendering();
  HRESULT hr;
  hr = d3d_device_->DrawPrimitive(D3DPT_POINTLIST,
                  elements_rendered,
                  elements_consumed - elements_rendered);
  particle_vb_.elements_rendered = elements_consumed;
}



void GlobalSpecialFXManager::renderGeometry() {
}



bool GlobalSpecialFXManager::createDynamicVB(size_t buffer_size,
                       unsigned long fvf,
                       DynamicVB* vb) {
  ASSERT(d3d_device_) else return false;
  CONFIRM(SUCCEEDED(d3d_device_->CreateVertexBuffer(
       static_cast<UINT>(buffer_size),
       D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
       fvf,
       D3DPOOL_DEFAULT,
       &vb->d3d_vb, 0))) else return false;
  vb->elements_consumed = 0;

  return true;
}



bool GlobalSpecialFXManager::createDynamicIB(size_t buffer_size,
                       D3DFORMAT fmt,
                       DynamicIB* ib) {
  ASSERT(d3d_device_) else return false;
  CONFIRM(SUCCEEDED(d3d_device_->CreateIndexBuffer(
       static_cast<UINT>(buffer_size),
       D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
       fmt,
       D3DPOOL_DEFAULT,
       &ib->d3d_ib, 0))) else return false;
  ib->elements_consumed = 0;

  return true;
}



void GlobalSpecialFXManager::deleteController(SpecialFXController* controller) {
  unregisterFXController(controller);
  unregisterGeometryController(controller);
  all_controllers_.erase(controller);
  {
  SpecialFXControllerSet::iterator i = allocated_controllers_.find(controller);
  if (i == allocated_controllers_.end()) return;
  allocated_controllers_.erase(i);
  }
  controller->destroy(this); // free this controller's references
  delete controller;
}



void GlobalSpecialFXManager::initializeParticleRendering() {
  ASSERT(d3d_device_) else return;

  d3d_device_->SetFVF(RENDERING_PARTICLE_FVF);
  d3d_device_->SetStreamSource(0, particle_vb_.d3d_vb, 0, sizeof(RenderingParticle));

  if (current_image_ != active_image_) {
  setImage(active_image_);
  }
}



void GlobalSpecialFXManager::setImage(int image_index) {
  ASSERT(d3d_device_) else return;
  LPDIRECT3DTEXTURE9 texture = NULL;
  if (image_index >= 0 && image_index < images_.size())
  texture = images_.at(image_index);
  d3d_device_->SetTexture(0, texture);
  current_image_ = image_index;
}


SpecialFXBinding* GlobalSpecialFXManager::getLocationBinding(float x, float y) {
  SpecialFXBinding* binding = new SpecialFXBinding_Location(x, y);
  bindings_.insert(binding);
  return binding;
}


SpecialFXBinding* GlobalSpecialFXManager::getActorBinding(Actor* actor) {
  ActorBindings::iterator i = actor_bindings_.find(actor);
  if (i != actor_bindings_.end()) {
    return i->second;
  } else {
    SpecialFXBinding* binding = new SpecialFXBinding_Actor(actor);
    { // add a reference in the actors map
      binding->addReference();
      actor_bindings_.insert(ActorBindings::value_type(actor, binding));
    }
    bindings_.insert(binding);
    return binding;
  }
}


SpecialFXBinding* GlobalSpecialFXManager::createProjectileBinding(
  int projectile_id, double expiration_time,
  float x, float y, float dx, float dy, int termination_fx, int termination_sound) {
#ifdef EXTENDED_DEBUGGING
  {
  ProjectileBindings::iterator i = projectile_bindings_.find(projectile_id);
  ASSERT(i == projectile_bindings_.end()) else return i->second;
  }
#endif
  SpecialFXBinding* binding = new SpecialFXBinding_Projectile(expiration_time,
                                                              x,
                                                              y,
                                                              dx,
                                                              dy,
                                                              termination_fx,
                                                              termination_sound);
  { // add a reference in the projectile map
    projectile_bindings_.insert(ProjectileBindings::value_type(projectile_id, binding));
    binding->addReference();
  }
  bindings_.insert(binding);
  return binding;
}



const VisualFXParams* GlobalSpecialFXManager::getVisualFXParams(int index) {
  if (index < 0 ||
      index >= static_cast<int>(visual_fx_types_.size())) return NULL;
  return &visual_fx_types_.at(index);
}



SpecialFXController* GlobalSpecialFXManager::createVisualFX(double time,
                                                            const VisualFXParams* visual_fx_params,
                                                            SpecialFXBinding* binding) {
  SpecialFXController* controller = NULL;
  switch (visual_fx_params->type) {
    case VisualFXParams::ORBITING_PARTICLES_SOURCE:
    case VisualFXParams::ORBITING_PARTICLES_TARGET: {
      const CompiledVisualFXParams_OrbitingParticles* params =
        &visual_fx_params->orbiting_particles;
      controller = new SpecialFXController_OrbitingParticle(time,
                                                            binding,
                                                            params->duration,
                                                            params);
      fx_controllers_.insert(
        Controllers::value_type(visual_fx_params->orbiting_particles.particle_sfx_image,
                                controller));

      if (controller != NULL) {
        all_controllers_.insert(controller);
        allocated_controllers_.insert(controller);
      }
    } break;


    case VisualFXParams::TRAILING_PARTICLES_SOURCE:
    case VisualFXParams::TRAILING_PARTICLES_TARGET: {
      const CompiledVisualFXParams_TrailingParticles* params =
        &visual_fx_params->trailing_particles;
      controller =
        new SpecialFXController_TrailingParticle(time,
                                                 binding,
                                                 params->duration,
                                                 params);
      fx_controllers_.insert(
        Controllers::value_type(params->particle_sfx_image, controller));

      if (controller != NULL) {
        all_controllers_.insert(controller);
        allocated_controllers_.insert(controller);
      }
    } break;

    case VisualFXParams::SWIRLING_PARTICLES_SOURCE:
    case VisualFXParams::SWIRLING_PARTICLES_TARGET: {
      const CompiledVisualFXParams_SwirlingParticles* params =
          &visual_fx_params->swirling_particles;
      controller =
        new SpecialFXController_SwirlingParticle(time,
                                                 binding,
                                                 params->duration,
                                                 params);
      fx_controllers_.insert(
          Controllers::value_type(params->particle_sfx_image, controller));
    } break;

    default:
      DEBUG_INFO("Invalid controller");
      break;
  }

  return controller;
}


}