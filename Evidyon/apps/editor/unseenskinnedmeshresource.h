//------------------------------------------------------------------------------------------------
// File:    unseenskinnedmeshresource.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __UNSEENSKINNEDMESHRESOURCE_H__
#define __UNSEENSKINNEDMESHRESOURCE_H__




/**
 * This class references a .usm file containing a skinned mesh ready for animation
 */
class UnseenSkinnedMeshResource : public dc::dcResource<UnseenSkinnedMeshResource>
{
    public:

        /**
         * Initializes this resource
         */
        UnseenSkinnedMeshResource();


    public:

        /**
         * Loads the referenced mesh from the source file
         */
        bool loadSkinnedMesh(UnseenSkinnedMesh* skinnedMesh) const;

        /**
         * Gets the source file name for this skinned mesh
         * @return Pointer to the internal file name object
         */
        dc::dcFileName* getFileName();


    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The file from which to load the mesh
        dc::dcFileName mySourceFile;

        /// Whether or not to reverse the winding order of this mesh.  Setting
        /// this flag corrects meshes that are inside-out.
        dc::dcBoolean myReverseWindingOrderFlag;
};



#endif