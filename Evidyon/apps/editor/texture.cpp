//------------------------------------------------------------------------------------------------
// File:    texture.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "texture.h"




//------------------------------------------------------------------------------------------------
// Name:  Texture
// Desc:  Initializes this resource with the given name
//------------------------------------------------------------------------------------------------
Texture::Texture()
{
    member("Source Image", &myImageRef);
    member("Flip Horizontal", &myFlipHorizontal);
    member("Flip Vertical", &myFlipVertical);
    member("Enable Alpha Testing", &myEnableAlphaTestingFlag);
    member("Enable Alpha Blending", &myEnableAlphaBlendingFlag);
}


//------------------------------------------------------------------------------------------------
// Name:  loadPreview
// Desc:  Initializes a preview structure for textures
//------------------------------------------------------------------------------------------------
bool Texture::loadPreview(Image::Preview* imagePreviewArray, size_t imagePreviews,
                           Texture::Preview* preview)
{
    // Make sure the parameters are valid
    if (APP_ERROR(!imagePreviewArray || !preview)("Invalid parameter to Texture::loadPreview"))
        return false;

    // Save the basic information in the preview
    preview->texture = this;
    preview->enableAlphaTesting = myEnableAlphaTestingFlag.getValue();
    preview->enableAlphaBlending = myEnableAlphaBlendingFlag.getValue();

    // Reset the preview image
    preview->imagePreview = 0;

    // Get the source image
    dc::dcTable<Image>::Element* imageElement = myImageRef.getReferencedResource();
    if (APP_WARNING(imageElement == 0)("Texture %s has a null image reference", getName().c_str()))
        return true;

    // Get the index of this image in the table
    size_t imageIndex = imageElement->getIndex();

    // Obtain the image preview from the array
    if (APP_WARNING(imageIndex >= imagePreviews || imagePreviewArray[imageIndex].image != imageElement->getImplementation())
            ("Texture %s references image with a mismatched index; ignoring", getName().c_str()))
        return true; // Technically, this still succeeds

    // Save the associated image preview
    preview->imagePreview = &imagePreviewArray[imageIndex];

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  compareTo
// Desc:  Sorts textures into correct rendering order
//------------------------------------------------------------------------------------------------
int Texture::compareTo(const dc::dcGenericResource* other) const
{
    unsigned long typeID = getTypeID(), otherTypeID = other ? other->getTypeID() : 0;
    unsigned long ownerTypeID = getOwner() ? getOwner()->getTypeID() : 0,
             otherOwnerTypeID = other ? (other->getOwner() ? other->getOwner()->getTypeID() : 0) : 0;
    if (other == 0 || ((typeID != otherTypeID) && ((ownerTypeID != otherOwnerTypeID) || !getOwner()->isInterface())) )
        return dc::dcGenericResource::compareTo(other);
    else
    {
        Texture* otherTexture = (Texture*)other;

        // Sort by alpha testing flags so that the alpha-blended textures get rendered last
        if (!getEnableAlphaBlendingValue() &&  otherTexture->getEnableAlphaBlendingValue())
            return -1;
        if ( getEnableAlphaBlendingValue() && !otherTexture->getEnableAlphaBlendingValue())
            return +1;
        if (!getEnableAlphaTestingValue() &&  otherTexture->getEnableAlphaTestingValue())
            return -1;
        if ( getEnableAlphaTestingValue() && !otherTexture->getEnableAlphaTestingValue())
            return +1;

        // Sort by image references
        //return myImageRef.compareTo(otherTexture->getImageRef());
        const dc::dcTable<Image>::Element* image = myImageRef.getReferencedResource();
        const dc::dcTable<Image>::Element* otherImage = otherTexture->myImageRef.getReferencedResource();
        if (!image && otherImage) return -1;
        if (image && !otherImage) return +1;
        if (image == otherImage) return 0;
        size_t index = image->getIndex();
        size_t otherIndex = otherImage->getIndex();
        if (index < otherIndex) return -1;
        if (index > otherIndex) return +1;

        // rank by whatever's left
        return dc::dcGenericResource::compareTo(other);
    }
}



//------------------------------------------------------------------------------------------------
// Name:  configureBaseState
// Desc:  Sets up root-class texture information
//------------------------------------------------------------------------------------------------
bool Texture::configureBaseState(LPDIRECT3DDEVICE9 d3dDevice, DWORD stage, D3DMATRIX* textureTransform)
{
    // Enable texture effects
    d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, myEnableAlphaTestingFlag.getValue());
    d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, myEnableAlphaBlendingFlag.getValue());

    bool flipHorizontal = myFlipHorizontal.getValue(),
         flipVertical = myFlipVertical.getValue();

    // Update the texture transform, if necessary
    if (flipHorizontal) textureTransform->_11 *= -1.0f;
    if (flipVertical)   textureTransform->_22 *= -1.0f;

    // If we updated the transform, return true
    return flipHorizontal || flipVertical;
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string Texture::staticTypeName()
{
    return "Texture";
}
