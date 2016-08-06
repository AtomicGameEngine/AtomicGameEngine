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

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"

#include "PlatformAddCmd.h"

#include <Poco/File.h>

namespace ToolCore
{

PlatformAddCmd::PlatformAddCmd(Context* context) : Command(context)
{

}

PlatformAddCmd::~PlatformAddCmd()
{

}

bool PlatformAddCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    String value = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

    if (argument != "platform-add")
    {
        errorMsg = "Unable to parse build command";
        return false;
    }

    if (!value.Length())
    {
        errorMsg = "Unable to parse platform";
        return false;
    }

    platformToAdd_ = value.ToLower();

    return true;
}

void PlatformAddCmd::Run()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    Platform* platform = tsystem->GetPlatformByName(platformToAdd_);

    if (!platform)
    {
        Error(ToString("Unknown platform: %s", platformToAdd_.CString()));
        return;
    }

    if (project->ContainsPlatform(platform->GetPlatformID()))
    {
        Error(ToString("Project already contains platform: %s", platformToAdd_.CString()));
        return;
    }

    ATOMIC_LOGINFOF("Adding platform: %s", platformToAdd_.CString());

    project->AddPlatform(platform->GetPlatformID());

    Finished();
}

}
