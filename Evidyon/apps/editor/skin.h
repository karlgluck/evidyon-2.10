//------------------------------------------------------------------------------------------------
// File:    skin.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __SKIN_H__
#define __SKIN_H__


/**
 * A skin maps mesh subsets (for animated meshes or regular meshes) to a 
 */
class Skin : public dc::dcResource<Skin>
{
    public:

        /**
         * Contains raw information needed to display a preview of this resource
         */
        struct Preview
        {
            /// The kind of map that the skin uses
            typedef std::map<int,Texture::Preview*> BasicMapType;

            /// The resource that created this preview
            Skin* skin;

            /// The texture for the image
            BasicMapType map;
        };


    public:

        // The map type used by a skin
        typedef dc::dcMap<dcInteger,dcTable<Texture>::Reference> Map;

    public:

        /**
         * Initializes this resource
         */
        Skin();

        /**
         * Loads the preview structure for this skin.  If the texture can't be found for any of the
         * entries in this skin, a warning is logged but the method still succeeds (the entry is
         * NULL, so be careful!)
         * @param texturePreviewArray A straight array of textures that corresponds to the order of the
         *                            textures in the root texture table
         * @param texturePreviews How many entries are in the texture preview array
         * @param preview The destination preview structure.
         * @return Whether or not the load method succeeds
         */
        bool loadPreview(Texture::Preview* texturePreviewArray, size_t texturePreviews, Preview* preview);

        /**
         * Gets the map for this skin
         */
        Map* getMap() const;


    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// The internal map of subset indices to texture references
        Map myMap;

        // This is the only implementation of its own type
        static dc::dcClass<Skin>::Implements<Skin> implementsSelf;
};


#endif