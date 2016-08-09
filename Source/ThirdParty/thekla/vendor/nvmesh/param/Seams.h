// Copyright NVIDIA Corporation 2006 -- Ignacio Castano <icastano@nvidia.com>

#ifndef NV_MESH_SEAMS_H
#define NV_MESH_SEAMS_H

#include <nvmath/Vector.h>
#include <nvmesh/nvmesh.h>


namespace nv
{
	namespace HalfEdge { class Mesh; }
	class FloatImage;

	bool isValidTileAtlas(const HalfEdge::Mesh * mesh, uint w, uint h);
	bool isSeamless(const HalfEdge::Mesh * mesh, const FloatImage * img, uint channel=0);
	
	// Move seams texcoords to texel centers.
	void adjustSeamTexCoords(HalfEdge::Mesh * mesh, int width, int height, float offset=0.0f);

	bool fixTileAtlasSeams(const HalfEdge::Mesh * mesh, FloatImage * img, float offset=0.0f);
	bool fixCornerSeams(const HalfEdge::Mesh * mesh, FloatImage * img, float offset=0.0f);

} // nv namespace

#endif // NV_MESH_SEAMS_H
