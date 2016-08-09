// Copyright NVIDIA Corporation 2006 -- Ignacio Castano <icastano@nvidia.com>

#include "Seams.h"

#include <nvimage/FloatImage.h>
#include <nvmesh/halfedge/HalfEdgeMesh.h>
#include <nvmesh/halfedge/HalfEdgeVertex.h>

using namespace nv;

namespace
{
	// @@ Clean this up!
	
#if NV_OS_WIN32 && !NV_OS_MINGW
	// C-99 defines this function.
	static float roundf(float f)
	{
		// @@ This is not quite the same, though.
		return floorf(f + 0.5f);
	}
#endif
	
	static int iround(float f)
	{
		return int(roundf(f));
	}
	static int ifloor(float f)
	{
		return int(floorf(f));
	}
	
	static Vector2 round(Vector2::Arg v)
	{
		return Vector2(roundf(v.x()), roundf(v.y()));
	}

	static Vector2 floor(Vector2::Arg v)
	{
		return Vector2(floorf(v.x()), floorf(v.y()));
	}

	static Vector2 ceil(Vector2::Arg v)
	{
		return Vector2(ceilf(v.x()), ceilf(v.y()));
	}

	static bool isUnitRange(float f)
	{
		return f > 0.0f && f < 1.0f;
	}
	
} // namespace



bool nv::isValidTileAtlas(const HalfEdge::Mesh * mesh, uint w, uint h)
{
	bool texCoordInUnitRange = true;
	bool texCoordAtPixelCenter = true;
	bool texCoordSeamsOrthogonal = true;
	bool texCoordSeamsSameLength = true;
	
	const Vector2 dim((float)w, (float)h);
	
	nvDebug("--- Checking if this is valid tile atlas...\n");
	
	const uint vertexCount = mesh->vertexCount();
	for(uint v = 0; v < vertexCount; v++) {
		const HalfEdge::Vertex * vertex = mesh->vertexAt(v);
		
		Vector2 tex = vertex->tex();
		
		// Test that texcoords are in [0,1].
		if (!isUnitRange(tex.x()) || !isUnitRange(tex.y())) {
			texCoordInUnitRange = false;
		}
		
		const uint colocalCount = vertex->colocalCount();
		if (colocalCount > 1) {
			
			tex *= dim;
			
			// Test that seam texcoords are at pixel centers.
			if (!equal(tex.x(), roundf(tex.x())) || !equal(tex.y(), roundf(tex.y()))) {
				
				texCoordAtPixelCenter = false;
			}
		}		
	}
	
	nvDebug("---    Texcoords %s in [0, 1].\n", texCoordInUnitRange ? "are" : "are not");
	nvDebug("---    Seam texcoords %s at pixel centers.\n", texCoordAtPixelCenter ? "are" : "are not");
	
	
	// Process edge seams.
	const uint edgeCount = mesh->edgeCount();
	for(uint e = 0; e < edgeCount; e++) {
		const HalfEdge::Edge * edge = mesh->edgeAt(e);
		
		// Skip non-seams and already processed edges.
		if (edge->isBoundary() || !edge->isSeam() || edge->id() > edge->pair()->id()) {
			continue;
		}
		
		const HalfEdge::Vertex * va0 = edge->from();
		const HalfEdge::Vertex * va1 = edge->to();
		const HalfEdge::Vertex * vb0 = edge->pair()->to();
		const HalfEdge::Vertex * vb1 = edge->pair()->from();
		
		// @@ floor?
		const Vector2 ta0 = round(scale(va0->tex(), dim));
		const Vector2 ta1 = round(scale(va1->tex(), dim));
		const Vector2 tb0 = round(scale(vb0->tex(), dim));
		const Vector2 tb1 = round(scale(vb1->tex(), dim));
		
		if (ta0 == tb0 && ta1 == tb1) {
			// Not a real seam!
			continue;
		}
		
		// Make sure it's seamless.
		if ((ta0.x() != ta1.x() && ta0.y() != ta1.y()) ||
			(tb0.x() != tb1.x() && tb0.y() != tb1.y()))
		{
			texCoordSeamsOrthogonal = false;
		}
		
		if (iround(length(ta1 - ta0)) != iround(length(tb1 - tb0))) {
			texCoordSeamsSameLength = false;
		}
	}
	
	nvDebug("---    Seams %s orthogonal.\n", texCoordSeamsOrthogonal ? "are" : "are not");
	nvDebug("---    Seams %s the same length.\n", texCoordSeamsSameLength ? "have" : "don't have");
	
	const bool isValid = texCoordInUnitRange && texCoordAtPixelCenter && texCoordSeamsOrthogonal && texCoordSeamsSameLength;
	
	nvDebug("---    This is %s valid tile atlas.\n", isValid ? "a" : "*not* a");
	
	return isValid;
}


