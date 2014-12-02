//------------------------------------------------------------------------------------------------
// File:    animatedtexture.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "texture.h"
#include "animatedtexture.h"


/// Resolve this class's implementation linker
dcClass<AnimatedTexture>::Implements<Texture> implementsTexture;



//------------------------------------------------------------------------------------------------
// Name:  AnimatedTexture
// Desc:  Initializes this class
//------------------------------------------------------------------------------------------------
AnimatedTexture::AnimatedTexture()
{
    member("Horizontal Frame Divisions", &myFramesWide);
    member("Vertical Frame Divisions", &myFramesHigh);
    member("Speed", &myAnimationSpeed);
}



//------------------------------------------------------------------------------------------------
// Name:  configureTextureStageState
// Desc:  Sets up the texture stage indicated on the provided device for rendering
//------------------------------------------------------------------------------------------------
HRESULT AnimatedTexture::configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage)
{
    // Figure out the number of frames
    int framesWide = myFramesWide.getValue();
    int framesHigh = myFramesHigh.getValue();
    int totalFrames = framesWide * framesHigh;

    // Calculate the current frame
    int frame = ((int)floorf((GetTickCount() / 1000.0f) * myAnimationSpeed.getValue())) % totalFrames;

    // Generate the projection matrix to put this frame in the [0,0] [1,1] box.  This requires
    // a scaling and translation.
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);
    world._11 = 1.0f / framesWide;
    world._22 = 1.0f / framesHigh;
    world._31 = -1.0f / framesWide * (frame % framesWide);
    world._32 = -1.0f / framesHigh * ((int)(frame / framesWide));

    // Update the base state
    configureBaseState(pd3dDevice, stage, &world);

    // Unclamp texture addresses for this stuff
    pd3dDevice->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    pd3dDevice->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    pd3dDevice->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    // Set the matrix
    pd3dDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), &world);

    // Success
    return S_OK;
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string AnimatedTexture::staticTypeName()
{
    return "Animated Texture";
}
