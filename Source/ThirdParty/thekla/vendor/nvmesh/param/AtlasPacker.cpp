// This code is in the public domain -- castano@gmail.com

#include "nvmesh.h" // pch

#include "AtlasPacker.h"
#include "nvmesh/geometry/Measurements.h"
#include "nvmesh/halfedge/Vertex.h"
#include "nvmesh/halfedge/Face.h"
#include "nvmesh/param/Atlas.h"
#include "nvmesh/param/Util.h"
#include "nvmesh/raster/Raster.h"

#include "nvmath/Vector.inl"
#include "nvmath/ConvexHull.h"

#include "nvcore/StrLib.h" // debug
#include "nvcore/StdStream.h" // fileOpen

#include <float.h> // FLT_MAX
#include <limits.h> // UINT_MAX

using namespace nv;


AtlasPacker::AtlasPacker(Atlas * atlas) : m_atlas(atlas), m_bitmap(256, 256)
{
    m_width = 0;
    m_height = 0;
}

AtlasPacker::~AtlasPacker()
{
}

// This should compute convex hull and use rotating calipers to find the best box. Currently it uses a brute force method.
static void computeBoundingBox(Chart * chart, Vector2 * majorAxis, Vector2 * minorAxis, Vector2 * minCorner, Vector2 * maxCorner)
{
    // Compute list of boundary points.
    Array<Vector2> points(16);

    HalfEdge::Mesh * mesh = chart->chartMesh();
    const uint vertexCount = mesh->vertexCount();

    for (uint i = 0; i < vertexCount; i++) {
        HalfEdge::Vertex * vertex = mesh->vertexAt(i);
        if (vertex->isBoundary()) {
            points.append(vertex->tex);
        }
    }

    Array<Vector2> hull;

    convexHull(points, hull, 0.00001f);

    // @@ Ideally I should use rotating calipers to find the best box. Using brute force for now.

    float best_area = FLT_MAX;
    Vector2 best_min;
    Vector2 best_max;
    Vector2 best_axis;

    const uint hullCount = hull.count();
    for (uint i = 0, j = hullCount-1; i < hullCount; j = i, i++) {

        if (equal(hull[i], hull[j])) {
            continue;
        }

        Vector2 axis = normalize(hull[i] - hull[j], 0.0f);
        nvDebugCheck(isFinite(axis));

        // Compute bounding box.
        Vector2 box_min(FLT_MAX, FLT_MAX);
        Vector2 box_max(-FLT_MAX, -FLT_MAX);

        for (uint v = 0; v < hullCount; v++) {

           Vector2 point = hull[v];

           float x = dot(axis, point);
           if (x < box_min.x) box_min.x = x;
           if (x > box_max.x) box_max.x = x;

           float y = dot(Vector2(-axis.y, axis.x), point);
           if (y < box_min.y) box_min.y = y;
           if (y > box_max.y) box_max.y = y;
        }

        // Compute box area.
        float area = (box_max.x - box_min.x) * (box_max.y - box_min.y);

        if (area < best_area) {
            best_area = area;
            best_min = box_min;
            best_max = box_max;
            best_axis = axis;
        }
    }

    // Consider all points, not only boundary points, in case the input chart is malformed.
    for (uint i = 0; i < vertexCount; i++) {
        HalfEdge::Vertex * vertex = mesh->vertexAt(i);
        Vector2 point = vertex->tex;

        float x = dot(best_axis, point);
        if (x < best_min.x) best_min.x = x;
        if (x > best_max.x) best_max.x = x;

        float y = dot(Vector2(-best_axis.y, best_axis.x), point);
        if (y < best_min.y) best_min.y = y;
        if (y > best_max.y) best_max.y = y;
    }

    *majorAxis = best_axis;
    *minorAxis = Vector2(-best_axis.y, best_axis.x);
    *minCorner = best_min;
    *maxCorner = best_max;
}


