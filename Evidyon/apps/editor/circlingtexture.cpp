//------------------------------------------------------------------------------------------------
// File:    circlingtexture.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "texture.h"
#include "circlingtexture.h"


/// Resolve this class's implementation linker
dcClass<CirclingTexture>::Implements<Texture> implementsTexture;


//------------------------------------------------------------------------------------------------
// Name:  CirclingTexture
// Desc:  Initializes this resource with the given name
//------------------------------------------------------------------------------------------------
CirclingTexture::CirclingTexture()
{
    member("Speed", &myCirclingSpeed);
    member("Magnitude", &myCirclingMagnitude);

    // Set up default values
    myCirclingSpeed.setValue(0.5f);
    myCirclingMagnitude.setValue(0.5f);
}



//------------------------------------------------------------------------------------------------
// Name:  configureTextureStageState
// Desc:  Sets up the texture stage indicated on the provided device for rendering
//------------------------------------------------------------------------------------------------
HRESULT CirclingTexture::configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage)
{
    // Create a nontransforming matrix
    D3DXMATRIXA16 mat;
    D3DXMatrixIdentity(&mat);

    // Make the matrix circle
    mat._31 = myCirclingMagnitude.getValue() * cosf(GetTickCount() / 1000.0f * D3DX_PI * 2 * myCirclingSpeed.getValue());
    mat._32 = myCirclingMagnitude.getValue() * sinf(GetTickCount() / 1000.0f * D3DX_PI * 2 * myCirclingSpeed.getValue());

    // Update the base state
    configureBaseState(pd3dDevice, stage, &mat);

    // Unclamp texture addresses for this stuff
    pd3dDevice->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    pd3dDevice->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    pd3dDevice->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    // Set the matrix
    pd3dDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), &mat);

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string CirclingTexture::staticTypeName()
{
    return "Circling Texture";
}
