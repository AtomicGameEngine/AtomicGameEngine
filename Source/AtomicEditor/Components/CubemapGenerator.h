//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <Atomic/Graphics/GraphicsDefs.h>

#include "EditorComponent.h"

namespace Atomic
{
class Node;
class Zone;
class Camera;
class Viewport;
class Texture2D;
class RenderSurface;
class RenderPath;
class Image;
}

using namespace Atomic;

namespace AtomicEditor
{

class CubemapGenerator : public EditorComponent
{
    ATOMIC_OBJECT(CubemapGenerator, EditorComponent);

public:
    /// Construct.
    CubemapGenerator(Context* context);

    /// Destruct.
    virtual ~CubemapGenerator();

    /// Register object factory.
    static void RegisterObject(Context* context);

    bool Render();

    int GetImageSize() const { return imageSize_; }
    void SetImageSize(int size) { imageSize_ = size; }

protected:

private:

    void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
    void HandleEndFrame(StringHash eventType, VariantMap& eventData);

    bool InitRender();
    bool InitPaths();
    void EndRender();

    void SaveCubemapXML();

    SharedPtr<Image> GetImage(Texture2D* tex2d);

    CubeMapFace GetFaceForCycle(int cycle);
    String GetFaceName(CubeMapFace face);
    Quaternion RotationOf(CubeMapFace face);

    SharedPtr<Node> cameraNode_;
    SharedPtr<Camera> camera_;
    SharedPtr<Viewport> viewport_;
    SharedPtr<Texture2D> renderImage_;
    SharedPtr<RenderSurface> renderSurface_;

    int updateCycle_;
    int imageSize_;

    String namePrefix_;

    String outputPathAbsolute_;
    String resourcePath_;

    WeakPtr<SceneEditor3D> sceneEditor_;

};


}
