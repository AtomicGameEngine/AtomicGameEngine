//
// Copyright (c) 2014-2017 THUNDERBEAST GAMES LLC
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

#include "../Assets/AssetDatabase.h"
#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"
#include "../Build/BuildEvents.h"
#include "../Build/BuildSystem.h"

#include "CacheCmd.h"

namespace ToolCore
{

CacheCmd::CacheCmd(Context* context) : Command(context),
    cleanCache_(false),
    generateCache_(false)
{

}

CacheCmd::~CacheCmd()
{

}

bool CacheCmd::ParseInternal(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    for (unsigned i = startIndex; i < arguments.Size(); i++)
    {
        if (arguments[i].Length() > 1)
        {
            String argument = arguments[i].ToLower();

            if (argument == "generate")
            {
                generateCache_ = true;
                continue;
            }

            // process any argument/value pairs
            if (arguments[i][0] != '-')
                continue;

            // eat additonal argument '-'
            while (argument.StartsWith("-"))
            {
                argument.Erase(0);
            }

            String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

            if (argument == "clean")
                cleanCache_ = true;

        }
    }

    return true;
}


bool CacheCmd::CreateCacheDirectory() const
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (fileSystem->DirExists(cachePath_))
        return true;

    fileSystem->CreateDir(cachePath_);

    if (!fileSystem->DirExists(cachePath_))
    {
        ATOMIC_LOGERRORF("Unable to create cache directory %s", cachePath_.CString());
        return false;
    }

    return true;
}

bool CacheCmd::RemoveCacheDirectory() const
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (!fileSystem->DirExists(cachePath_))
        return true;

    fileSystem->RemoveDir(cachePath_, true);

    if (fileSystem->DirExists(cachePath_))
    {
        ATOMIC_LOGERRORF("Unable to remove cache directory %s", cachePath_.CString());
        return false;
    }

    return true;
}

bool CacheCmd::CleanCache() const
{
    ATOMIC_LOGINFO("Cleaning Cache...");

    if (!RemoveCacheDirectory())
        return false;

    if (!CreateCacheDirectory())
        return false;

   return true;
}

bool CacheCmd::GenerateCache() const
{
    ATOMIC_LOGINFO("Generating Cache... hold on");

    AssetDatabase* database = GetSubsystem<AssetDatabase>();
    database->ReimportAllAssets();

    ATOMIC_LOGINFO("Cache Generated");

    return true;
}

void CacheCmd::Run()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    ToolEnvironment* env = GetSubsystem<ToolCore::ToolEnvironment>();
    Project* project = tsystem->GetProject();

    cachePath_ = AddTrailingSlash(project->GetProjectPath()) + "Cache";

    if (cleanCache_)
    {
        if (!CleanCache())
        {
            Error("Unable to clean cache");
            return;
        }
    }

    if (generateCache_)
    {
        if (!GenerateCache())
        {
            Error("Unable to generate cache");
            return;
        }
    }

    Finished();
}

}
