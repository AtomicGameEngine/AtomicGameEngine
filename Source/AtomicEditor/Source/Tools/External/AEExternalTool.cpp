// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/IO/Log.h>

#include "Subprocess/AESubprocessSystem.h"

#include "AEExternalTool.h"

namespace AtomicEditor
{

ExternalTool::ExternalTool(Context* context) :
    Object(context)
{
}

ExternalTool::~ExternalTool()
{

}

bool ExternalTool::LaunchInternal(const String& applicationPath, const String& args)
{
    SubprocessSystem* system = GetSubsystem<SubprocessSystem>();

    Vector<String> vargs;
    if (args.Length())
        vargs = args.Split(' ');

    subprocess_ = system->Launch(applicationPath, vargs);

    return subprocess_.NotNull();

}


}
