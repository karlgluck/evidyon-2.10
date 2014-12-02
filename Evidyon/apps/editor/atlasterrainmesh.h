//------------------------------------------------------------------------------------------------
// File:    atlasterrainmesh.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __ATLASTERRAINMESH_H__
#define __ATLASTERRAINMESH_H__

/**
 * Generates the terrain mesh that can be used with the following subset definitions
 */
bool CreateTerrainMesh(LPDIRECT3DDEVICE9 d3dDevice, LPD3DXMESH* terrainMesh);



enum TerrainMeshSubset
{
    // 4 sides of the wall, from Y = -1 to Y = 0
    TMS_WALL_SIDES,

    // Flat square at Y = 0 centered around X,Y = (0,0)
    TMS_FLAT_TEXN,
    TMS_FLAT_TEXE,
    TMS_FLAT_TEXS,
    TMS_FLAT_TEXW,

    // Square mesh, normal texture coordinates with a whole
    // triangle flat on Y=0 and a corner at Y=1
    TMS_HIGHCORNER_NW_TEXN,
    TMS_HIGHCORNER_NW_TEXE,
    TMS_HIGHCORNER_NW_TEXS,
    TMS_HIGHCORNER_NW_TEXW,
    TMS_HIGHCORNER_NE_TEXN,
    TMS_HIGHCORNER_NE_TEXE,
    TMS_HIGHCORNER_NE_TEXS,
    TMS_HIGHCORNER_NE_TEXW,
    TMS_HIGHCORNER_SE_TEXN,
    TMS_HIGHCORNER_SE_TEXE,
    TMS_HIGHCORNER_SE_TEXS,
    TMS_HIGHCORNER_SE_TEXW,
    TMS_HIGHCORNER_SW_TEXN,
    TMS_HIGHCORNER_SW_TEXE,
    TMS_HIGHCORNER_SW_TEXS,
    TMS_HIGHCORNER_SW_TEXW,

    // Square mesh, normal texture coordinates with a whole
    // triangle flat on Y=0 and a corner at Y=-1
    TMS_LOWCORNER_NW_TEXN,
    TMS_LOWCORNER_NW_TEXE,
    TMS_LOWCORNER_NW_TEXS,
    TMS_LOWCORNER_NW_TEXW,
    TMS_LOWCORNER_NE_TEXN,
    TMS_LOWCORNER_NE_TEXE,
    TMS_LOWCORNER_NE_TEXS,
    TMS_LOWCORNER_NE_TEXW,
    TMS_LOWCORNER_SE_TEXN,
    TMS_LOWCORNER_SE_TEXE,
    TMS_LOWCORNER_SE_TEXS,
    TMS_LOWCORNER_SE_TEXW,
    TMS_LOWCORNER_SW_TEXN,
    TMS_LOWCORNER_SW_TEXE,
    TMS_LOWCORNER_SW_TEXS,
    TMS_LOWCORNER_SW_TEXW,

    // Square mesh, normal texture coordinates with one side
    // on Y=0 and one side at Y=1
    TMS_HIGHSIDE_N_TEXN,
    TMS_HIGHSIDE_N_TEXE,
    TMS_HIGHSIDE_N_TEXS,
    TMS_HIGHSIDE_N_TEXW,
    TMS_HIGHSIDE_E_TEXN,
    TMS_HIGHSIDE_E_TEXE,
    TMS_HIGHSIDE_E_TEXS,
    TMS_HIGHSIDE_E_TEXW,
    TMS_HIGHSIDE_S_TEXN,
    TMS_HIGHSIDE_S_TEXE,
    TMS_HIGHSIDE_S_TEXS,
    TMS_HIGHSIDE_S_TEXW,
    TMS_HIGHSIDE_W_TEXN,
    TMS_HIGHSIDE_W_TEXE,
    TMS_HIGHSIDE_W_TEXS,
    TMS_HIGHSIDE_W_TEXW,

    // Square mesh, normal texture coordinates with a whole
    // triangle flat on Y=1 and a corner at Y=0
    TMS_RAISED_LOWCORNER_NW_TEXN,
    TMS_RAISED_LOWCORNER_NW_TEXE,
    TMS_RAISED_LOWCORNER_NW_TEXS,
    TMS_RAISED_LOWCORNER_NW_TEXW,
    TMS_RAISED_LOWCORNER_NE_TEXN,
    TMS_RAISED_LOWCORNER_NE_TEXE,
    TMS_RAISED_LOWCORNER_NE_TEXS,
    TMS_RAISED_LOWCORNER_NE_TEXW,
    TMS_RAISED_LOWCORNER_SE_TEXN,
    TMS_RAISED_LOWCORNER_SE_TEXE,
    TMS_RAISED_LOWCORNER_SE_TEXS,
    TMS_RAISED_LOWCORNER_SE_TEXW,
    TMS_RAISED_LOWCORNER_SW_TEXN,
    TMS_RAISED_LOWCORNER_SW_TEXE,
    TMS_RAISED_LOWCORNER_SW_TEXS,
    TMS_RAISED_LOWCORNER_SW_TEXW,

    // Square mesh, normal texture coordinates with one side
    // at Y=0 and a side at Y=-1
    TMS_LOWSIDE_N_TEXN,
    TMS_LOWSIDE_N_TEXE,
    TMS_LOWSIDE_N_TEXS,
    TMS_LOWSIDE_N_TEXW,
    TMS_LOWSIDE_E_TEXN,
    TMS_LOWSIDE_E_TEXE,
    TMS_LOWSIDE_E_TEXS,
    TMS_LOWSIDE_E_TEXW,
    TMS_LOWSIDE_S_TEXN,
    TMS_LOWSIDE_S_TEXE,
    TMS_LOWSIDE_S_TEXS,
    TMS_LOWSIDE_S_TEXW,
    TMS_LOWSIDE_W_TEXN,
    TMS_LOWSIDE_W_TEXE,
    TMS_LOWSIDE_W_TEXS,
    TMS_LOWSIDE_W_TEXW,

    // The number of terrain meshes
    TMS_COUNT,
    TMS_INVALID = 0xFFFFFFFF,
};

#define TMS_FLAT(tex)             ((TerrainMeshSubset)(TMS_FLAT_TEXN + (tex)))
#define TMS_HIGHCORNER(dir, tex)  ((TerrainMeshSubset)(TMS_HIGHCORNER_NW_TEXN + (dir) * 4 + (tex)))
#define TMS_LOWCORNER(dir, tex)   ((TerrainMeshSubset)(TMS_LOWCORNER_NW_TEXN + (dir) * 4 + (tex)))
#define TMS_HIGHSIDE(dir, tex)    ((TerrainMeshSubset)(TMS_HIGHSIDE_N_TEXN + (dir) * 4 + (tex)))
#define TMS_RAISED_LOWCORNER(dir, tex) ((TerrainMeshSubset)(TMS_RAISED_LOWCORNER_NW_TEXN + (dir) * 4 + (tex)))
#define TMS_LOWSIDE(dir, tex)     ((TerrainMeshSubset)(TMS_LOWSIDE_N_TEXN + (dir) * 4 + (tex)))


#endif