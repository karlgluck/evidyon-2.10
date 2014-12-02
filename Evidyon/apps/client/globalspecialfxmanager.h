#ifndef __GLOBALSPECIALFXMANAGER_H__
#define __GLOBALSPECIALFXMANAGER_H__

#include <d3d9.h>
#include <dcx/memorypool>
#include <map>
#include <set>
#include <vector>



struct GeometryVertex;
typedef WORD GeometryIndex;
class IsometricCamera;

// Necessary until the Compiled and client-loaded versions differ
#include "../shared/compiledspecialfx.h"
#include "../shared/client-editor/clientprojectiledescription.h"
#include "../shared/evidyon_enchantments.h"


namespace Evidyon {

class Actor;
class GlobalRenderingManager;
class SpecialFXController;
class GlobalSoundManager;


struct CompiledMagicFXParams;
typedef struct CompiledMagicFXParams MagicFXParams;
struct CompiledVisualFXParams;
typedef struct CompiledVisualFXParams VisualFXParams;
struct ClientProjectileDescription;
typedef struct ClientProjectileDescription ProjectileFXParams;
struct ClientEnchantmentDescription;
typedef struct ClientEnchantmentDescription EnchantmentFXParams;
struct ClientEnchantmentClassDescription;
typedef struct ClientEnchantmentClassDescription EnchantmentClassParams;

struct RenderingParticle {
  float x, y, z;
  float point_size;
  D3DCOLOR color;
};

struct OrbitingParticleData {
  RenderingParticle rendering_particle;
  double dying_time;
  float angle_from_source;
  float distance_from_source;
};

struct TrailingParticleData {
  RenderingParticle rendering_particle;
  double dying_time;
  float vx, vy, vz;
};

struct SwirlingParticleData {
  RenderingParticle rendering_particle;
  //double start_moving_time; // offset for trailing particles
  float height;
  float angle_from_source;
  float speed;
};

union ParticleData {
  OrbitingParticleData orbiting_particle;
  TrailingParticleData trailing_particle;
  SwirlingParticleData swirling_particle;
};


class SpecialFXBinding;




class GlobalSpecialFXManager {
  static const int MAX_PARTICLES = 5000;
  typedef dcx::dcxMemoryPool<ParticleData, MAX_PARTICLES> ParticleDataMemoryPool;
  typedef std::set<SpecialFXController*> SpecialFXControllerSet;
  typedef std::vector<LPDIRECT3DTEXTURE9> Images;
  typedef std::multimap<int,SpecialFXController*> Controllers;
  typedef std::vector<MagicFXParams> MagicFXTypes;
  typedef std::vector<VisualFXParams> VisualFXTypes;
  typedef std::vector<ProjectileFXParams> ProjectileFXTypes;
  typedef std::vector<EnchantmentFXParams> EnchantmentFXTypes;
  typedef std::vector<EnchantmentClassParams> EnchantmentClassTypes;


  typedef std::map<Actor*, SpecialFXBinding*> ActorBindings;
  typedef std::map<int, SpecialFXBinding*> ProjectileBindings;
  typedef std::set<SpecialFXBinding*> Bindings; // used to free memory



  struct DynamicVB {
    LPDIRECT3DVERTEXBUFFER9 d3d_vb;
    int elements_consumed;
    int elements_rendered;
  };

  struct DynamicIB {
    LPDIRECT3DINDEXBUFFER9 d3d_ib;
    int elements_consumed;
    int elements_rendered;
  };

public:
  GlobalSpecialFXManager();
  bool create(LPDIRECT3DDEVICE9 d3d_device, GlobalSoundManager* sound_manager);
  bool addVisualFXImage(size_t index, LPDIRECT3DTEXTURE9 image_texture);
  bool addMagicFXType(size_t index, const MagicFXParams* magic_fx_params);
  bool addProjectileFXType(size_t index, const ProjectileFXParams* projectile_fx_params);
  bool addVisualFXType(size_t index, const VisualFXParams* visual_fx_params);
  bool addEnchantmentFXType(size_t index, const EnchantmentFXParams* enchantment_fx_params);
  bool addEnchantmentClass(size_t index, const EnchantmentClassParams* enchantment_class_params); 


  void destroy();

public:
  void createMagicFX(double time, int magic_fx_type, int power, Actor* source_actor, float source_x, float source_y, Actor** targets, int number_of_targets);
  void createProjectileFX(double time, int projectile_id, int projectile_fx_type, float x, float y, float dx, float dy);
  void destroyProjectileFX(double time, int projectile_id);

  // Creates enchantments for the actor and (optionally) sets corresponding values
  // in the enchantment_active array to 'true' or 'false' to indicate whether
  // that enchantment class has an element in it.
  void setActorEnchantments(double time,
                            unsigned int enchantment_mask,
                            Actor* actor,
                            bool enchantment_active[Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES]);

