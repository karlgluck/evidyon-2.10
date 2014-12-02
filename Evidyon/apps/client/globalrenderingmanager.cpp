#include "globalrenderingmanager.h"
#include <dc/debug>

#include "../common/safemacros.h"

namespace Evidyon {

GlobalRenderingManager::GlobalRenderingManager() {
  d3d_device_ = NULL;
  current_stage0_texture_ = NULL;
  current_texture_ = NULL;
}

void GlobalRenderingManager::create(LPDIRECT3DDEVICE9 d3d_device) {
  destroy();
  if (d3d_device) (d3d_device_ = d3d_device)->AddRef();
}

void GlobalRenderingManager::destroy() {
  for (TextureDescriptions::iterator i = texture_descriptions_.begin();
     i != texture_descriptions_.end(); ++i) {
  SAFE_RELEASE(i->texture);
  }
  texture_descriptions_.clear();
  resetCurrentTexture();
  SAFE_RELEASE(current_stage0_texture_);
  current_texture_ = NULL;
  SAFE_RELEASE(d3d_device_);
}

bool GlobalRenderingManager::addTexture(size_t index, const TextureDescription& texture) {
  CONFIRM(texture_descriptions_.size() == index) else return false;
  texture_descriptions_.push_back(texture);
  return true;
}

void GlobalRenderingManager::changeCurrentTexture(int texture_id) {
  if (texture_id < texture_descriptions_.size()) {
  changeCurrentTexture(&texture_descriptions_.at(texture_id));
  }
}

void GlobalRenderingManager::resetCurrentTexture() {
  current_texture_ = NULL;
  if (d3d_device_) {
  changeCurrentD3DStage0Texture(NULL);
  d3d_device_->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  d3d_device_->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
  d3d_device_->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
  d3d_device_->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
  }
}

void GlobalRenderingManager::changeCurrentD3DStage0Texture(LPDIRECT3DTEXTURE9 texture) {
  current_texture_ = NULL;
  if (current_stage0_texture_ != texture) {
  SAFE_RELEASE(current_stage0_texture_);
  if (d3d_device_) {
    current_stage0_texture_ = texture;
    d3d_device_->SetTexture(0, texture);
    if (texture) texture->AddRef();
  }
  }
}

void GlobalRenderingManager::changeCurrentTexture(TextureDescription* texture) {
  if (current_texture_ == texture) return;
  LPDIRECT3DDEVICE9 d3d_device = d3d_device_;
  if (d3d_device== NULL) return;
  if (texture == NULL) { resetCurrentTexture(); return; }

  d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
  d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

  changeCurrentD3DStage0Texture(texture->texture);
  if (texture->alpha_test) {
    d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, texture->alpha_blend ? TRUE : FALSE);
    d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
    d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, texture->alpha_blend ? FALSE : TRUE);
    d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    //d3d_device->SetRenderState(D3DRS_ALPHAREF, 0x1F);
  } else {
    d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
    d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
    d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
  }

  D3DXMATRIXA16 mat;
  D3DXMatrixIdentity(&mat);
  double time = GetTickCount() / 1000.0;  // TODO: better timing :D
  switch (texture->type) {
    case TextureDescription::ANIMATED: {
      int frames_wide = texture->animated_flags.frames_wide;
      int frames_high = texture->animated_flags.frames_wide;
      int total_frames = frames_wide * frames_high;

      // Calculate the current frame
      int frame = ((int)floorf(time * texture->animated_flags.speed)) % total_frames;

      // Generate the projection matrix to put this frame in the [0,0] [1,1] box.  This requires
      // a scaling and translation.
      mat._11 =  1.0f / frames_wide;
      mat._22 =  1.0f / frames_high;
      mat._31 = -1.0f / frames_wide * (frame % frames_wide);
      mat._32 = -1.0f / frames_high * ((int)(frame / frames_wide));
    } break;

    case TextureDescription::CIRCLING: {
      mat._31 = texture->circling_flags.radius * cosf(time * texture->circling_flags.speed);
      mat._32 = texture->circling_flags.radius * sinf(time * texture->circling_flags.speed);
    } break;

    case TextureDescription::SLIDING: {
      // Build the translation matrix for moving the texture in a line
      const float SCALING = 10.0f;
      mat._31 = fmodf(texture->sliding_flags.dx * time / SCALING, 1.0f);
      mat._32 = fmodf(texture->sliding_flags.dy * time / SCALING, 1.0f);
    } break;

    case TextureDescription::STATIC: {
    } break;
  }
  if (texture->flip_horizontal) mat._11 *= -1.0f;
  if (texture->flip_vertical)   mat._22 *= -1.0f;
  d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
  d3d_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
  d3d_device->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + 0), &mat);
}





}