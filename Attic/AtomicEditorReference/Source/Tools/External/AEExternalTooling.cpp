// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include "AEEvents.h"

#include "AEExternalTooling.h"
#include "AtomicTiled.h"

namespace AtomicEditor
{

ExternalTooling::ExternalTooling(Context* context) :
    Object(context)
{
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(ExternalTooling, HandleEditorShutdown));
}

ExternalTooling::~ExternalTooling()
{
}


void ExternalTooling::LaunchOrOpen(const String& name, const String& fullpath)
{

    if (tools_.Contains(name))
    {
        // TODO: open
        return;
    }

    if (name == "AtomicTiled")
    {
        SharedPtr<ExternalTool> tiled(new AtomicTiled(context_));

        if (tiled->Launch(fullpath))
        {
            tools_[name] = tiled;
        }

    }

}

void ExternalTooling::ToolExited(const String& name)
{
    if (!tools_.Contains(name))
        return;

    tools_.Erase(name);

}

String ExternalTooling::GetToolApplicationPath()
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    String appDir = fileSystem->GetProgramDir();

 #ifdef ATOMIC_PLATFORM_WINDOWS
    appDir += "/Applications/";
 #else

    unsigned p = appDir.Find("/Contents/MacOS");

    if (p != String::NPOS)
    {
        appDir.Erase(p, appDir.Length() - p);
        appDir += "/Contents/Applications/";
    }

#endif

    return appDir;

}

void ExternalTooling::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}


}