bool nv::isSeamless(const HalfEdge::Mesh * mesh, const FloatImage * img, uint channel/*=0*/)
{
	bool isSeamless = true;
	uint errorCount = 0;

	nvDebug("--- Testing if mesh has seam artifacts with the given texture.\n");
	
	const uint vertexCount = mesh->vertexCount();
	for(uint v = 0; v < vertexCount; v++) {
		const HalfEdge::Vertex * vertex = mesh->vertexAt(v);
		nvDebugCheck(vertex != NULL);
		
		// Use volatile to force stack storage and make sure that comparison is exact.
		const Vector2 tex = vertex->tex();
		volatile float displacement = img->sampleLinear(tex.x(), tex.y(), channel, FloatImage::WrapMode_Clamp);
		
		const uint colocalCount = vertex->colocalCount();
		if (colocalCount != 1) {
			
			for(HalfEdge::Vertex::ConstVertexIterator it(vertex->colocals()); !it.isDone(); it.advance()) {
				const HalfEdge::Vertex * colocal = it.current();
				
				const Vector2 colocalTex = colocal->tex();
				volatile float colocalDisplacement = img->sampleLinear(colocalTex.x(), colocalTex.y(), channel, FloatImage::WrapMode_Clamp);
				
				if (colocalDisplacement != displacement) {
					isSeamless = false;
					errorCount++;
				}
			}
		}
	}

	if (!isSeamless) {
		nvDebug("---   Mesh parameterization is *not* seamless. (%d errors)\n", errorCount);
	}
	else {
		nvDebug("---   Mesh parameterization is seamless.\n");
	}
	
	return false;
}


// Texel centers in OpenGL are at (0.5, 0.5)
static Vector2 roundToTexelCenter(Vector2::Arg v, int w, int h, float offset)
{
	float x = (floor(v.x() * w + offset) + 0.5f) / w;
	float y = (floor(v.y() * h + offset) + 0.5f) / h;
	return Vector2(x, y);
}

void nv::adjustSeamTexCoords(HalfEdge::Mesh * mesh, int width, int height, float offset)
{
	// Quantize seam texture coordinates.
	const uint vertexCount = mesh->vertexCount();
	for(uint v = 0; v < vertexCount; v++)
	{
		HalfEdge::Vertex * vertex =  mesh->vertexAt(v);
	
		const uint colocalCount = vertex->colocalCount();
		if (colocalCount > 1)
		{
			vertex->setTex(roundToTexelCenter(vertex->tex(), width, height, offset));
		}
	}
}


