//------------------------------------------------------------------------------------------------
// File:    skinnedmeshattachmentpoint.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "skinnedmeshattachmentpoint.h"


//------------------------------------------------------------------------------------------------
// Name: SkinnedMeshAttachmentPoint
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
SkinnedMeshAttachmentPoint::SkinnedMeshAttachmentPoint()
{
    member("Source Bone Index", &mySourceBoneIndex);
    member("Scaling", &myScaling);
    member("Translation from Bone", &myTranslation);
    member("Rotation from Bone", &myRotation);

    // Initialize the scaling factor
    myScaling.getX()->setValue(1.0f);
    myScaling.getY()->setValue(1.0f);
    myScaling.getZ()->setValue(1.0f);
}



//------------------------------------------------------------------------------------------------
// Name: getSourceBoneIndexValue
// Desc: Obtains the bone on which this attachment point is based
//------------------------------------------------------------------------------------------------
unsigned int SkinnedMeshAttachmentPoint::getSourceBoneIndexValue() const
{
    return mySourceBoneIndex.getValue();
}



//------------------------------------------------------------------------------------------------
// Name: generateOffsetMatrix
// Desc: Creates the offset matrix to which to attach items
//------------------------------------------------------------------------------------------------
void SkinnedMeshAttachmentPoint::generateOffsetMatrix(D3DXMATRIX* offsetMatrix) const
{
    D3DXMATRIX build;
    D3DXMatrixScaling(&build, myScaling.getX()->getValue(),
                               myScaling.getY()->getValue(),
                               myScaling.getZ()->getValue());
    D3DXMATRIX rotation;
    D3DXMatrixRotationYawPitchRoll(&rotation, D3DXToRadian(myRotation.getY()->getValue()),
                                               D3DXToRadian(myRotation.getX()->getValue()),
                                               D3DXToRadian(myRotation.getZ()->getValue()));
    D3DXMatrixMultiply(&build, &build, &rotation);
    D3DXMATRIX translation;
    D3DXMatrixTranslation(&translation, myTranslation.getX()->getValue(),
                                         myTranslation.getY()->getValue(),
                                         myTranslation.getZ()->getValue());
    D3DXMatrixMultiply(offsetMatrix, &build, &translation);
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Gets the string name of this resource type
//       @return Class name
//------------------------------------------------------------------------------------------------
std::string SkinnedMeshAttachmentPoint::staticTypeName()
{
    return "SkinnedMeshAttachmentPoint";
}

