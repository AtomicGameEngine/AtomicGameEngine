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

#include <Atomic/IO/Log.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"

#include "Command.h"

namespace ToolCore
{

Command::Command(Context* context) : Object(context),
    timeOut_(0.0f)
{

}

Command::~Command()
{

}

bool Command::LoadProject()
{
    if (!projectPath_.Length())
        return false;

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    // default to current directly if command doesn't provide the path
    if (!projectPath_.Length())
        projectPath_ = fileSystem->GetCurrentDir();

    String projectFile;
    if (projectPath_.EndsWith(".atomic", false))
    {
        projectFile = projectPath_;
        projectPath_ = GetPath(projectPath_);
    }
    else
    {
        Vector<String> projectFiles;
        fileSystem->ScanDir(projectFiles, projectPath_, "*.atomic", SCAN_FILES, false);
        if (!projectFiles.Size())
        {
            ATOMIC_LOGERRORF("No .atomic project file in %s", projectPath_.CString());
            return false;
        }
        else if (projectFiles.Size() > 1)
        {
            ATOMIC_LOGERRORF("Multiple .atomic project files found in %s", projectPath_.CString());
            return false;
        }
        projectFile = projectPath_ + "/" + projectFiles[0];
    }

    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    if (!tsystem->LoadProject(projectFile))
    {
        ATOMIC_LOGERRORF("Failed to load project: %s", projectFile.CString());
        return false;
    }

    return true;

}

void Command::ParseCommonArguments(const Vector<String> &arguments, String &errorMsg)
{
    for (unsigned i = 0; i < arguments.Size(); ++i)
    {
        if (arguments[i].Length() > 1 && arguments[i][0] == '-')
        {
            String argument = arguments[i].Substring(1).ToLower();
            String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

            // eat additonal argument '-'
            while (argument.StartsWith("-"))
            {
                argument.Erase(0);
            }

            if (argument == "project" && value.Length())
            {
                projectPath_ = value;
            }
        }
    }

}

bool Command::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    errorMsg = String::EMPTY;

    ParseCommonArguments(arguments, errorMsg);

    if (errorMsg.Length())
    {
        return false;
    }

    return ParseInternal(arguments, startIndex, errorMsg);
}

void Command::Error(const String& errorMsg)
{
    VariantMap eventData;
    eventData[CommandError::P_MESSAGE] = errorMsg;
    SendEvent(E_COMMANDERROR, eventData);
}

void Command::Finished()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    if (project && project->IsDirty())
        project->Save(project->GetProjectFilePath());

    SendEvent(E_COMMANDFINISHED);
}

}
