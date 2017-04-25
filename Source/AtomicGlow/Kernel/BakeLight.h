// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
// Copyright 2009-2017 Intel Corporation
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

#include <Atomic/Core/Mutex.h>
#include "BakeNode.h"

namespace Atomic
{
    class Light;
    class Zone;
}

using namespace Atomic;

namespace AtomicGlow
{

class LightRay;
class BakeMesh;

class BakeLight : public BakeNode
{
    ATOMIC_OBJECT(BakeLight, BakeNode)

    public:

    BakeLight(Context* context, SceneBaker *sceneBaker);
    virtual ~BakeLight();

    virtual void SetLight(Atomic::Light* light) = 0;

    virtual void Light(LightRay* ray) = 0;

protected:

    // Common light properties
    Color color_;
    Vector3 position_;
    Vector3 direction_;

    float range_;


private:

};

// Zone ambient, etc
class ZoneBakeLight : public BakeLight
{
    ATOMIC_OBJECT(ZoneBakeLight, BakeLight)

    public:

    ZoneBakeLight(Context* context, SceneBaker* sceneBaker);
    virtual ~ZoneBakeLight();

    void Light(LightRay* lightRay);
    void SetLight(Atomic::Light* light) {}

    void SetZone(Zone* zone);

protected:

private:

    Zone* zone_;

};


class DirectionalBakeLight : public BakeLight
{
    ATOMIC_OBJECT(DirectionalBakeLight, BakeLight)

    public:

    DirectionalBakeLight(Context* context, SceneBaker* sceneBaker);
    virtual ~DirectionalBakeLight();

    void Set(const Vector3& direction, const Vector3& radiance, float cosAngle);

    void Light(LightRay* lightRay);

    void SetLight(Atomic::Light* light);


protected:


private:

};

class PointBakeLight : public BakeLight
{
    ATOMIC_OBJECT(PointBakeLight, BakeLight)

    public:

    PointBakeLight(Context* context, SceneBaker* sceneBaker);
    virtual ~PointBakeLight();

    void Light(LightRay* lightRay);

    void SetLight(Atomic::Light* light);

protected:


private:

};

// BakeMesh Bounce Lighting
class BounceBakeLight : public BakeLight
{
    ATOMIC_OBJECT(BounceBakeLight, BakeLight)

    public:

    BounceBakeLight(Context* context, SceneBaker* sceneBaker);
    virtual ~BounceBakeLight();

    void Light(LightRay* lightRay);

    void SetLight(Atomic::Light* light);

    void SetBakeMesh(BakeMesh* bakeMesh);

    void SetBounceSamples(const PODVector<BounceSample>& bounceSamples);

    unsigned GetNumBounceSamples() const { return bounceSamples_.Size(); }

protected:


private:

    static Mutex sortMutex_;

    PODVector<BounceSample> bounceSamples_;

    WeakPtr<BakeMesh> bakeMesh_;

};



}
