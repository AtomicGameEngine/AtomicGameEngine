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

#include <Atomic/Core/Context.h>
#include <Atomic/Core/CoreEvents.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>

#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Camera.h>
#include <Atomic/Graphics/Viewport.h>
#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/TextureCube.h>

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/XMLFile.h>

#include <Atomic/Scene/Node.h>

#include <ToolCore/ToolSystem.h>
#include <ToolCore/Project/Project.h>

#include "../Editors/SceneEditor3D/SceneEditor3DEvents.h"
#include "../Editors/SceneEditor3D/SceneEditor3D.h"

#include "CubemapGenerator.h"

namespace AtomicEditor
{

CubemapGenerator::CubemapGenerator(Context *context) : EditorComponent(context),
    updateCycle_(0),
    imageSize_(512),
    namePrefix_("Cubemap")
{

}

CubemapGenerator::~CubemapGenerator()
{

}

bool CubemapGenerator::Render()
{

    if(!InitRender())
    {
        ATOMIC_LOGERRORF("Unable to init render");
        return false;
    }

    GetScene()->SendEvent(E_CUBEMAPRENDERBEGIN);
    SubscribeToEvent(E_BEGINFRAME, ATOMIC_HANDLER(CubemapGenerator, HandleBeginFrame));
    SubscribeToEvent(E_ENDFRAME, ATOMIC_HANDLER(CubemapGenerator, HandleEndFrame));

    return true;

}

bool CubemapGenerator::InitPaths()
{

    String scenePath = sceneEditor_->GetFullPath();

    String pathName;
    String fileName;
    String ext;

    SplitPath(scenePath, pathName, fileName, ext);

    outputPathAbsolute_ = pathName + "Cubemaps/" + fileName + "/";

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (!fileSystem->DirExists(outputPathAbsolute_))
    {
        if (!fileSystem->CreateDirs(pathName,  "Cubemaps/" + fileName + "/"))
        {
            ATOMIC_LOGERRORF("CubemapGenerator::InitRender - Unable to create path: %s", outputPathAbsolute_.CString());
            return false;
        }
    }

    // TODO: There should be a better way of getting the resource path
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    resourcePath_ = outputPathAbsolute_;
    resourcePath_.Replace(project->GetResourcePath(), "");
    resourcePath_ = AddTrailingSlash(resourcePath_);

    return true;

}

bool CubemapGenerator::InitRender()
{

    sceneEditor_ = GetSceneEditor();

    if (sceneEditor_.Null())
    {
        ATOMIC_LOGERROR("CubemapGenerator::InitRender - unable to get scene editor");
        return false;
    }

    if (!InitPaths())
        return false;

    cameraNode_ = node_->CreateChild("CubeMapRenderCamera");
    cameraNode_->SetTemporary(true);

    camera_ = cameraNode_->CreateComponent<Camera>();
    camera_->SetTemporary(true);
    camera_->SetFov(90.0f);
    camera_->SetNearClip(0.0001f);
    camera_->SetAspectRatio(1.0f);

    RenderPath* renderPath = sceneEditor_->GetSceneView3D()->GetViewport()->GetRenderPath();
    viewport_ = new Viewport(context_, GetScene(), camera_, renderPath);

    renderImage_ = new Texture2D(context_);
    renderImage_->SetSize(imageSize_, imageSize_, Graphics::GetRGBAFormat(), TEXTURE_RENDERTARGET);

    renderSurface_ = renderImage_->GetRenderSurface();
    renderSurface_->SetViewport(0, viewport_);
    renderSurface_->SetUpdateMode(SURFACE_UPDATEALWAYS);

    return true;

}

void CubemapGenerator::SaveCubemapXML()
{
    SharedPtr<XMLFile> xmlFile(new XMLFile(context_));
    XMLElement rootElem = xmlFile->CreateRoot("cubemap");

    String prefix = resourcePath_ + namePrefix_ + "_";

    String name = prefix + GetFaceName(FACE_POSITIVE_X) + ".png";
    rootElem.CreateChild("face").SetAttribute("name", name);
    name = prefix + GetFaceName(FACE_NEGATIVE_X) + ".png";
    rootElem.CreateChild("face").SetAttribute("name", name);
    name = prefix + GetFaceName(FACE_POSITIVE_Y) + ".png";
    rootElem.CreateChild("face").SetAttribute("name", name);
    name = prefix + GetFaceName(FACE_NEGATIVE_Y) + ".png";
    rootElem.CreateChild("face").SetAttribute("name", name);
    name = prefix + GetFaceName(FACE_POSITIVE_Z) + ".png";
    rootElem.CreateChild("face").SetAttribute("name", name);
    name = prefix + GetFaceName(FACE_NEGATIVE_Z) + ".png";
    rootElem.CreateChild("face").SetAttribute("name", name);

    String xmlPath = outputPathAbsolute_ + namePrefix_ + ".xml";

    SharedPtr<File> file(new File(context_, xmlPath, FILE_WRITE));
    xmlFile->Save(*file, "    ");
    file->Close();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    TextureCube* texcube = cache->GetResource<TextureCube>(resourcePath_ + namePrefix_ + ".xml");
    if (texcube)
        cache->ReloadResource(texcube);

}

void CubemapGenerator::EndRender()
{
    UnsubscribeFromEvent(E_BEGINFRAME);
    UnsubscribeFromEvent(E_ENDFRAME);

    SaveCubemapXML();

    cameraNode_->Remove();

    cameraNode_ = 0;
    camera_ = 0;
    viewport_ = 0;
    renderSurface_ = 0;

    // release renderImage_ after renderSurface_, as it doesn't hold a ref
    renderImage_ = 0;
    updateCycle_ = 0;

    GetScene()->SendEvent(E_CUBEMAPRENDEREND);

}

void CubemapGenerator::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
{
    updateCycle_++;
    if (updateCycle_ < 7)
        cameraNode_->SetWorldRotation(RotationOf(GetFaceForCycle(updateCycle_)));
    else
    {
        EndRender();
    }
}

void CubemapGenerator::HandleEndFrame(StringHash eventType, VariantMap& eventData)
{
    SharedPtr<Image> image(GetImage(renderImage_));

    String path = outputPathAbsolute_;

    if (namePrefix_.Length())
        path += namePrefix_;

    path.AppendWithFormat("_%s.png", GetFaceName(GetFaceForCycle(updateCycle_)).CString());

    image->SavePNG(path);

}

SharedPtr<Image> CubemapGenerator::GetImage(Texture2D* tex2d)
{
    Image* rawImage = new Image(tex2d->GetContext());

    const unsigned format = tex2d->GetFormat();

    if (format == Graphics::GetRGBAFormat() || format == Graphics::GetRGBA16Format() || format == Graphics::GetRGBAFloat32Format())
        rawImage->SetSize(tex2d->GetWidth(), tex2d->GetHeight(), 4);
    else if (format == Graphics::GetRGBFormat())
        rawImage->SetSize(tex2d->GetWidth(), tex2d->GetHeight(), 3);
    else
        return SharedPtr<Image>();

    tex2d->GetData(0, rawImage->GetData());

    return SharedPtr<Image>(rawImage);
}


CubeMapFace CubemapGenerator::GetFaceForCycle(int cycle)
{
    switch (cycle)
    {
    case 1:
        return FACE_POSITIVE_X;
    case 2:
        return FACE_POSITIVE_Y;
    case 3:
        return FACE_POSITIVE_Z;
    case 4:
        return FACE_NEGATIVE_X;
    case 5:
        return FACE_NEGATIVE_Y;
    case 6:
        return FACE_NEGATIVE_Z;
    }
    return FACE_POSITIVE_X;

}

String CubemapGenerator::GetFaceName(CubeMapFace face)
{
    switch (face)
    {
    case FACE_POSITIVE_X:
        return "PosX";
    case FACE_POSITIVE_Y:
        return "PosY";
    case FACE_POSITIVE_Z:
        return "PosZ";
    case FACE_NEGATIVE_X:
        return "NegX";
    case FACE_NEGATIVE_Y:
        return "NegY";
    case FACE_NEGATIVE_Z:
        return "NegZ";
    default:
        break;

    }
    return "PosX";
}

Quaternion CubemapGenerator::RotationOf(CubeMapFace face)
{
    Quaternion result;
    switch (face)
    {
    //  Rotate camera according to probe rotation
    case FACE_POSITIVE_X:
        result = Quaternion(0, 90, 0);
        break;
    case FACE_NEGATIVE_X:
        result = Quaternion(0, -90, 0);
        break;
    case FACE_POSITIVE_Y:
        result = Quaternion(-90, 0, 0);
        break;
    case FACE_NEGATIVE_Y:
        result = Quaternion(90, 0, 0);
        break;
    case FACE_POSITIVE_Z:
        result = Quaternion(0, 0, 0);
        break;
    case FACE_NEGATIVE_Z:
        result = Quaternion(0, 180, 0);
        break;
    default:
        break;
    }
    return result;
}

void CubemapGenerator::RegisterObject(Context* context)
{
    context->RegisterFactory<CubemapGenerator>();

    ATOMIC_ATTRIBUTE("Name Prefix", String, namePrefix_, "Cubemap", AM_DEFAULT);
    ATOMIC_ATTRIBUTE("Image Size", int, imageSize_, 512, AM_DEFAULT);

}


}
