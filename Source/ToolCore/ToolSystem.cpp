//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include "Platform/PlatformWeb.h"
#include "Platform/PlatformMac.h"
#include "Platform/PlatformWindows.h"
#include "Platform/PlatformAndroid.h"
#include "Platform/PlatformIOS.h"

#include "Assets/AssetDatabase.h"
#include "Net/CurlManager.h"
#include "License/LicenseSystem.h"
#include "Build/BuildSystem.h"
#include "Subprocess/SubprocessSystem.h"

#include "ToolSystem.h"
#include "ToolEnvironment.h"
#include "ToolEvents.h"

#include "Project/Project.h"
#include "Project/ProjectUserPrefs.h"

#ifdef ATOMIC_DOTNET
#include "NETTools/NETToolSystem.h"
#endif

namespace ToolCore
{

ToolSystem::ToolSystem(Context* context) : Object(context),
    cli_(false)
{
    context_->RegisterSubsystem(new AssetDatabase(context_));
    context_->RegisterSubsystem(new CurlManager(context_));
    context_->RegisterSubsystem(new LicenseSystem(context_));
    context_->RegisterSubsystem(new BuildSystem(context_));
    context_->RegisterSubsystem(new SubprocessSystem(context_));

#ifdef ATOMIC_DOTNET
    context_->RegisterSubsystem(new NETToolSystem(context_));
#endif

    // platform registration
    RegisterPlatform(new PlatformMac(context));
    RegisterPlatform(new PlatformWeb(context));
    RegisterPlatform(new PlatformWindows(context));
    RegisterPlatform(new PlatformIOS(context));
    RegisterPlatform(new PlatformAndroid(context));
}

ToolSystem::~ToolSystem()
{

}

bool ToolSystem::LoadProject(const String& fullpath)
{

    String pathName, fileName, ext;

    SplitPath(fullpath, pathName, fileName, ext);

    String path;

    if (ext == ".atomic")
    {
        path = RemoveTrailingSlash(GetPath(AddTrailingSlash(pathName)));
    }
    else
    {
        path = RemoveTrailingSlash(GetPath(AddTrailingSlash(fullpath)));
    }

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // Do NOT add the root project path as a resource path, otherwise
    // it will introduce a situation where there are nested resource paths
    // which will confuse the ResourceCache and can cause resources referenced
    // by various paths to be seen as unique resources
    //cache->AddResourceDir(path, 0);

    String resourcePath = path;
    resourcePath += "/Resources";
    cache->AddResourceDir(resourcePath, 0);

    project_ = new Project(context_);
    project_->SetResourcePath(resourcePath);

    bool result = project_->Load(fullpath);

    if (result)
    {
        // TODO: persistent platform setting
        SetCurrentPlatform(project_->GetUserPrefs()->GetDefaultPlatform());
    }

    return result;
}

void ToolSystem::CloseProject()
{
    if (project_.Null())
        return;

    SendEvent(E_PROJECTUNLOADED);

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    String projectPath = project_->GetProjectPath();
    String resourcePath = project_->GetResourcePath();

    project_ = 0;

    cache->RemoveResourceDir(resourcePath);
    cache->RemoveResourceDir(projectPath);

    cache->ReleaseAllResources(true);

}

void ToolSystem::SetCurrentPlatform(PlatformID platform)
{
    VariantMap eventData;

    if (platform == PLATFORMID_UNDEFINED)
    {
        currentPlatform_ = NULL;
        eventData[PlatformChanged::P_PLATFORM] = (Platform*) 0;
        SendEvent(E_PLATFORMCHANGED, eventData);
        return;
    }

    if (!platforms_.Contains((unsigned) platform))
        return;

    currentPlatform_ = platforms_[(unsigned)platform];
    eventData[PlatformChanged::P_PLATFORM] = currentPlatform_;
    SendEvent(E_PLATFORMCHANGED, eventData);

}

Platform* ToolSystem::GetPlatformByID(PlatformID platform)
{
    if (!platforms_.Contains((unsigned) platform))
        return NULL;

    return platforms_[(unsigned)platform];

}

Platform* ToolSystem::GetPlatformByName(const String& name)
{
    HashMap<unsigned, SharedPtr<Platform> >::Iterator itr = platforms_.Begin();
    while (itr != platforms_.End())
    {
        if ((*itr).second_->GetName().ToLower()== name.ToLower())
            return (*itr).second_;
        itr++;
    }

    return NULL;
}

Platform *ToolSystem::GetCurrentPlatform()
{
    return currentPlatform_;
}

void ToolSystem::RegisterPlatform(Platform* platform)
{
    platforms_.Insert(MakePair((unsigned)platform->GetPlatformID(), SharedPtr<Platform>(platform)));
}

}
