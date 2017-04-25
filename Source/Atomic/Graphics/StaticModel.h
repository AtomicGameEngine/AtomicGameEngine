//
// Copyright (c) 2008-2017 the Urho3D project.
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

#include "../Graphics/Drawable.h"

namespace Atomic
{

class Model;

/// Static model per-geometry extra data.
struct StaticModelGeometryData
{
    /// Geometry center.
    Vector3 center_;
    /// Current LOD level.
    unsigned lodLevel_;

    // ATOMIC BEGIN

    bool enabled_;
    Geometry* batchGeometry_;

    // ATOMIC END

};

/// Static model component.
class ATOMIC_API StaticModel : public Drawable
{
    ATOMIC_OBJECT(StaticModel, Drawable);

public:
    /// Construct.
    StaticModel(Context* context);
    /// Destruct.
    ~StaticModel();
    /// Register object factory. Drawable must be registered first.
    static void RegisterObject(Context* context);

    /// Process octree raycast. May be called from a worker thread.
    virtual void ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results);
    /// Calculate distance and prepare batches for rendering. May be called from worker thread(s), possibly re-entrantly.
    virtual void UpdateBatches(const FrameInfo& frame);
    /// Return the geometry for a specific LOD level.
    virtual Geometry* GetLodGeometry(unsigned batchIndex, unsigned level);
    /// Return number of occlusion geometry triangles.
    virtual unsigned GetNumOccluderTriangles();
    /// Draw to occlusion buffer. Return true if did not run out of triangles.
    virtual bool DrawOcclusion(OcclusionBuffer* buffer);

    /// Set model.
    virtual void SetModel(Model* model);
    /// Set material on all geometries.
    virtual void SetMaterial(Material* material);
    /// Set material on one geometry. Return true if successful.
    virtual bool SetMaterial(unsigned index, Material* material);
    /// Set occlusion LOD level. By default (M_MAX_UNSIGNED) same as visible.
    void SetOcclusionLodLevel(unsigned level);
    /// Apply default materials from a material list file. If filename is empty (default), the model's resource name with extension .txt will be used.
    void ApplyMaterialList(const String& fileName = String::EMPTY);

    /// Return model.
    Model* GetModel() const { return model_; }

    /// Return number of geometries.
    unsigned GetNumGeometries() const { return geometries_.Size(); }

    /// Return material by geometry index.
    virtual Material* GetMaterial(unsigned index = 0) const;

    /// Return occlusion LOD level.
    unsigned GetOcclusionLodLevel() const { return occlusionLodLevel_; }

    /// Determines if the given world space point is within the model geometry.
    bool IsInside(const Vector3& point) const;
    /// Determines if the given local space point is within the model geometry.
    bool IsInsideLocal(const Vector3& point) const;

    /// Set model attribute.
    void SetModelAttr(const ResourceRef& value);
    /// Set materials attribute.
    void SetMaterialsAttr(const ResourceRefList& value);
    /// Return model attribute.
    ResourceRef GetModelAttr() const;
    /// Return materials attribute.
    const ResourceRefList& GetMaterialsAttr() const;

    // ATOMIC BEGIN

    /// Get whether a named submesh is visible
    bool GetGeometryVisible(const String& name);
    /// Show a named submesh
    void ShowGeometry(const String& name);
    /// Hide a named submesh
    void HideGeometry(const String& name);

    /// Returns true if any geometry is hidden in the model
    bool GetGeometryHidden() const { return geometryDisabled_; }

    void SetGeometryEnabledAttr(const VariantVector& value);
    const VariantVector& GetGeometryEnabledAttr() const;

    bool GetLightmap() const { return lightmap_; }
    void SetLightmap(bool lightmap) { lightmap_ = lightmap; }

    float GetLightmapScale() const { return lightmapScale_; }
    void SetLightmapScale(float scale) { lightmapScale_ = scale; }

    unsigned GetLightmapSize() const { return lightmapSize_; }
    void SetLightmapSize(unsigned size) { lightmapSize_ = size; }

    unsigned GetLightmapIndex() const { return lightmapIndex_; }
    void SetLightmapIndex(unsigned idx) { lightmapIndex_ = idx; }

    const Vector4& GetLightmapTilingOffset() const { return lightmapTilingOffset_; }
    void SetLightmapTilingOffset(Vector4 tilingOffset) { lightmapTilingOffset_ = tilingOffset; }

    // ATOMIC END

protected:
    /// Recalculate the world-space bounding box.
    virtual void OnWorldBoundingBoxUpdate();
    /// Set local-space bounding box.
    void SetBoundingBox(const BoundingBox& box);
    /// Set number of geometries.
    void SetNumGeometries(unsigned num);
    /// Reset LOD levels.
    void ResetLodLevels();
    /// Choose LOD levels based on distance.
    void CalculateLodLevels();

    /// Extra per-geometry data.
    PODVector<StaticModelGeometryData> geometryData_;
    /// All geometries.
    Vector<Vector<SharedPtr<Geometry> > > geometries_;
    /// Model.
    SharedPtr<Model> model_;
    /// Occlusion LOD level.
    unsigned occlusionLodLevel_;
    /// Material list attribute.
    mutable ResourceRefList materialsAttr_;

    // ATOMIC BEGIN

    // Apply geometry hiding when updating batches
    void UpdateBatchesHideGeometry();

    mutable VariantVector geometryEnabled_;
    /// true if any geometry has been disabled
    mutable bool geometryDisabled_;

    bool lightmap_;
    float lightmapScale_;
    unsigned lightmapSize_;

    unsigned lightmapIndex_;
    Vector4 lightmapTilingOffset_;

    // ATOMIC END

private:
    /// Handle model reload finished.
    void HandleModelReloadFinished(StringHash eventType, VariantMap& eventData);
};

}