  // This creates an enchantment effect object whose binding is managed by the
  // enchanted actor itself.  This is different from internal types, as the
  // binding will not be automatically destroyed by eraseActorReferences.  It
  // is the responsibility of the owning class to expire the binding and
  // release its reference.
  // Enchantment effects do not expire until the the binding expires.
  SpecialFXBinding* createEnchantmentFX(double time,
                                        int enchantment_fx_type,
                                        Actor* enchanted_actor);

  void update(double time, double time_since_last_update);

  // Draws effects that reference common textures and use full 3D geometry.
  void render(int texture_id, IsometricCamera* camera,
              GlobalRenderingManager* global_rendering_manager);

  // Draws effects that don't reference common textures.  The global
  // rendering manager should have its current texture reset before
  // calling this method.
  void renderFX(IsometricCamera* camera);

  // Removes all internal references to the given actor structure.
  void eraseActorReferences(Actor* actor);

  // Resets this structure, leaving type definitions and buffer allocations
  // intact.
  void clear();

  // Gets rid of internal references to projectiles, area fx, etc.
  void onChangeMap(Actor* client_actor);

  // Controls controller membership in this manager.  Invoked automatically
  // by the create* methods, but available to external methods.
  void registerFXController(int particle_type, SpecialFXController* controller);
  void unregisterFXController(SpecialFXController* controller);
  void registerGeometryController(int texture_id, SpecialFXController* controller);
  void unregisterGeometryController(SpecialFXController* controller);

  // Used by SpecialFXController structures to get space in the rendering
  // buffer.  This method should only be invoked within the scope
  // of renderParticles.
  bool lockParticles(int number_of_particles_requested,
                     RenderingParticle** particles,
                     int* number_of_particles_obtained);
  void unlockParticles();

  bool lockGeometry(int number_of_vertices, int number_of_indices,
                    GeometryVertex** vertices, GeometryIndex** indices);
  void unlockGeometry();


  OrbitingParticleData* acquireOrbitingParticleData();
  void releaseOrbitingParticleData(OrbitingParticleData* particle);

  TrailingParticleData* acquireTrailingParticleData();
  void releaseTrailingParticleData(TrailingParticleData* particle);

  SwirlingParticleData* acquireSwirlingParticleData();
  void releaseSwirlingParticleData(SwirlingParticleData* particle);


private:
  // Draw the current set of particles from the vertex buffer to the screen.
  // If the current texture is not set, this method sets it and makes sure
  // that the rendering parameters have been initialized.
  void renderParticles();
  void renderGeometry();

// Internal helper methods
private:
  bool createDynamicVB(size_t buffer_size, unsigned long fvf, DynamicVB* vb);
  bool createDynamicIB(size_t buffer_size, D3DFORMAT fmt, DynamicIB* ib);
  void deleteController(SpecialFXController* controller);
  void initializeParticleRendering();
  void setImage(int image_index);
  SpecialFXBinding* getLocationBinding(float x, float y);
  SpecialFXBinding* getActorBinding(Actor* actor);
  SpecialFXBinding* createProjectileBinding(int projectile_id,
    double expiration_time, float x, float y, float dx, float dy,
    int termination_fx, int termination_sound);
  const VisualFXParams* getVisualFXParams(int index);
  SpecialFXController* createVisualFX(double time, const VisualFXParams* visual_fx_params, SpecialFXBinding* binding);

private:

  // All of the effects stored in this manager
  SpecialFXControllerSet all_controllers_, allocated_controllers_;

  LPDIRECT3DDEVICE9 d3d_device_;

  // These are both dynamic vertex buffers.  The geometry VB is used to render
  // geometry subsets on a per-texture basis.  The particle VB draws textures
  // during the internal renderFX call.
  DynamicVB particle_vb_, geometry_vb_;
  DynamicIB geometry_ib_;

  int current_image_;  // image settings initialized in D3D
  int active_image_;   // image that should be used for rendering

  // Holds the images used by the FX rendering.
  Images images_;

  // Keeps track of different controllers
  Controllers fx_controllers_, geometry_controllers_;

  // Effect types
  MagicFXTypes magic_fx_types_;
  VisualFXTypes visual_fx_types_;
  ProjectileFXTypes projectile_fx_types_;
  EnchantmentFXTypes enchantment_fx_types_;

  // Configuration tymes
  EnchantmentClassTypes enchantment_class_types_;

  // Effect binding types
  ProjectileBindings projectile_bindings_;
  ActorBindings actor_bindings_;
  Bindings bindings_; // entries without references are deallocated and removed

  // Holds memory for particles that the different controllers can use
  ParticleDataMemoryPool particle_data_memory_pool_;

  GlobalSoundManager* sound_manager_;
};

}


#endif