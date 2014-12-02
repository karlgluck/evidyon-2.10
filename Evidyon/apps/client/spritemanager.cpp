//------------------------------------------------------------------------------------------------
// File:    spritemanager.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"





//------------------------------------------------------------------------------------------------
// Name:  SpriteManager
// Desc:  Resets the class
//------------------------------------------------------------------------------------------------
SpriteManager::SpriteManager()
{
    // Reset the sprite
    mySpriteRenderer = 0;

    // Reset each of the layer offset matrices
    for (int layer = 0; layer < (int)LAYER_COUNT; ++layer)
        D3DXMatrixIdentity(&myLayerOffsets[layer]);
}



//------------------------------------------------------------------------------------------------
// Name:  initialize
// Desc:  Sets up the sprite manager for use.  This must be called before
//------------------------------------------------------------------------------------------------
bool SpriteManager::initialize(LPDIRECT3DDEVICE9 d3dDevice)
{
    // Make sure the sprite renderer doesn't already exist
    if (APP_WARNING(mySpriteRenderer != 0)("SpriteManager - multiple initialization"))
        return false;

    // Create the rendering sprite
    if (APP_ERROR(FAILED(D3DXCreateSprite(d3dDevice, &mySpriteRenderer)))("Unable to create sprite renderer for GUI"))
        return false;

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  shutdown
// Desc:  Clears D3D objects used by this sprite manager.
//------------------------------------------------------------------------------------------------
void SpriteManager::shutdown()
{
    // Get rid of the rendering object
    SAFE_RELEASE(mySpriteRenderer);

    // Free all of the sprite layers
    for (int layer = 0; layer < (int)LAYER_COUNT; ++layer)
    {
        // Erase all of the textures in sprites in this layer
        clearLayer((Layer)layer);

        // Get rid of the memory
        myLayers[layer].freeMemory();
    }
}




//------------------------------------------------------------------------------------------------
// Name:  clear
// Desc:  Removes all of the sprites from this manager
//------------------------------------------------------------------------------------------------
void SpriteManager::clear()
{
    // Free all of the sprite layers
    for (int layer = 0; layer < (int)LAYER_COUNT; ++layer)
        clearLayer((Layer)layer);
}




//------------------------------------------------------------------------------------------------
// Name:  clearLayer
// Desc:  Removes all of the sprites from one of the layers of this manager
//------------------------------------------------------------------------------------------------
void SpriteManager::clearLayer(Layer layer)
{
    // Free the memory for all of the textures in this layer
    for (size_t spriteIndex = 0; spriteIndex < myLayers[layer].size(); ++spriteIndex)
    {
        // Get the sprite object
        Sprite* sprite = myLayers[layer].get(spriteIndex);

        // Release the texture
        SAFE_RELEASE(sprite->texture);
    }

    // Reset the number of entries in the layer
    myLayers[layer].clear();
}




//------------------------------------------------------------------------------------------------
// Name:  addSprite
// Desc:  
//------------------------------------------------------------------------------------------------
SpriteManager::Sprite* SpriteManager::addSprite(Layer layer)
{
    // Get a sprite from this layer
    Sprite* sprite = myLayers[layer].add();

    // If the sprite pointer is valid, fill the structure with junk data to make sure the user
    // resets it properly
#if defined(DEBUG) || defined(_DEBUG)
    if (sprite) memset(sprite, 0xFEEDF00D, sizeof(Sprite));
#endif

    // Return the result
    return sprite;
}




//------------------------------------------------------------------------------------------------
// Name:  render
// Desc:  Draws all of the sprites in this manager to the screen
//------------------------------------------------------------------------------------------------
bool SpriteManager::render()
{
    // Whether or not the sprite-rendering segment has been entered.  This little flag saves a
    // lot of state changing when there are no sprites!
    bool enteredSpriteRendering = false;

    // Render each of the layers in order
    for (int layer = 0; layer < (int)LAYER_COUNT; ++layer)
    {
        // Make sure that we've entered the rendering phase
        if (!enteredSpriteRendering && myLayers[layer].size() > 0)
        {
            // Set up the sprite for rendering
            HRESULT hr = mySpriteRenderer->Begin(D3DXSPRITE_ALPHABLEND|D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
            if (APP_WARNING(FAILED(hr))("Unable to initialize sprite manager for rendering"))
                return false;

            // Successfully initialized rendering
            enteredSpriteRendering = true;
        }

        // Render each of the sprites in this layer
        for (size_t spriteIndex = 0; spriteIndex < myLayers[layer].size(); ++spriteIndex)
        {
            // Get the sprite at this index
            Sprite* sprite = myLayers[layer].get(spriteIndex);

            // This pointer is to the transformation matrix that should be used to render the sprite
            D3DXMATRIX* transform = &sprite->transform;

            // If this layer has an offset, build the combined transform
            D3DXMATRIX combinedTransform;
            if (!D3DXMatrixIsIdentity(&myLayerOffsets[layer]))
            {
                // Multiply the matrices
                D3DXMatrixMultiply(&combinedTransform, &myLayerOffsets[layer], &sprite->transform);

                // Set the transformation matrix
                transform = &combinedTransform;
            }

            // Set the sprite's transformation matrix
            APP_WARNING(FAILED(mySpriteRenderer->SetTransform(transform)))("Couldn't set sprite's transform");

            // The source image region to use (NULL is everything)
            RECT* srcRect;

            // Whether or not we should use a source region
            if (sprite->srcRegion.right <= sprite->srcRegion.left ||
                sprite->srcRegion.bottom <= sprite->srcRegion.top)
                srcRect = 0;
            else
                srcRect = &sprite->srcRegion;

            // Render the sprite
            HRESULT hr = mySpriteRenderer->Draw(sprite->texture, srcRect, NULL, sprite->positionOffset, sprite->color);
            if (APP_WARNING(FAILED(hr))("Sprite rendering failed!"))
                break;
        }
    }

    // If we rendered things, the renderer should be closed
    if (enteredSpriteRendering)
        mySpriteRenderer->End();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  deleteGroup
// Desc:  Erases all of the sprites with the designated group in the specified layer
//------------------------------------------------------------------------------------------------
void SpriteManager::deleteGroup(Layer layer, int group)
{
    // If this layer is empty, don't do anything
    if (myLayers[layer].size() == 0) return;

    size_t end = myLayers[layer].size() - 1, start = 0;

    // Repeat until the end and start pointers are the same
    while(start < end)
    {
        // Remove entries from the end
        while(end > start && myLayers[layer].get(end)->group == group)
        {
            // Erase this element's texture reference
            Sprite* sprite = myLayers[layer].get(end);
            SAFE_RELEASE(sprite->texture);

            end--;
        }

        // Resize the array to get rid of the end elements
        if (start == end)
        {
            Sprite* sprite = myLayers[layer].get(end);
            SAFE_RELEASE(sprite->texture);
            myLayers[layer].truncate(end);
        }
        else
            myLayers[layer].truncate(end + 1);

        // Swap down elements from the end to cover those we are removing at the start
        while(start < end && myLayers[layer].get(end)->group != group)
        {
            if (myLayers[layer].get(start)->group == group)
            {
                // Erase the sprite's data
                Sprite* sprite = myLayers[layer].get(start);
                SAFE_RELEASE(sprite->texture);

                // Get rid of the element here by copying down the element at "end"
                myLayers[layer].remove(start);

                // Decrement the end pointer because the layer now has fewer elements
                end--;
            }

            // Increment the index
            start++;
        }
    }
}


//------------------------------------------------------------------------------------------------
// Name:  setGroupColor
// Desc:  Sets the color of all of the sprites that are in the designated group
//------------------------------------------------------------------------------------------------
void SpriteManager::setGroupColor(Layer layer, int group, D3DCOLOR color)
{
    for (size_t i = 0; i < myLayers[layer].size(); ++i)
    {
        // Get the sprite from this layer; if it's in the right group, change its color
        Sprite* sprite = myLayers[layer].get(i);
        if (sprite && sprite->group == group)
            sprite->color = color;
    }
}


//------------------------------------------------------------------------------------------------
// Name:  getFirstSpriteInGroup
// Desc:  Returns the first sprite in the given group, or null if it doesn't exist
//------------------------------------------------------------------------------------------------
SpriteManager::Sprite* SpriteManager::getFirstSpriteInGroup(Layer layer, int group)
{
    // Search for the first entry of the given group
    for (size_t i = 0; i < myLayers[layer].size(); ++i)
    {
        Sprite* sprite = myLayers[layer].get(i);
        if (sprite && sprite->group == group) return sprite;
    }

    // Nothing found
    return 0;
}





//------------------------------------------------------------------------------------------------
// Name:  transformLayer
// Desc:  Offsets all of the sprites in the given layer by the transformation
//------------------------------------------------------------------------------------------------
//void SpriteManager::transformLayer(Layer layer, const D3DXMATRIX* matrix)
//{
//}




//void fadeSprites(double secondsUntilStart, double entranceDuration, double secondsUntilEnd, double exitDuration)
//{
//}




