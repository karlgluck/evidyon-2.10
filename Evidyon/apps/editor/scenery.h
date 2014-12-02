//------------------------------------------------------------------------------------------------
// File:    scenery.h
//
// Desc:    Scenery applies a skin and transformations to specify how a static mesh will be
//          displayed in the world.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __SCENERY_H__
#define __SCENERY_H__

#include "stdafx.h"
#include "image.h"
#include "texture.h"
#include "skin.h"
#include "mesh.h"

/**
 * Generates the scenery
 */
class Scenery : public dc::dcResource<Scenery>
{
    public:

        /**
         * Contains raw information needed to display a preview of this resource
         */
        struct Preview
        {
            /// The resource that created this preview
            Scenery* scenery;

            /// The mesh that this scenery references.  0 is valid.
            Mesh::Preview* meshPreview;

            /// The skin that this scenery references.  0 is valid.
            Skin::Preview* skinPreview;

            /// The transformation to apply to the scenery
            D3DXMATRIXA16 transform;
        };


    public:

        /**
         * Initializes this scenery with a name
         */
        Scenery();

        /**
         * Initializes a preview structure for this scenery element
         * @param meshPreviewArray Array of mesh previews in the same order as the global mesh table
         * @param meshPreviews The number of entries in the mesh preview array
         * @param skinPreviewArray Array of skin previews in the same order as the global skin table
         * @param skinPreviews The number of entries in the skin preview array
         * @param preview Destination preview structure
         * @return Whether or not the preview was initialized correctly
         */
        bool loadPreview(Mesh::Preview* meshPreviewArray, size_t meshPreviews,
                          Skin::Preview* skinPreviewArray, size_t skinPreviews, Preview* preview);

        dc::dcTable<Mesh>::Reference* getMeshRef() const { return (dcTable<Mesh>::Reference*)&myMesh; }

        dc::dcList<Skin>::Reference* getSkinRef() const { return (dcList<Skin>::Reference*)&mySkin; }

        dcxVector3<dcFloat>* getRotation() const { return (dcxVector3<dcFloat>*)&myRotation; }
        dcxVector3<dcFloat>* getScaling() const { return (dcxVector3<dcFloat>*)&myScaling; }
        dcxVector3<dcFloat>* getTranslation() const { return (dcxVector3<dcFloat>*)&myTranslation; }

    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// The source mesh to use for this scenery
        dc::dcTable<Mesh>::Reference myMesh;

        /// The skin to apply to the mesh
        dc::dcList<Skin>::Reference mySkin;

        /// Transforms the mesh
        dcxVector3<dcFloat> myRotation;
        dcxVector3<dcFloat> myScaling;
        dcxVector3<dcFloat> myTranslation;


    protected:

        // This is the only implementation of its own type
        static dc::dcClass<Scenery>::Implements<Scenery> implementsSelf;
};



#endif