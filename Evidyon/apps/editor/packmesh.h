//------------------------------------------------------------------------------------------------
// File:    packmesh.h
//
// Desc:    An image that is part of a resource pack
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __PACKMESH_H__
#define __PACKMESH_H__

/**
 * A pack mesh is a mesh that is inside of a resource pack
 */
class PackMesh : public dc::dcResource<PackMesh,Mesh>
{
    public:

        /**
         * Initializes this resource
         */
        PackMesh();


    public:


    public:

        /**
         * Changes the resource index that this packed mesh uses to access its source data.  This
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
         * Loads the geometry held by this class into the destination set.  Note that the
         * returned set is indexed by subset; also, the data that is returned MUST be editable.
         * This means that by editing whatever comes out, the original data loaded by the
         * class must not change.
         *   @param pd3dDevice The source device (if necessary)
         *   @param subsetGeometry The destination for the mesh's subset geometry
         *   @return Result code
         */
        bool obtainSourceGeometry(LPDIRECT3DDEVICE9 pd3dDevice, SubsetGeometry* subsetGeometry) const;


    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The resource pack to which the image is attached
        dc::dcTable<ResourcePack>::Reference myResourcePack;

        /// The index that this mesh occupies in the pack.  We alias this type
        /// so that it can't be edited like a normal integer.
        Alias<dcInteger> myResourceIndex;

        /// This class implements the Mesh interface
        static dc::dcClass<PackMesh>::Implements<Mesh> implementsMesh;
};



#endif