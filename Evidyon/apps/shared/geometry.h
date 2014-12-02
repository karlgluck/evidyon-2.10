//------------------------------------------------------------------------------------------------
// File:  geometry.h
//
// Desc:  Declares all of the structures and functions necessary to handle generic static
//      three-dimensional geometry
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <map>

/// A SubsetIndex specifies what kind of data is used to reference a subset in the skin
typedef DWORD SubsetIndex;

/// An invalid subset index
#define SUBSET_INVALID 0xFFFFFFFF

/**
 * A geometry vertex stores information that is used to render any kind of static geometry in
 * a 3d scene.
 *   @author Karl Gluck
 */
struct GeometryVertex
{
  /// Vertex location
  FLOAT x, y, z;

  /// Normal
  FLOAT nx, ny, nz;

  /// Texture coordinates
  FLOAT u, v;
};

/// The Direct3D vertex format of a geometry stream
#define D3DFVF_GEOMETRYVERTEX   (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)


/// Use 16-bit indices for geometry.  Geometry indices always specify lists of triangles, so
/// if given a number of indices, the number of polygons is that number divided by three.
typedef WORD GeometryIndex;
#define D3DFMT_GEOMETRYINDEX  (D3DFMT_INDEX16)

/**
 * Holds a set of geometry.  This is usually organized by some render-based limiting factor,
 * such as which texture the geometry uses.
 */
struct Geometry
{
  /// List of vertices for this geometry
  GeometryVertex* pVertices;

  /// How many vertices this geometry contains
  DWORD NumVertices;

  /// List of indices for this geometry
  GeometryIndex* pIndices;

  /// How many indices this geometry contains
  DWORD NumIndices;
};

/// Maps a mesh's subset index to a set of geometry (only 1 geometry entry per subset)
typedef std::multimap<SubsetIndex,Geometry*> SubsetGeometryCollection;

/// Maps a mesh's subset index to a set of geometry (only 1 geometry entry per subset)
typedef std::map<SubsetIndex,Geometry*> SubsetGeometry;

/**
 * Creates a new geometry structure with the requested buffer sizes
 *   @param Vertices How many vertices are required
 *   @param Indices How many indices are required
 *   @param ppGeometry Output geometry pointer, or NULL
 *   @return Whether or not the allocation succeeded
 */
HRESULT AllocateGeometry(DWORD Vertices, DWORD Indices, Geometry** ppGeometry);

/**
 * Calculates the total number of vertices/indices in a set of geometry
 */
VOID GetGeometryTotals(SubsetGeometry* geometry, DWORD* pTotalVertices, DWORD* pTotalIndices);

/**
 * Erases a geometry pointer
 *   @param ppGeometry Reference to the pointer to erase
 */
VOID DeallocateGeometry(Geometry* pGeometry);

/**
 * Erases all of the geometry in a group
 *   @param pGroup The group of geometry to clear
 */
VOID DeallocateGeometry(SubsetGeometryCollection* pGroup);

/**
 * Erases all of the geometry in a group
 *   @param pGroup The group of geometry to clear
 */
VOID DeallocateGeometry(SubsetGeometry* pGroup);

/**
 * Compresses a collection of geometry with multiple sets of geometry that have the
 * same subset indices into a list of geometry in which there is only one geometry
 * buffer per subset index.
 *   @param pSourceGeometry The geometry from which to read
 *   @param pDestGeometry Destination for collected geometry
 *   @return Whether or not the collection was successful
 */
HRESULT CollectGeometry(const SubsetGeometryCollection* pSourceGeometry,
             SubsetGeometry* pDestGeometry);



#endif