void AtlasPacker::packCharts(int quality, float texelsPerUnit, int padding)
{
    nvCheck(padding >= 0);
    m_padding = padding;

    const uint chartCount = m_atlas->chartCount();

    Array<float> chartOrderArray;
    chartOrderArray.resize(chartCount);

    Array<Vector2> chartExtents;
    chartExtents.resize(chartCount);

    float meshArea = 0;
    for (uint c = 0; c < chartCount; c++) {
        Chart * chart = m_atlas->chartAt(c);

        if (!chart->isDisk()) {
            chartOrderArray[c] = 0;

            // Skip non-disks.
            continue;
        }

        // Compute surface area to sort charts.
        float chartArea = computeSurfaceArea(chart->chartMesh()) * chart->scale;
        meshArea += chartArea;

        // Compute chart scale
        float parametricArea = computeParametricArea(chart->chartMesh());
        float scale = (chartArea / parametricArea) * texelsPerUnit;
        if (parametricArea < NV_EPSILON) {
            scale = 0;
        }
        nvCheck(isFinite(scale));

        // Compute bounding box of chart.
        Vector2 majorAxis, minorAxis, origin, end;
        computeBoundingBox(chart, &majorAxis, &minorAxis, &origin, &end);

        nvCheck(isFinite(majorAxis) && isFinite(minorAxis) && isFinite(origin));

        // Sort charts by perimeter. @@ This is sometimes producing somewhat unexpected results. Is this right?
        chartOrderArray[c] = ((end.x - origin.x) + (end.y - origin.y)) * scale;

        // Translate, rotate and scale vertices. Compute extents.
        Vector2 extents(0.0f);
        HalfEdge::Mesh * mesh = chart->chartMesh();
        const uint vertexCount = mesh->vertexCount();
        for (uint i = 0; i < vertexCount; i++)
        {
            HalfEdge::Vertex * vertex = mesh->vertexAt(i);

            Vector2 tmp;
            tmp.x = dot(vertex->tex, majorAxis);
            tmp.y = dot(vertex->tex, minorAxis);
            tmp -= origin;
            tmp *= scale;
            if (tmp.x < 0 || tmp.y < 0) {
                nvDebug("tmp: %f %f\n", tmp.x, tmp.y);
                nvDebug("scale: %f\n", scale);
                nvDebug("origin: %f %f\n", origin.x, origin.y);
                nvDebug("majorAxis: %f %f\n", majorAxis.x, majorAxis.y);
                nvDebug("minorAxis: %f %f\n", minorAxis.x, minorAxis.y);
                nvDebugBreak();
            }

            vertex->tex = tmp;

            extents = max(extents, tmp);
        }
        nvDebugCheck(extents.x >= 0 && extents.y >= 0);

        // Limit chart size. @@ Ideally we should adjust the scale of the entire mesh.
        if (extents.x > 1024 || extents.y > 1024) {
            float limit = max(extents.x, extents.y);

            scale = 1024 / (limit + 1);

            extents.set(0, 0);
            for (uint i = 0; i < vertexCount; i++)
            {
                HalfEdge::Vertex * vertex = mesh->vertexAt(i);
                vertex->tex *= scale;
                extents = max(extents, vertex->tex);
            }
            nvDebugCheck(extents.x <= 1024 && extents.y <= 1024);
        }

        chartExtents[c] = extents;
    }

    // Sort charts by area.
    m_radix.sort(chartOrderArray);
    const uint32 * ranks = m_radix.ranks();

    // Estimate size of the map based on the mesh surface area and given texel scale.
    float texelCount = meshArea * square(texelsPerUnit) / 0.75f; // Assume 75% utilization.
    if (texelCount < 1) texelCount = 1;
    uint approximateExtent = nextPowerOfTwo(uint(sqrtf(texelCount)));

    // Init bit map.
    m_bitmap.clearAll();
    if (approximateExtent > m_bitmap.width()) {
        m_bitmap.resize(approximateExtent, approximateExtent, false);
    }

    int w = 0;
    int h = 0;

    // Add sorted charts to bitmap.
    for (uint i = 0; i < chartCount; i++)
    {
        uint c = ranks[chartCount - i - 1]; // largest chart first

        Chart * chart = m_atlas->chartAt(c);
 
        if (!chart->isDisk()) continue;

        BitMap chart_bitmap(iceil(chartExtents[c].x) + padding * 2, iceil(chartExtents[c].y) + padding * 2);
        chart_bitmap.clearAll();

        drawChartBitmap(chart, &chart_bitmap, padding);

        int best_x, best_y;
        int best_cw, best_ch;
        int best_r;
        findChartLocation(quality, &chart_bitmap, chartExtents[c], w, h, &best_x, &best_y, &best_cw, &best_ch, &best_r);

        // Update parametric extents.
        w = max(w, best_x + best_cw + padding);
        h = max(h, best_y + best_ch + padding);

        // Resize bitmap if necessary.
        if (uint(w) > m_bitmap.width() || uint(h) > m_bitmap.height()) {
            m_bitmap.resize(nextPowerOfTwo(w), nextPowerOfTwo(h), false);
        }

        // Add chart.
        addChart(chart, w, h, best_x, best_y, best_r);

        // Translate and rotate chart texture coordinates.
        HalfEdge::Mesh * mesh = chart->chartMesh();
        const uint vertexCount = mesh->vertexCount();
        for (uint v = 0; v < vertexCount; v++) {
            HalfEdge::Vertex * vertex = mesh->vertexAt(v);
            Vector2 t = vertex->tex;
            if (best_r) swap(t.x, t.y);
            vertex->tex.x = best_x + t.x;
            vertex->tex.y = best_y + t.y;
            nvCheck(vertex->tex.x >= 0 && vertex->tex.y >= 0);
        }
    }

    w -= padding - 1; // Leave one pixel border!
    h -= padding - 1;

    // This is not the right way to force a square POT texture.
    #if FORCE_POT
    w = nextPowerOfTwo(w);
    h = nextPowerOfTwo(h);
    w = max(w, h);
    h = max(w, h);
    m_bitmap.resize(w, h, false);
    #endif

    m_width = max(0, w);
    m_height = max(0, h);
}