bool nv::fixTileAtlasSeams(const HalfEdge::Mesh * mesh, FloatImage * img, float offset)
{
	bool fixed = true;
	
	const uint w = img->width();
	const uint h = img->height();
	const Vector2 dim((float)w, (float)h);
	
	// Process edge seams.
	const uint edgeCount = mesh->edgeCount();
	for(uint e = 0; e < edgeCount; e++) {
		const HalfEdge::Edge * edge = mesh->edgeAt(e);
		
		if (edge->isBoundary() || !edge->isSeam()) {
			continue;
		}
		
		const HalfEdge::Vertex * va0 = edge->from();
		const HalfEdge::Vertex * va1 = edge->to();
		const HalfEdge::Vertex * vb0 = edge->pair()->to();
		const HalfEdge::Vertex * vb1 = edge->pair()->from();

		// Round to nearest texel.
		const Vector2 ta0 = floor(scale(va0->tex(), dim) + Vector2(offset));
		const Vector2 ta1 = floor(scale(va1->tex(), dim) + Vector2(offset));
		const Vector2 tb0 = floor(scale(vb0->tex(), dim) + Vector2(offset));
		const Vector2 tb1 = floor(scale(vb1->tex(), dim) + Vector2(offset));


		if (ta0 == tb0 && ta1 == tb1) {
			// Not a seam!
			continue;
		}

		if (edge->id() > edge->pair()->id()) {
			// Already processed.
			continue;
		}


		// Make sure it's seamless.
		if (ta0.x() != ta1.x() && ta0.y() != ta1.y()) {
			fixed = false;
			continue; // not horizontal nor vertical.
		}
		if (tb0.x() != tb1.x() && tb0.y() != tb1.y()) {
			fixed = false;
			continue; // not horizontal nor vertical.
		}
		
		Vector2 ta = ta0;
		Vector2 dta = ta1 - ta0;
		int numSteps = int(length(dta));
		dta = round(dta * (1.0f / numSteps));
		
		nvDebugCheck(dta.x() == 0.0f || dta.y() == 0.0f);
		nvDebugCheck(fabs(dta.x()) == 1.0f || fabs(dta.y()) == 1.0f);
		
		Vector2 tb = tb0;
		Vector2 dtb = tb1 - tb0;
		int numSteps2 = int(length(dtb));
		dtb = round(dtb * (1.0f / numSteps2));
		
		if (numSteps != numSteps2) {
			// Edges have different scales -> not seamless.
			fixed = false;
			dtb = scale(vb1->tex() - vb0->tex(), dim);
			continue;
		}
		
		nvDebugCheck(dtb.x() == 0.0f || dtb.y() == 0.0f);
		nvDebugCheck(fabs(dtb.x()) == 1.0f || fabs(dtb.y()) == 1.0f);
		
		ta += dta;
		tb += dtb;

		// Traverse edge starting at (ta0,tb0), skip endpoints.
		for(int i = 1; i < numSteps; i++) {
			
			const int xa = (int)ta.x();
			const int ya = (int)ta.y();
			
			const int xb = (int)tb.x();
			const int yb = (int)tb.y();
			
			const uint channelCount = img->componentNum();
			for(uint c = 0; c < channelCount; c++) {
				float value = (img->pixel(xa, ya, c) + img->pixel(xb, yb, c)) * 0.5f;
				img->setPixel(value, xa, ya, c);
				img->setPixel(value, xb, yb, c);
			}
			
			ta += dta;
			tb += dtb;
		}
	}
	
	fixCornerSeams(mesh, img);

	return fixed;
}


bool nv::fixCornerSeams(const HalfEdge::Mesh * mesh, FloatImage * img, float offset/*= 0*/)
{
	const uint w = img->width();
	const uint h = img->height();

	//img->clear(0.5);

	// Process vertex corners.
	const uint vertexCount = mesh->vertexCount();
	for(uint v = 0; v < vertexCount; v++)
	{
		const HalfEdge::Vertex * vertex = mesh->vertexAt(v);
		
		const uint colocalCount = vertex->colocalCount();
		if (colocalCount == 1) {
			continue;
		}
		
		// For each channel.
		const uint channelCount = img->componentNum();
		for(uint c = 0; c < channelCount; c++)
		{
			float value = 0.0f;
			
			for(HalfEdge::Vertex::ConstVertexIterator it(vertex->colocals()); !it.isDone(); it.advance())
			{
				const HalfEdge::Vertex * colocal = it.current();
			
				// Is this rounded correctly?
				const int x = (int)floor(colocal->tex().x() * w + offset);
				const int y = (int)floor(colocal->tex().y() * h + offset);
				
				value += img->pixel(x, y, c);
			}
			
			value *= 1.0f / colocalCount;
			
			// Write average value.
			for(HalfEdge::Vertex::ConstVertexIterator it(vertex->colocals()); !it.isDone(); it.advance())
			{
				const HalfEdge::Vertex * colocal = it.current();
				
				// Is this rounded correctly?
				const int x = (int)floor(colocal->tex().x() * w + offset);
				const int y = (int)floor(colocal->tex().y() * h + offset);
				
				img->setPixel(value, x, y, c);
			}
		}
	}

	return true;
}
