//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../NETTools/NETProjectGen.h"

#include "NETProjectGenCmd.h"

namespace ToolCore
{

NETProjectGenCmd::NETProjectGenCmd(Context* context) : Command(context)
{

}

NETProjectGenCmd::~NETProjectGenCmd()
{

}

bool NETProjectGenCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();

    projectFile_ = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;
    scriptPlatform_ = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;

    if (argument != "net-projectgen")
    {
        errorMsg = "Unable to parse bind command";
        return false;
    }

    if (!projectFile_.Length())
    {
        errorMsg = "Unable to parse net-projectgen command";
        return false;
    }


    if (!scriptPlatform_.Length())
    {
        errorMsg = "Unable to parse net-projectgen command";
        return false;
    }

    return true;
}

void NETProjectGenCmd::Run()
{
    SharedPtr<NETProjectGen> gen(new NETProjectGen(context_));
    gen->SetScriptPlatform(scriptPlatform_);

    gen->LoadProject(projectFile_);

    gen->Generate();

    Finished();
}

}