void AtlasPacker::findChartLocation(int quality, const BitMap * bitmap, Vector2::Arg extents, int w, int h, int * best_x, int * best_y, int * best_w, int * best_h, int * best_r)
{
    int attempts = 256;
    if (quality == 1) attempts = 4096;
    if (quality == 2) attempts = 2048;
    if (quality == 3) attempts = 1024;
    if (quality == 4) attempts = 512;

    if (quality == 0 || w*h < attempts)
    {
        findChartLocation_bruteForce(bitmap, extents, w, h, best_x, best_y, best_w, best_h, best_r);
    }
    else
    {
        findChartLocation_random(bitmap, extents, w, h, best_x, best_y, best_w, best_h, best_r, attempts);
    }
}

void AtlasPacker::findChartLocation_bruteForce(const BitMap * bitmap, Vector2::Arg extents, int w, int h, int * best_x, int * best_y, int * best_w, int * best_h, int * best_r)
{
    int best_metric = INT_MAX;

    // Try two different orientations.
    for (int r = 0; r < 2; r++)
    {
        int cw = iceil(extents.x);
        int ch = iceil(extents.y);
        if (r & 1) swap(cw, ch);

        for (int y = 1; y <= h + m_padding; y++) // Leave 1 pixel border on the left
        {
            for (int x = 1; x <= w + m_padding; x++) // Leave 1 pixel border on the top
            {
                // Early out.
                int area = max(w, x+cw+m_padding) * max(h, y+ch+m_padding);
                //int perimeter = max(w, x+cw+m_padding) + max(h, y+ch+m_padding);
                int extents = max(max(w, x+cw+m_padding), max(h, y+ch+m_padding));

                int metric = extents*extents + area;

                if (metric > best_metric) {
                    continue;
                }
                if (metric == best_metric && max(x, y) >= max(*best_x, *best_y)) {
                    // If metric is the same, pick the one closest to the origin.
                    continue;
                }

                if (canAddChart(bitmap, w, h, x, y, r))
                {
                    best_metric = metric;
                    *best_x = x;
                    *best_y = y;
                    *best_w = cw;
                    *best_h = ch;
                    *best_r = r;

                    if (area == w*h)
                    {
                        // Chart is completely inside, do not look at any other location.
                        goto done;
                    }
                }
            }
        }
    }

done:
    nvDebugCheck (best_metric != INT_MAX);
}

