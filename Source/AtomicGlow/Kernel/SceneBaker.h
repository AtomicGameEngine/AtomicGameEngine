// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Atomic/Scene/Scene.h>
#include "GlowTypes.h"

using namespace Atomic;

namespace AtomicGlow
{

class LightRay;
class BakeMesh;
class BakeLight;
class EmbreeScene;

class SceneBaker : public Object
{
    ATOMIC_OBJECT(SceneBaker, Object)

    public:

    SceneBaker(Context* context, const String& projectPath);
    virtual ~SceneBaker();    

    bool Light(const GlowLightMode lightMode);
    void LightFinishCycle();

    bool LoadScene(const String& filename);

    bool GenerateLightmaps();

    const VectorBuffer& GetBakeData() const { return bakeData_; }

    GlowLightMode GetCurrentLightMode() const { return currentLightMode_; }

    void QueryLights(const BoundingBox& bbox, PODVector<BakeLight*>& lights);

    void TraceRay(LightRay* lightRay, const PODVector<AtomicGlow::BakeLight *>& bakeLights);

    Scene* GetScene() const { return scene_; }

    EmbreeScene* GetEmbreeScene() const { return embreeScene_; }

    int GetCurrentGIBounce() const {return currentGIPass_; }

    bool WriteBakeData(VectorBuffer& buffer);

    bool GetStandaloneMode() const { return standaloneMode_; }

    void SetStandaloneMode(bool standaloneMode) { standaloneMode_ = standaloneMode; }

    const BoundingBox& GetSceneBounds() const { return sceneBounds_; }

private:

    //void FilterLightmap(Image* lightmap);
    //void EmitLightmap(int lightMapIndex);
    //bool TryAddStaticModelBaker(StaticModelBaker *bakeModel);

    bool Preprocess();

    /// process direct (and ao/ambient) lighting, returns true if there is work to be done
    bool LightDirect();
    void LightDirectFinish();

    /// process a GI cycle, returns true if there is work to be done
    bool LightGI();
    void LightGIFinish();

    bool SaveLitScene();

    // Direct lighting

    void DirectLight( LightRay* lightRay, const PODVector<BakeLight*>& bakeLights);
    Color DirectLightFromPoint( LightRay* lightRay, const BakeLight* light ) const;
    Color DirectLightFromPointSet( LightRay* lightRay, const BakeLight* light ) const;
    float DirectLightInfluenceFromPoint(LightRay* lightRay, const Vector3 &point, const BakeLight *light ) const;

    // New GI

    bool EmitPhotons();
    void IndirectLight(LightRay* lightRay);

    SharedPtr<Scene> scene_;
    SharedPtr<EmbreeScene> embreeScene_;

    Vector<SharedPtr<BakeMesh>> bakeMeshes_;

    Vector<SharedPtr<BakeLight>> bakeLights_;

    VectorBuffer bakeData_;

    GlowLightMode currentLightMode_;

    int currentGIPass_;

    String projectPath_;
    bool standaloneMode_;

    BoundingBox sceneBounds_;

};

}
