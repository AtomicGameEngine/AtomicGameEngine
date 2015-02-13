// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

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
    OBJECT(ProcSky);

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

    void OnNodeSet(Node* node);

    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
    void Initialize();

    void ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results);

    /// Recalculate the world-space bounding box.
    virtual void OnWorldBoundingBoxUpdate();


};

}
