#ifndef __GLOBALRENDERINGMANAGER_H__
#define __GLOBALRENDERINGMANAGER_H__

#include <d3dx9.h>
#include <vector>

namespace Evidyon {

struct TextureDescription {
  LPDIRECT3DTEXTURE9 texture;
  bool alpha_blend, alpha_test;
  bool flip_horizontal, flip_vertical;

  enum Type {
  ANIMATED,
  SLIDING,
  CIRCLING,
  STATIC,
  } type;

  union {
  struct {
    int frames_wide, frames_high;
    float speed;
  } animated_flags;
  struct {
    float dx, dy;
  } sliding_flags;
  struct {
    float speed;
    float radius;
  } circling_flags;
  };
};


class GlobalRenderingManager {
  typedef std::vector<TextureDescription> TextureDescriptions;

public:
  GlobalRenderingManager();
  void create(LPDIRECT3DDEVICE9 d3d_device);
  void destroy();

  // The 'index' parameter is just used to make sure that the texture being
  // added is in the right location.  This method should be called linearly
  // in ascending index order to fill all of the texture resources.
  bool addTexture(size_t index, const TextureDescription& texture);

  void changeCurrentTexture(int texture_id);
  void resetCurrentTexture();

  void changeCurrentD3DStage0Texture(LPDIRECT3DTEXTURE9 texture);

  inline size_t numberOfTextures() const { return texture_descriptions_.size(); }

protected:
  void changeCurrentTexture(TextureDescription* texture);

protected:
  LPDIRECT3DDEVICE9 d3d_device_;
  LPDIRECT3DTEXTURE9 current_stage0_texture_;
  TextureDescription* current_texture_;

  TextureDescriptions texture_descriptions_;
};

}


#endif