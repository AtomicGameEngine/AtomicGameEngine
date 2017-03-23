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
#include <Atomic/IO/FileSystem.h>

#include "../ToolSystem.h"
#include "../Project/ProjectFile.h"
#include "../Project/ProjectEvents.h"

#include "ProjectCmd.h"

#include <Poco/File.h>

#define  PROJECTCMD_CACHE_CLEAN 0x1 // Cache command first cleans current cache

namespace ToolCore
{

ProjectCmd::ProjectCmd(Context* context) : 
    Command(context),
    requiresProjectLoad_(false),
    options_(0)
{

}

ProjectCmd::~ProjectCmd()
{

}

// usage: project <projectPath> [cache (-clean)]
bool ProjectCmd::ParseInternal(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    if (argument != "project")
    {
        errorMsg = "Unable to parse project command";
        return false;
    }

    command_ = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;
    if (command_ == "cache")
    {
        requiresProjectLoad_ = true;
        SubscribeToEvent(E_PROJECTLOADED, ATOMIC_HANDLER(ProjectCmd, HandleProjectLoaded));

        for (unsigned i = startIndex + 3; i < arguments.Size(); i++)
        {
            if (arguments[i].Length() > 1 && arguments[i][0] == '-')
            {
                String argument = arguments[i].Substring(1).ToLower();
                String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

                if (argument == "clean")
                {
                    options_ = options_ | PROJECTCMD_CACHE_CLEAN;
                    SubscribeToEvent(E_PROJECTBEGINLOAD, ATOMIC_HANDLER(ProjectCmd, HandleProjectBeginLoad));
                    i++;
                }
            }
        }
    }
    else
    {
        errorMsg = "Unknown project command";
        return false;
    }

    projectPath_ = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

    return true;
}

void ProjectCmd::HandleProjectBeginLoad(StringHash eventType, VariantMap& eventData)
{
    if (options_ & PROJECTCMD_CACHE_CLEAN)
    {
        String cachePath = GetPath(eventData[ProjectBeginLoad::P_PROJECTPATH].GetString());
        cachePath = AddTrailingSlash(cachePath) + "Cache";

        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        if (fileSystem->DirExists(cachePath) &&
            !fileSystem->RemoveDir(cachePath, true))
            Error("Cache clean failed");
    }
}

void ProjectCmd::HandleProjectLoaded(StringHash eventType, VariantMap& eventData)
{
    bool result = eventData[ProjectLoaded::P_RESULT].GetBool();

    if (!eventData[ProjectLoaded::P_RESULT].GetBool())
        Error("Project load failed");
    else
        Finished();
}

void ProjectCmd::Run()
{
    if (command_ == "cache")
    {
        // Nothing to do here
    }
    else
    {
        Finished();
    }

}

}
