//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define fmax max
#define fmin min
#endif

#include "../Graphics/Drawable.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/IndexBuffer.h"
#include "../Graphics/VertexBuffer.h"
#include "../Graphics/Geometry.h"
#include "../Graphics/Material.h"
#include "../Graphics/Zone.h"
#include "../Scene/Node.h"

namespace Atomic
{


class ProcSky : public Drawable
{
    ATOMIC_OBJECT(ProcSky, Drawable);

public:
    /// Construct.
    ///
    ProcSky(Context* context);

    /// Destruct.
    ~ProcSky();

    /// Register object factory. Drawable must be registered first.
    static void RegisterObject(Context* context);

    float SetDayTime(float time);
    float GetDayTime()
    {
        return dayTime_;
    }

    void SetAutoUpdate(bool autoUpdate)
    {
        autoUpdate_ = autoUpdate;
    }

    /// Calculate distance and prepare batches for rendering. May be called from worker thread(s), possibly re-entrantly.
    virtual void UpdateBatches(const FrameInfo& frame);
    /// Prepare geometry for rendering. Called from a worker thread if possible (no GPU update.)
    virtual void UpdateGeometry(const FrameInfo& frame);
    /// Return whether a geometry update is necessary, and if it can happen in a worker thread.
    virtual UpdateGeometryType GetUpdateGeometryType();

    static float GetTimeOfDay() { return timeOfDay_; }

protected:

    SharedPtr<Texture2D> skyTexture_;
    SharedPtr<Material> skyMaterial_;
    SharedPtr<Light> sunlight_;

    /// Geometry.
    SharedPtr<Geometry> geometry_;
    /// Vertex buffer.
    SharedPtr<VertexBuffer> vertexBuffer_;

    /// Index buffer.
    SharedPtr<IndexBuffer> indexBuffer_;

    SharedPtr<Zone> zone_;

    void UpdateVertexBuffer(const FrameInfo& frame);
    void UpdateIndexBuffer();

    bool autoUpdate_;

    float dayTime_;
    float lastDayTimeUpdate_;
    float shadowFade_;

    static float timeOfDay_;

    float sunAngle_;
    float sunSize_;

    Color topColor_;
    Color horizColor_;
    Color lerpColor_;
    Color fogColor_;

    /// Custom world transform.
    Matrix3x4 customWorldTransform_;

    bool initialized_;
    bool flipped_;

    void OnNodeSet(Node* node);

    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
    void HandleBeginViewUpdate(StringHash eventType, VariantMap& eventData);

    void Initialize();

    void ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results);

    /// Recalculate the world-space bounding box.
    virtual void OnWorldBoundingBoxUpdate();


};

}
