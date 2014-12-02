//------------------------------------------------------------------------------------------------
// File:    statictexture.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "texture.h"
#include "statictexture.h"


/// Resolve this class's implementation linker
dcClass<StaticTexture>::Implements<Texture> implementsTexture;


//------------------------------------------------------------------------------------------------
// Name:  StaticTexture
// Desc:  Initializes this resource with the given name
//------------------------------------------------------------------------------------------------
StaticTexture::StaticTexture()
{
}



//------------------------------------------------------------------------------------------------
// Name:  configureTextureStageState
// Desc:  Sets up the texture stage indicated on the provided device for rendering
//------------------------------------------------------------------------------------------------
HRESULT StaticTexture::configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage)
{
    // Generate a matrix
    D3DXMATRIXA16 world;
    D3DXMatrixIdentity(&world);
    if (configureBaseState(pd3dDevice, stage, &world))
    {
        // Set the transform matrix
        pd3dDevice->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
        pd3dDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), &world);
    }
    else
    {
        // Disable any texture transforms
        pd3dDevice->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    }

    // Set up the rest of the state
    return S_OK;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string StaticTexture::staticTypeName()
{
    return "Static Texture";
}
