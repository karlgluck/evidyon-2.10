//------------------------------------------------------------------------------------------------
// File:    terrain.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "texture.h"
#include "terrain.h"



dcClass<Terrain>::Implements<Terrain> Terrain::implementsSelf;


//------------------------------------------------------------------------------------------------
// Name:  getString
// Desc:  Implements the enum-index-to-string conversion for dc::dcEnum<MapNavigability>
//------------------------------------------------------------------------------------------------
const char* dc::dcEnum<TerrainDirection>::getString(TerrainDirection value)
{
    switch(value)
    {
        case TERRAINDIRECTION_NORTH: return "North";
        case TERRAINDIRECTION_EAST:  return "East";
        case TERRAINDIRECTION_SOUTH: return "South";
        case TERRAINDIRECTION_WEST:  return "West";
        default: return 0;
    }
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Implements the class name for dc::dcEnum<MapNavigability>
//------------------------------------------------------------------------------------------------
std::string dc::dcEnum<TerrainDirection>::staticTypeName()
{
    return "Terrain Direction";
}


//------------------------------------------------------------------------------------------------
// Name:  Terrain
// Desc:  Initializes this class
//------------------------------------------------------------------------------------------------
Terrain::Terrain()
{
    member("Texture - Top", &myTopTextureRef);
    member("Texture - Sides", &mySideTextureRef);
    member("Is Wall?", &myWallFlag);
    member("Direction", &myDirection);

    // Initialize data
    myDirection.setValue(TERRAINDIRECTION_NORTH);
}



//------------------------------------------------------------------------------------------------
// Name:  loadPreview
// Desc:  Initializes a preview structure for this terrain type
//------------------------------------------------------------------------------------------------
bool Terrain::loadPreview(Texture::Preview* texturePreviewArray, size_t texturePreviews, Preview* preview)
{
    // Check parameters
    if (APP_ERROR(!texturePreviewArray || !preview)("Invalid parameter to Terrain::loadPreview"))
        return false;

    // Initialize the structure
    preview->terrain = this;
    preview->isWall = myWallFlag.getValue();
    preview->direction = myDirection.getValue();
    preview->topTexturePreview = 0;
    preview->sideTexturePreview = 0;

    // Get the referenced top texture
    dc::dcTable<Texture>::Element* topTextureElement = myTopTextureRef.getReferencedResource();
    if (!APP_WARNING(topTextureElement == 0)
        ("Terrain %s has a null top texture reference", getName().c_str()))
    {
        // Get the index of this texture in the table
        size_t textureIndex = topTextureElement->getIndex();

        // Obtain the texture preview from the array
        if (!APP_WARNING(textureIndex >= texturePreviews || texturePreviewArray[textureIndex].texture != topTextureElement->getImplementation())
                ("Terrain %s references top texture with a mismatched index; ignoring", getName().c_str()))
        {
            // Save the associated texture preview
            preview->topTexturePreview = &texturePreviewArray[textureIndex];
        }
    }

    // Get the referenced side texture
    dc::dcTable<Texture>::Element* sideTextureElement = mySideTextureRef.getReferencedResource();
    if (sideTextureElement != 0)
    {
        // Get the index of this texture in the table
        size_t textureIndex = sideTextureElement->getIndex();

        // Obtain the texture preview from the array
        if (!APP_WARNING(textureIndex >= texturePreviews || texturePreviewArray[textureIndex].texture != sideTextureElement->getImplementation())
                ("Terrain %s references side texture with a mismatched index; ignoring", getName().c_str()))
        {
            // Save the associated texture preview
            preview->sideTexturePreview = &texturePreviewArray[textureIndex];
        }
    }

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this resource types
//------------------------------------------------------------------------------------------------
std::string Terrain::staticTypeName()
{
    return "Terrain";
}
