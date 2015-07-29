
#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include "Platform/PlatformWeb.h"
#include "Platform/PlatformMac.h"
#include "Platform/PlatformWindows.h"
#include "Assets/AssetDatabase.h"
#include "Net/CurlManager.h"
#include "License/LicenseSystem.h"
#include "Build/BuildSystem.h"


#include "ToolSystem.h"
#include "ToolEnvironment.h"
#include "ToolEvents.h"

#include "Project/Project.h"


namespace ToolCore
{

ToolSystem::ToolSystem(Context* context) : Object(context),
    cli_(false)
{
    context_->RegisterSubsystem(new AssetDatabase(context_));
    context_->RegisterSubsystem(new CurlManager(context_));
    context_->RegisterSubsystem(new LicenseSystem(context_));
    context_->RegisterSubsystem(new BuildSystem(context_));

    // platform registration
    RegisterPlatform(new PlatformMac(context));
    RegisterPlatform(new PlatformWeb(context));
    RegisterPlatform(new PlatformWindows(context));
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
    cache->AddResourceDir(path, 0);

    String resourcePath = path;
    resourcePath += "/Resources";
    cache->AddResourceDir(resourcePath, 0);

    project_ = new Project(context_);
    project_->SetResourcePath(resourcePath);

    return project_->Load(fullpath);
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
    if (platform == PLATFORMID_UNDEFINED)
    {
        currentPlatform_ = NULL;
        return;
    }

    if (!platforms_.Contains((unsigned) platform))
        return;

    currentPlatform_ = platforms_[(unsigned)platform];

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
