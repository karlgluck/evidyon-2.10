//------------------------------------------------------------------------------------------------
// File:    slidingtexture.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "texture.h"
#include "slidingtexture.h"


/// Resolve this class's implementation linker
dcClass<SlidingTexture>::Implements<Texture> implementsTexture;


//------------------------------------------------------------------------------------------------
// Name:  SlidingTexture
// Desc:  Initializes this resource with the given name
//------------------------------------------------------------------------------------------------
SlidingTexture::SlidingTexture()
{
    member("Speed", &myMovementSpeed);
    member("Angle", &myMovementAngle);

    // Initialize the movement speed
    myMovementSpeed.setValue(1.0f);
}



//------------------------------------------------------------------------------------------------
// Name:  configureTextureStageState
// Desc:  Sets up the texture stage indicated on the provided device for rendering
//------------------------------------------------------------------------------------------------
HRESULT SlidingTexture::configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage)
{
    // Create a nontransforming matrix
    D3DXMATRIXA16 mat;
    D3DXMatrixIdentity(&mat);

    // Build the transformation parameters
    float amt = GetTickCount() / 1000.0f * myMovementSpeed.getValue(),
          dx = amt * cosf(D3DXToRadian(myMovementAngle.getValue())),
          dy = amt * sinf(D3DXToRadian(myMovementAngle.getValue()));

    // Build the translation matrix for moving the texture in a line
    mat._31 = fmodf(dx, 1.0f);
    mat._32 = fmodf(dy, 1.0f);

    // Update the base state
    configureBaseState(pd3dDevice, stage, &mat);

    // Unclamp texture addresses
    pd3dDevice->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    pd3dDevice->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    pd3dDevice->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    // Set the matrix
    pd3dDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), &mat);

    // Success
    return S_OK;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string SlidingTexture::staticTypeName()
{
    return "Sliding Texture";
}
