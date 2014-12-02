//------------------------------------------------------------------------------------------------
// File:  d3dscene.cpp
//
// Desc:  The Project Volucris client state machine
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"



//
////------------------------------------------------------------------------------------------------
//// Name:  changeCurrentTexture
//// Desc:  
////------------------------------------------------------------------------------------------------
//void VolucrisClient::changeCurrentTexture(int textureID)
//{
//  changeCurrentTexture(&myResources.textures.textureArray[textureID]);
//}
//
////------------------------------------------------------------------------------------------------
//// Name:  changeCurrentTexture
//// Desc:  
////------------------------------------------------------------------------------------------------
//void VolucrisClient::resetCurrentTexture()
//{
//  myCurrentTexture = NULL;
//  changeCurrentD3DStage0Texture(0);
//  myD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
//  myD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
//  myD3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
//  myD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
//}
//
//
//
////------------------------------------------------------------------------------------------------
//// Name:  changeCurrentD3DStage0Texture
//// Desc:  
////------------------------------------------------------------------------------------------------
//void VolucrisClient::changeCurrentD3DStage0Texture(LPDIRECT3DTEXTURE9 texture)
//{
//  if (myCurrentD3DStage0Texture != texture)
//  {
//    SAFE_RELEASE(myCurrentD3DStage0Texture);
//    if (myD3DDevice)
//    {
//      myCurrentD3DStage0Texture = texture;
//      myD3DDevice->SetTexture(0, texture);
//      if (texture) texture->AddRef();
//    }
//  }
//}
//
////------------------------------------------------------------------------------------------------
//// Name:  changeCurrentTexture
//// Desc:  
////------------------------------------------------------------------------------------------------
//void VolucrisClient::changeCurrentTexture(GameFileDataTexture* texture)
//{
//  if (myCurrentTexture != texture)
//  {
//    // Set the texture, if it is valid; otherwise, clear the pipeline
//    if (texture)
//    {
//      // Change the texture
//      myCurrentTexture = texture;
//
//      // Set the Direct3D image
//      changeCurrentD3DStage0Texture(texture->image);
//
//      // Initialize the device rendering parameters
//      DWORD flags = texture->flags;
//      myD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, (flags & 2) != 0);
//      myD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (flags & 1) != 0);
//
//      // If we are alpha-blending, change the rendering state
//      if ((flags & 1) != 0)
//      {
//        myD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//        myD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//        myD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
//        myD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
//        myD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
//        myD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
//      }
//      else
//      {
//        myD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
//        myD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
//        myD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//        myD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//        myD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
//        myD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
//      }
//
//      // Calculate the texture transformation matrix
//      D3DXMATRIXA16 mat;
//      D3DXMatrixIdentity(&mat);
//      if (flags & 64)
//      {
//        // Animated texture
//        int framesWide = (flags&0xFF000000)>>24;
//        int framesHigh = (flags&0x00FF0000)>>16;
//        int totalFrames = framesWide * framesHigh;
//
//        // Calculate the current frame
//        int frame = ((int)floorf((GetTickCount() / 1000.0f) * texture->param1)) % totalFrames;
//
//        // Generate the projection matrix to put this frame in the [0,0] [1,1] box.  This requires
//        // a scaling and translation.
//        mat._11 = 1.0f / framesWide;
//        mat._22 = 1.0f / framesHigh;
//        mat._31 = -1.0f / framesWide * (frame % framesWide);
//        mat._32 = -1.0f / framesHigh * ((int)(frame / framesWide));
//      }
//      else if (flags & 32)
//      {
//        // Sliding texture
//        float amt = GetTickCount() / 1000.0f * texture->param1,
//            dx = amt * cosf(D3DXToRadian(texture->param2)),
//            dy = amt * sinf(D3DXToRadian(texture->param2));
//
//        // Build the translation matrix for moving the texture in a line
//        mat._31 = fmodf(dx, 1.0f);
//        mat._32 = fmodf(dy, 1.0f);
//      }
//      else if (flags & 16)
//      {
//        // Circling texture
//        DWORD time = dcxWin32Clock::getSystemTime();
//        mat._31 = texture->param2 * cosf(time / 1000.0f * D3DX_PI * 2 * texture->param1);
//        mat._32 = texture->param2 * sinf(time / 1000.0f * D3DX_PI * 2 * texture->param1);
//      }
//      if (flags & 8)   mat._11 *= -1.0f;
//      if (flags & 4)   mat._22 *= -1.0f;
//      myD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
//      myD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
//      myD3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
//      myD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + 0), &mat);
//    }
//    else
//    {
//      resetCurrentTexture();
//    }
//  }
//}