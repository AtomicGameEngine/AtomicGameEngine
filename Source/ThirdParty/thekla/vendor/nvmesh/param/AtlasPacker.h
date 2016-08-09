// This code is in the public domain -- castano@gmail.com

#pragma once
#ifndef NV_MESH_ATLASPACKER_H
#define NV_MESH_ATLASPACKER_H

#include "nvcore/RadixSort.h"
#include "nvmath/Vector.h"
#include "nvmath/Random.h"
#include "nvimage/BitMap.h"

#include "nvmesh/nvmesh.h"


namespace nv
{
    class Atlas;
    class Chart;

	struct AtlasPacker
	{
		AtlasPacker(Atlas * atlas);
		~AtlasPacker();

        void packCharts(int quality, float texelArea, int padding);
        float computeAtlasUtilization() const;
        uint width() const { return m_width; }
        uint height() const { return m_height; }

    private:

        void findChartLocation(int quality, const BitMap * bitmap, Vector2::Arg extents, int w, int h, int * best_x, int * best_y, int * best_w, int * best_h, int * best_r);
        void findChartLocation_bruteForce(const BitMap * bitmap, Vector2::Arg extents, int w, int h, int * best_x, int * best_y, int * best_w, int * best_h, int * best_r);
        void findChartLocation_random(const BitMap * bitmap, Vector2::Arg extents, int w, int h, int * best_x, int * best_y, int * best_w, int * best_h, int * best_r, int minTrialCount);

        void drawChartBitmap(const Chart * chart, BitMap * bitmap, int padding);
        
        bool canAddChart(const BitMap * bitmap, int w, int h, int x, int y, int r);
        void checkCanAddChart(const Chart * chart, int w, int h, int x, int y, int r);
        void addChart(const Chart * chart, int w, int h, int x, int y, int r);

        static bool checkBitsCallback(void * param, int x, int y, Vector3::Arg bar, Vector3::Arg dx, Vector3::Arg dy, float coverage);
        static bool setBitsCallback(void * param, int x, int y, Vector3::Arg bar, Vector3::Arg dx, Vector3::Arg dy, float coverage);

    private:

        Atlas * m_atlas;
        BitMap m_bitmap;
        RadixSort m_radix;

        uint m_width;
        uint m_height;
        int m_padding; // for the callbacks.

        MTRand m_rand;

	};

} // nv namespace

#endif // NV_MESH_ATLASPACKER_H
