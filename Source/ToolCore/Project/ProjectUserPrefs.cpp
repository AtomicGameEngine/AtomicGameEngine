//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include "ProjectEvents.h"
#include "ProjectUserPrefs.h"

#include <Atomic/IO/File.h>
#include <Atomic/Resource/JSONFile.h>

namespace ToolCore
{

ProjectUserPrefs::ProjectUserPrefs(Context* context) : Object(context),
  snapTranslationX_(1.0f),
  snapTranslationY_(1.0f),
  snapTranslationZ_(1.0f),
  snapRotation_(15.0f),
  snapScale_(0.1f)

{
#ifdef ATOMIC_PLATFORM_OSX
    defaultPlatform_ = PLATFORMID_MAC;
#else
    defaultPlatform_ = PLATFORMID_WINDOWS;
#endif
}

ProjectUserPrefs::~ProjectUserPrefs()
{

}

float ProjectUserPrefs::GetSnapTranslationX() const
{
    return snapTranslationX_;
}

float ProjectUserPrefs::GetSnapTranslationY() const
{
    return snapTranslationY_;
}

float ProjectUserPrefs::GetSnapTranslationZ() const
{
    return snapTranslationZ_;
}

float ProjectUserPrefs::GetSnapRotation() const
{
    return snapRotation_;
}

float ProjectUserPrefs::GetSnapScale() const
{
    return snapScale_;
}

void ProjectUserPrefs::SetSnapTranslationX(float value)
{
    snapTranslationX_ = value;
}

void ProjectUserPrefs::SetSnapTranslationY(float value)
{
    snapTranslationY_ = value;
}

void ProjectUserPrefs::SetSnapTranslationZ(float value)
{
    snapTranslationZ_ = value;
}

void ProjectUserPrefs::SetSnapRotation(float value)
{
    snapRotation_ = value;
}

void ProjectUserPrefs::SetSnapScale(float value)
{
    snapScale_ = value;
}

bool ProjectUserPrefs::Load(const String& path)
{
    SharedPtr<File> file(new File(context_, path));
    if (!file->IsOpen())
        return false;

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));
    bool result = jsonFile->Load(*file);
    file->Close();

    if (!result)
        return false;

    JSONValue& root = jsonFile->GetRoot();
    if (!root.IsObject())
        return false;

    lastBuildPath_ = root.Get("lastBuildPath").GetString();

    if (root.Contains("snapTransX"))
        SetSnapTranslationX(root.Get("snapTransX").GetFloat());
    if (root.Contains("snapTransY"))
        SetSnapTranslationY(root.Get("snapTransY").GetFloat());
    if (root.Contains("snapTransZ"))
        SetSnapTranslationZ(root.Get("snapTransZ").GetFloat());

    if (root.Contains("snapRotation"))
        SetSnapRotation(root.Get("snapRotation").GetFloat());

    if (root.Contains("snapScale"))
        SetSnapScale(root.Get("snapScale").GetFloat());

    return true;
}

void ProjectUserPrefs::Save(const String& path)
{

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue& root = jsonFile->GetRoot();

    SharedPtr<File> file(new File(context_, path, FILE_WRITE));

    root.Set("lastBuildPath", lastBuildPath_);

    // Snap settings
    root.Set("snapTransX", snapTranslationX_);
    root.Set("snapTransY", snapTranslationY_);
    root.Set("snapTransZ", snapTranslationZ_);
    root.Set("snapRotation", snapRotation_);
    root.Set("snapScale", snapScale_);

    jsonFile->Save(*file, String("   "));

    file->Close();

    VariantMap evData;
    evData[ProjectUserPrefSaved::P_PREFS] = this;
    SendEvent(E_PROJECTUSERPREFSAVED);


}


}