void AtlasPacker::findChartLocation_random(const BitMap * bitmap, Vector2::Arg extents, int w, int h, int * best_x, int * best_y, int * best_w, int * best_h, int * best_r, int minTrialCount)
{
    int best_metric = INT_MAX;

    for (int i = 0; i < minTrialCount || best_metric == INT_MAX; i++)
    {
        int r = m_rand.getRange(1);
        int x = 1 + m_rand.getRange(w + m_padding - 1);
        int y = 1 + m_rand.getRange(h + m_padding - 1);

        int cw = iceil(extents.x);
        int ch = iceil(extents.y);
        if (r & 1) swap(cw, ch);

        // Early out.
        int area = max(w, x+cw+m_padding) * max(h, y+ch+m_padding);
        //int perimeter = max(w, x+cw+m_padding) + max(h, y+ch+m_padding);
        int extents = max(max(w, x+cw+m_padding), max(h, y+ch+m_padding));

        int metric = extents*extents + area;

        if (metric > best_metric) {
            continue;
        }
        if (metric == best_metric && min(x, y) > min(*best_x, *best_y)) {
            // If metric is the same, pick the one closest to the origin.
            continue;
        }

        if (canAddChart(bitmap, w, h, x, y, r))
        {
            best_metric = metric;
            *best_x = x;
            *best_y = y;
            *best_w = cw;
            *best_h = ch;
            *best_r = r;

            if (area == w*h)
            {
                // Chart is completely inside, do not look at any other location.
                break;
            }
        }
    }
}


void AtlasPacker::drawChartBitmap(const Chart * chart, BitMap * bitmap, int padding)
{
    const int w = bitmap->width();
    const int h = bitmap->height();
    const Vector2 extents = Vector2(float(w), float(h));
    
    // Rasterize chart faces, check that all bits are not set.
    const uint faceCount = chart->faceCount();
    for (uint f = 0; f < faceCount; f++)
    {
        const HalfEdge::Face * face = chart->chartMesh()->faceAt(f);
        
        Vector2 vertices[4];

        uint edgeCount = 0;
        for (HalfEdge::Face::ConstEdgeIterator it(face->edges()); !it.isDone(); it.advance())
        {
            if (edgeCount < 4)
            {
                vertices[edgeCount] = it.vertex()->tex + Vector2(float(padding), float(padding));
            }
            edgeCount++;
        }

        if (edgeCount == 3)
        {
            Raster::drawTriangle(true, extents, true, vertices, AtlasPacker::setBitsCallback, bitmap);
        }
        else
        {
            Raster::drawQuad(true, extents, true, vertices, AtlasPacker::setBitsCallback, bitmap);
        }
    }

    // Expand chart by padding pixels. (dilation)
    BitMap tmp(w, h);
    for (int i = 0; i < padding; i++) {
        tmp.clearAll();

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                bool b = bitmap->bitAt(x, y);
                if (!b) {
                    if (x > 0) {
                        b |= bitmap->bitAt(x - 1, y);
                        if (y > 0) b |= bitmap->bitAt(x - 1, y - 1);
                        if (y < h-1) b |= bitmap->bitAt(x - 1, y + 1);
                    }
                    if (y > 0) b |= bitmap->bitAt(x, y - 1);
                    if (y < h-1) b |= bitmap->bitAt(x, y + 1);
                    if (x < w-1) {
                        b |= bitmap->bitAt(x + 1, y);
                        if (y > 0) b |= bitmap->bitAt(x + 1, y - 1);
                        if (y < h-1) b |= bitmap->bitAt(x + 1, y + 1);
                    }
                }
                if (b) tmp.setBitAt(x, y);
            }
        }

        swap(tmp, *bitmap);
    }
}

