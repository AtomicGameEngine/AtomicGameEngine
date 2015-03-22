
#include <Atomic/Core/Context.h>

#include "Platform/PlatformWeb.h"
#include "Net/CurlManager.h"
#include "License/LicenseSystem.h"
#include "ToolSystem.h"


namespace ToolCore
{

ToolSystem::ToolSystem(Context* context) : Object(context)
{

    context_->RegisterSubsystem(new CurlManager(context_));
    context_->RegisterSubsystem(new LicenseSystem(context_));


    // platform registration
    RegisterPlatform(new PlatformWeb(context));
}

ToolSystem::~ToolSystem()
{

}

void ToolSystem::SetCurrentPlatform(PlatformID platform)
{
    if (!platforms_.Contains((unsigned) platform))
        return;

    currentPlatform_ = platforms_[(unsigned)platform];

}

PlatformID ToolSystem::GetCurrentPlatform()
{
    if (currentPlatform_.Null())
        return PLATFORMID_UNDEFINED;

    return currentPlatform_->GetPlatformID();
}

void ToolSystem::RegisterPlatform(Platform* platform)
{
    platforms_.Insert(MakePair((unsigned)platform->GetPlatformID(), SharedPtr<Platform>(platform)));
}

}
