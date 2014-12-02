//------------------------------------------------------------------------------------------------
// File:    skinnedmeshattachmentpoint.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __SKINNEDMESHATTACHMENTPOINT_H__
#define __SKINNEDMESHATTACHMENTPOINT_H__




class SkinnedMeshAttachmentPoint : public dc::dcResource<SkinnedMeshAttachmentPoint>
{
    public:

        /**
         * Initializes this class
         */
        SkinnedMeshAttachmentPoint();

        /**
         * Obtains the bone on which this attachment point is based
         */
        unsigned int getSourceBoneIndexValue() const;

        /**
         * Creates the offset matrix to which to attach items
         */
        void generateOffsetMatrix(D3DXMATRIX* offsetMatrix) const;


    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The bone on which this attachment point is based
        dc::dcByte mySourceBoneIndex;

        /// Generates a transformation matrix that modifies the bone's position
        /// into one that is appropriate for attaching items
        dcxVector3<dcFloat> myScaling;
        dcxVector3<dcFloat> myRotation;
        dcxVector3<dcFloat> myTranslation;
};


#endif