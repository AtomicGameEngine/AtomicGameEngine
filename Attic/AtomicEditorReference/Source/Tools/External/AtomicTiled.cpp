// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/IO/Log.h>

#include "AEEvents.h"
#include "AEExternalTooling.h"

#include "AtomicTiled.h"

namespace AtomicEditor
{

AtomicTiled::AtomicTiled(Context* context) :
    ExternalTool(context)
{
}

AtomicTiled::~AtomicTiled()
{

}

void AtomicTiled::Open(const String& fullpath)
{

}

void AtomicTiled::HandleEvent(StringHash eventType, VariantMap& eventData)
{
    if (eventType == E_SUBPROCESSOUTPUT)
    {
        String output = eventData[SubprocessOutput::P_TEXT].GetString();
        LOGINFOF("TILED: %s", output.CString());

    }
    else if (eventType == E_SUBPROCESSCOMPLETE)
    {
        ExternalTooling* tooling = GetSubsystem<ExternalTooling>();
        tooling->ToolExited("AtomicTiled");
    }
}

bool AtomicTiled::Launch(const String& args)
{
    ExternalTooling* tooling = GetSubsystem<ExternalTooling>();
    String app = tooling->GetToolApplicationPath();

 #ifdef ATOMIC_PLATFORM_WINDOWS
    app += "AtomicTiled/tiled.exe";
 #else
    app += "Tiled.app/Contents/MacOS/Tiled";

 #endif

    LaunchInternal(app, args);

    if (subprocess_)
    {
        SubscribeToEvent(subprocess_, E_SUBPROCESSCOMPLETE, HANDLER(AtomicTiled, HandleEvent));
        SubscribeToEvent(subprocess_, E_SUBPROCESSOUTPUT, HANDLER(AtomicTiled, HandleEvent));
        return true;
    }

    return false;

}

}