bool AtlasPacker::canAddChart(const BitMap * bitmap, int atlas_w, int atlas_h, int offset_x, int offset_y, int r)
{
    nvDebugCheck(r == 0 || r == 1);

    // Check whether the two bitmaps overlap.

    const int w = bitmap->width();
    const int h = bitmap->height();
    
    if (r == 0) {
        for (int y = 0; y < h; y++) {
            int yy = y + offset_y - m_padding;
            if (yy >= 0) {
                for (int x = 0; x < w; x++) {
                    int xx = x + offset_x - m_padding;
                    if (xx >= 0) {
                        if (bitmap->bitAt(x, y)) {
                            if (xx < atlas_w && yy < atlas_h) {
                                if (m_bitmap.bitAt(xx, yy)) return false;
                            }
                        }
                    }
                }
            }
        }
    }
    else if (r == 1) {
        for (int y = 0; y < h; y++) {
            int xx = y + offset_x - m_padding;
            if (xx >= 0) {
                for (int x = 0; x < w; x++) {
                    int yy = x + offset_y - m_padding;
                    if (yy >= 0) {
                        if (bitmap->bitAt(x, y)) {
                            if (xx < atlas_w && yy < atlas_h) {
                                if (m_bitmap.bitAt(xx, yy)) return false;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return true;
}


void AtlasPacker::checkCanAddChart(const Chart * chart, int w, int h, int x, int y, int r)
{
    nvDebugCheck(r == 0 || r == 1);
    Vector2 extents = Vector2(float(w), float(h));
    Vector2 offset = Vector2(float(x), float(y));

    // Rasterize chart faces, set bits.
    const uint faceCount = chart->faceCount();
    for (uint f = 0; f < faceCount; f++)
    {
        const HalfEdge::Face * face = chart->chartMesh()->faceAt(f);
        Vector2 vertices[4];
        uint edgeCount = 0;
        for (HalfEdge::Face::ConstEdgeIterator it(face->edges()); !it.isDone(); it.advance())
        {
            if (edgeCount < 4)
            {
                Vector2 t = it.vertex()->tex;
                if (r == 1) swap(t.x, t.y);
                vertices[edgeCount] = t + offset;
            }
            edgeCount++;
        }

        if (edgeCount == 3)
        {
            Raster::drawTriangle(true, extents, true, vertices, AtlasPacker::checkBitsCallback, &m_bitmap);
        }
        else
        {
            Raster::drawQuad(true, extents, true, vertices, AtlasPacker::checkBitsCallback, &m_bitmap);
        }
    }
}


void AtlasPacker::addChart(const Chart * chart, int w, int h, int x, int y, int r)
{
    nvDebugCheck(r == 0 || r == 1);
    Vector2 extents = Vector2(float(w), float(h));
    Vector2 offset = Vector2(float(x), float(y));

    // Rasterize chart faces, set bits.
    const uint faceCount = chart->faceCount();
    for (uint f = 0; f < faceCount; f++)
    {
        const HalfEdge::Face * face = chart->chartMesh()->faceAt(f);
        Vector2 vertices[4];

        uint edgeCount = 0;
        for (HalfEdge::Face::ConstEdgeIterator it(face->edges()); !it.isDone(); it.advance())
        {
            if (edgeCount < 4)
            {
                Vector2 t = it.vertex()->tex;
                if (r == 1) swap(t.x, t.y);
                vertices[edgeCount] = t + offset;
            }
            edgeCount++;
        }

        if (edgeCount == 3)
        {
            Raster::drawTriangle(true, extents, true, vertices, AtlasPacker::setBitsCallback, &m_bitmap);
        }
        else
        {
            Raster::drawQuad(true, extents, true, vertices, AtlasPacker::setBitsCallback, &m_bitmap);
        }
    }
}


/*static*/ bool AtlasPacker::checkBitsCallback(void * param, int x, int y, Vector3::Arg, Vector3::Arg, Vector3::Arg, float)
{
    BitMap * bitmap = (BitMap * )param;

    nvDebugCheck(bitmap->bitAt(x, y) == false);

    return true;
}

/*static*/ bool AtlasPacker::setBitsCallback(void * param, int x, int y, Vector3::Arg, Vector3::Arg, Vector3::Arg, float area)
{
    BitMap * bitmap = (BitMap * )param;

    if (area > 0.0001) {
        bitmap->setBitAt(x, y);
    }

    return true;
}


float AtlasPacker::computeAtlasUtilization() const {
    const uint w = m_width;
    const uint h = m_height;
    nvDebugCheck(w <= m_bitmap.width());
    nvDebugCheck(h <= m_bitmap.height());

    uint count = 0;
    for (uint y = 0; y < h; y++) {
        for (uint x = 0; x < w; x++) {
            count += m_bitmap.bitAt(x, y);
        }
    }

    return float(count) / (w * h);
}
