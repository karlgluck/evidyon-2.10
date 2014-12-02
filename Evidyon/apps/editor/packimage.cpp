//------------------------------------------------------------------------------------------------
// File:    packimage.cpp
//
// Desc:    An image that is part of a resource pack
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "allgameresources.h"
#include "resourcepack.h"
#include "packimage.h"


// Resolve the implementation linker
dcClass<PackImage>::Implements<Image> PackImage::implementsImage;


//------------------------------------------------------------------------------------------------
// Name:  PackImage
// Desc:  Initializes this resource
//------------------------------------------------------------------------------------------------
PackImage::PackImage()
{
    // Link members
    member("Resource Pack", &myResourcePack);
    member("Resource Index", &myResourceIndex);

    // Reset internal values
    myResourceIndex.setValue(-1);
}



//------------------------------------------------------------------------------------------------
// Name:  obtainD3DTexture
// Desc:  Loads the image denoted by this class using the specified device
//------------------------------------------------------------------------------------------------
bool PackImage::obtainD3DTexture(LPDIRECT3DDEVICE9 pd3dDevice, DWORD options, LPDIRECT3DTEXTURE9* ppd3dTexture) const
{
    // Fail without a device or if something is wrong with the output pointer
    if (!pd3dDevice || !ppd3dTexture) return false;

    // Get the pack from which to load this image
    dc::dcTable<ResourcePack>::Element* packElement = ((PackImage*)this)->myResourcePack.getReferencedResource();
    if (!packElement) return false;
    ResourcePack* pack = packElement->getImplementation();
    if (!pack) return false;

    // Obtain the image in the pack
    dcBuffer buffer;
    if (APP_ERROR(!pack->openResource(myResourceIndex.getValue(), &buffer))
        ("Failed to open image %i in pack file", myResourceIndex))
        return false;

    // Obtain options flags
    bool managed = (options & D3DTEX_MANAGED) != 0;
    bool mipmap = (options & D3DTEX_MIPMAP) != 0;
    bool systemmem = (options & D3DTEX_SYSTEMMEM) != 0;
    D3DPOOL pool = systemmem ? D3DPOOL_SYSTEMMEM : (managed ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT);

    // Holds return codes
    HRESULT hr;

    // Save a NULL first (just in case something goes wrong)
    *ppd3dTexture = NULL;

    // The destination for the texture interface
    LPDIRECT3DTEXTURE9 pd3dTexture;

    // Load the texture
    hr = D3DXCreateTextureFromFileInMemoryEx(pd3dDevice, buffer.getDataPointer(),
                   buffer.getDataSize(), D3DX_DEFAULT, D3DX_DEFAULT,
                   mipmap ? D3DX_DEFAULT : 1, 0, D3DFMT_UNKNOWN, pool,
                   D3DX_DEFAULT, D3DX_FILTER_POINT, 0, NULL, NULL, &pd3dTexture);

    // TODO:
    // buffer.destroy();

    // If there is a problem, there's not much more we can do
    if (APP_WARNING(FAILED(hr))("Couldn't load PackImage %i", myResourceIndex))
        return false;

    // Assign the output texture
    *ppd3dTexture = pd3dTexture;

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  setResourceIndex
// Desc:  Gets the string name of this resource type
//------------------------------------------------------------------------------------------------
void PackImage::setResourceIndex(int index)
{
    myResourceIndex.setValue(index);
}




//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the string name of this resource type
//------------------------------------------------------------------------------------------------
std::string PackImage::staticTypeName()
{
    return "Pack Image";
}
