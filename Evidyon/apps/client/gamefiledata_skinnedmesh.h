#ifndef __GAMEFILEDATA_SKINNEDMESH_H__
#define __GAMEFILEDATA_SKINNEDMESH_H__



struct GameFileDataSkinnedMesh {
  DWORD numberOfTextureGroups;
  DWORD numberOfFrames;
  DWORD numberOfBones;

  struct Frame
  {
    D3DXMATRIX offset;
    DWORD parentFrameIndex;
  };

  Frame* frameHierarchy;

  struct Bone
  {
    D3DXMATRIX inverseOffset;
    DWORD frameIndex;
  };

  Bone* skeleton;

  struct TextureGroup
  {
    DWORD numberOfBlendingGroups;

    struct BlendingGroup
    {
      D3DVERTEXBLENDFLAGS blendFlags;

      DWORD fvf;

      DWORD vertexStride;

      LPDIRECT3DVERTEXBUFFER9 vertexBuffer;

      LPDIRECT3DINDEXBUFFER9 indexBuffer;

      DWORD triangleStartIndex;
      DWORD numberOfTriangles;
      DWORD lowestReferencedVertexIndex;
      DWORD numberOfVertices;

      bool renderInSoftware;
    };

    BlendingGroup* blendingGroups;
  };

  TextureGroup* textureGroups;
};

#endif