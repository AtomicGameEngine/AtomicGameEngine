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

#include <Atomic/Core/Object.h>

#include "BakeMesh.h"
#include "LightMap.h"

namespace Atomic
{

class Image;

}

namespace AtomicGlow
{

using namespace Atomic;

// on 4 pixel boundry to prevent bilinear bleeds and for DDS 4x4 compression
static const int LIGHTMAP_PADDING = 4;

class LightMapPacker : public Object
{
    ATOMIC_OBJECT(LightMapPacker, Object)

    public:

    LightMapPacker(Context* context);
    virtual ~LightMapPacker();

    void AddRadianceMap(RadianceMap* radianceMap);

    void Pack();

    bool SaveLightmaps(const String& projectPath, const String& scenePath);

private:

    /// Attempts to add a radiance map to current working set
    bool TryAddRadianceMap(RadianceMap* radMap);
    void DilatedBlit(const Image* srcImage, Image* destImage, const IntRect& destRect);
    void EmitLightmap(unsigned lightMapID);

    Vector<SharedPtr<RadianceMap>> radMaps_;
    Vector<SharedPtr<LightMap>> lightMaps_;


    PODVector<RadianceMap*> workingSet_;

};


}
