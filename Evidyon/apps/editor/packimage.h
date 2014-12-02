//------------------------------------------------------------------------------------------------
// File:    packimage.h
//
// Desc:    An image that is part of a resource pack
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __PACKIMAGE_H__
#define __PACKIMAGE_H__



/**
 * A pack image is an image that is inside of a resource pack
 */
class PackImage : public dc::dcResource<PackImage,Image>
{
    public:

        /**
         * Initializes this resource
         */
        PackImage();


    public:

        /**
         * Loads the image denoted by this class using the specified device.  A device pointer
         * is made a parameter so that even when this class is not being edited, the method can
         * still be used to load the texture.
         *   @param pd3dDevice Device onto which to load the texture
         *   @param options Options used when creating the destination texture
         *   @param ppd3dTexture Return location for texture interface
         *   @return Result code
         */
        bool obtainD3DTexture(LPDIRECT3DDEVICE9 pd3dDevice, DWORD options, LPDIRECT3DTEXTURE9* ppd3dTexture) const;


    public:

        /**
         * Changes the resource index that this packed image uses to access its source data.  This
         * method is required for building resource packs, and really shouldn't be used anywhere
         * other than there.
         */
        void setResourceIndex(int index);

        /**
         * Get the resource pack reference for this image
         */
        dc::dcTable<ResourcePack>::Reference* getResourcePack() { return &myResourcePack; }


    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The resource pack to which the image is attached
        dc::dcTable<ResourcePack>::Reference myResourcePack;

        /// The index that this image occupies in the pack.  We alias this type
        /// so that it can't be edited like a normal integer.
        Alias<dcInteger> myResourceIndex;

        /// This class implements the Image interface
        static dc::dcClass<PackImage>::Implements<Image> implementsImage;
};



#endif