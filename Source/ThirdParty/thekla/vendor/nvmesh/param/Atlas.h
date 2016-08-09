// Copyright NVIDIA Corporation 2006 -- Ignacio Castano <icastano@nvidia.com>

#pragma once
#ifndef NV_MESH_ATLAS_H
#define NV_MESH_ATLAS_H

#include "nvcore/Array.h"
#include "nvcore/Ptr.h"
#include "nvmesh/nvmesh.h"
#include "nvmesh/halfedge/Mesh.h"


namespace nv
{
    namespace HalfEdge { class Mesh; }

    class Chart;

    struct SegmentationSettings
    {
        SegmentationSettings();

        float maxChartArea;
        float maxBoundaryLength;

        float proxyFitMetricWeight;
        float roundnessMetricWeight;
        float straightnessMetricWeight;
        float normalSeamMetricWeight;
        float textureSeamMetricWeight;
    };


    /// An atlas is a set of charts.
    class Atlas
    {
    public:

        Atlas(const HalfEdge::Mesh * mesh);
        ~Atlas();

        uint chartCount() const;
        uint vertexCount () const { return m_totalVertexCount; }

        const Chart * chartAt(uint i) const;
        Chart * chartAt(uint i);

        // Compute a trivial seamless texture similar to ZBrush.
        //bool computeSeamlessTextureAtlas(bool groupFaces = true, bool scaleTiles = false, uint w = 1024, uint h = 1024);

        // Extract the charts of the input mesh.
        void extractCharts();

        // Compute charts using a simple segmentation algorithm.
        void computeCharts(const SegmentationSettings & settings);

        void parameterizeCharts();

        // Pack charts in the smallest possible rectangle.
        float packCharts(int quality, float texelArea, int padding);

        uint faceChartAt(uint i) const { return m_faceChart[i]; }
        uint faceIndexWithinChartAt(uint i) const { return m_faceIndex[i]; }
        uint vertexCountBeforeChartAt(uint i) const { return m_chartVertexCountPrefixSum[i]; }

        uint width() const { return m_width; }
        uint height() const { return m_height; }

    private:

        uint m_width;
        uint m_height;

        const HalfEdge::Mesh * m_mesh;

        Array<Chart *> m_chartArray;
        Array<uint> m_chartVertexCountPrefixSum;

        Array<uint> m_faceChart; // the chart of every face of the input mesh.
        Array<uint> m_faceIndex; // the index within the chart for every face of the input mesh.

        uint m_totalVertexCount;

    };


    /// A chart is a connected set of faces with a certain topology (usually a disk).
    class Chart
    {
    public:

        Chart();

        void build(const HalfEdge::Mesh * originalMesh, const Array<uint> & faceArray);

        bool closeHoles();

        bool isDisk() const { return m_isDisk; }

        uint vertexCount() const { return m_chartMesh->vertexCount(); }
        uint colocalVertexCount() const { return m_unifiedMesh->vertexCount(); }

        uint faceCount() const { return m_faceArray.count(); }
        uint faceAt(uint i) const { return m_faceArray[i]; }

        const HalfEdge::Mesh * chartMesh() const { return m_chartMesh.ptr(); }
        HalfEdge::Mesh * chartMesh() { return m_chartMesh.ptr(); }
        const HalfEdge::Mesh * unifiedMesh() const { return m_unifiedMesh.ptr(); }
        HalfEdge::Mesh * unifiedMesh() { return m_unifiedMesh.ptr(); }

        //uint vertexIndex(uint i) const { return m_vertexIndexArray[i]; }

        uint mapChartVertexToOriginalVertex(uint i) const { return m_chartToOriginalMap[i]; }
        uint mapChartVertexToUnifiedVertex(uint i) const { return m_chartToUnifiedMap[i]; }

        void transferParameterization();

        float scale;

    private:

        bool closeLoop(uint start, const Array<HalfEdge::Edge *> & loop);

        // Chart mesh.
        AutoPtr<HalfEdge::Mesh> m_chartMesh;
        AutoPtr<HalfEdge::Mesh> m_unifiedMesh;

        bool m_isDisk;

        // List of faces of the original mesh that belong to this chart.
        Array<uint> m_faceArray;

        // Map vertices of the chart mesh to vertices of the original mesh.
        Array<uint> m_chartToOriginalMap;

        Array<uint> m_chartToUnifiedMap;

    };


} // nv namespace

#endif // NV_MESH_ATLAS_H
