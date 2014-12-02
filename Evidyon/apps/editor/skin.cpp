//------------------------------------------------------------------------------------------------
// File:    skin.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "image.h"
#include "texture.h"
#include "skin.h"

dcClass<Skin>::Implements<Skin> Skin::implementsSelf;



//------------------------------------------------------------------------------------------------
// Name:  Skin
// Desc:  Initializes this resource with the given name
//------------------------------------------------------------------------------------------------
Skin::Skin()
{
    member("Map", &myMap);
}



//------------------------------------------------------------------------------------------------
// Name:  loadPreview
// Desc:  Loads the preview structure for this skin
//------------------------------------------------------------------------------------------------
bool Skin::loadPreview(Texture::Preview* texturePreviewArray, size_t texturePreviews,
                        Skin::Preview* preview)
{
    // Check the parameters
    if (APP_ERROR(!texturePreviewArray || !preview)("Invalid parameter to Skin::loadPreview"))
        return false;

    // Set basic values
    preview->skin = this;
    preview->map.clear();

    // Get the members of this skin's map
    const dc::dcGenericResource::Array& members = myMap.getMembers();

    // For each of the entries in this map, add an entry to the preview map
    dc::dcGenericResource::Array::const_iterator end = members.end();
    for (dcGenericResource::Array::const_iterator i = members.begin(); i != end; ++i)
    {
        // Cast to the map element type
        Map::Element* element = (Map::Element*)(*i);

        // Get the raw key value
        int key = element->getKey()->getValue();

        // Initialize the texture preview pointer
        Texture::Preview* texturePreview = 0;

        // Get the referenced texture
        dc::dcTable<Texture>::Element* textureElement = element->getValue()->getReferencedResource();
        if (!APP_WARNING(textureElement == 0)
            ("Skin %s entry %s has a null texture reference", getName().c_str(), element->getName().c_str()))
        {
            // Get the index of this texture in the table
            size_t textureIndex = textureElement->getIndex();

            // Obtain the texture preview from the array
            if (!APP_WARNING(textureIndex >= texturePreviews || texturePreviewArray[textureIndex].texture != textureElement->getImplementation())
                    ("Skin %s entry %s references texture with a mismatched index; ignoring", getName().c_str(), element->getName().c_str()))
            {
                // Save the associated texture preview
                texturePreview = &texturePreviewArray[textureIndex];
            }
        }

        // Add this entry to the preview map
        preview->map.insert(Preview::BasicMapType::value_type(key, texturePreview));
    }

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  getMap
// Desc:  Gets the map for this skin
//------------------------------------------------------------------------------------------------
Skin::Map* Skin::getMap() const
{
    return (Skin::Map*)&myMap;
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string Skin::staticTypeName()
{
    return "Skin";
}



