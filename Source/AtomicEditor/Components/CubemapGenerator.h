//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
    OBJECT(CubemapGenerator);

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
