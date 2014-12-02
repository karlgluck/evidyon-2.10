//------------------------------------------------------------------------------------------------
// File:    mdjmesh.h
//
// Desc:    Loads a .MDJ-format mesh from a file
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __MDJMESH_H__
#define __MDJMESH_H__


/**
 * Implements the mesh type
 */
class MDJMesh : public dc::dcResource<MDJMesh,Mesh>
{
    public:

        /**
         * Initializes this class with the given name
         */
        MDJMesh();

        /**
         * Gets the file-name resource of this mesh
         */
        inline dc::dcFileName* getSourceFile() { return &mySourceFile; }


    public:

        /**
         * Loads geometry from the source file into the output subset geometry
         *   @param pd3dDevice A Direct3D device (if necessary)
         *   @param subsetGeometry The destination for the mesh's subset geometry
         *   @return Result code
         */
        bool obtainSourceGeometry(LPDIRECT3DDEVICE9 pd3dDevice, SubsetGeometry* subsetGeometry) const;


    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// The file from which to load the mesh
        dc::dcFileName mySourceFile;


    protected:

        /// This class implements the Mesh class
        static dc::dcClass<MDJMesh>::Implements<Mesh> implementsMesh;
};



#endif