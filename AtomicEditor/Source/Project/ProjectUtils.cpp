// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#include "AtomicEditor.h"

#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Graphics/Graphics.h>
#include "ProjectUtils.h"
#include "nfd.h"

#include "../AEEditor.h"

namespace AtomicEditor
{

ProjectUtils::ProjectUtils(Context* context) :
    Object(context)
{

}

void ProjectUtils::OpenProjectFileDialog()
{
    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_OpenDialog( "atomic",
                                NULL,
                                &outPath);

    if (outPath && result == NFD_OKAY)
    {
        String fullpath = outPath;
        Editor* editor = GetSubsystem<Editor>();
        editor->LoadProject(fullpath);
    }

    GetSubsystem<Graphics>()->RaiseWindow();

    if (outPath)
        free(outPath);

}

String ProjectUtils::GetBuildPath(const String& defaultPath)
{
    String buildPath;

    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_ChooseDirectory( "Please choose the build folder",
                                defaultPath.Length() ? defaultPath.CString() : NULL,
                                &outPath);

    if (outPath && result == NFD_OKAY)
    {
        buildPath = outPath;
    }

    if (outPath)
        free(outPath);

    GetSubsystem<Graphics>()->RaiseWindow();

    return GetInternalPath(buildPath);

}

String ProjectUtils::GetAndroidSDKPath(const String& defaultPath)
{
    String sdkPath;

    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_ChooseDirectory( "Please choose the root folder of your Android SDK",
                                defaultPath.Length() ? defaultPath.CString() : NULL,
                                &outPath);

    if (outPath && result == NFD_OKAY)
    {
        sdkPath = outPath;
    }

    if (outPath)
        free(outPath);

    GetSubsystem<Graphics>()->RaiseWindow();

    return GetInternalPath(sdkPath);

}


String ProjectUtils::NewProjectFileDialog()
{
    String projectPath;

    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_ChooseDirectory( "Please create a new folder for the project",
                                NULL,
                                &outPath);

    if (outPath && result == NFD_OKAY)
    {
        projectPath = outPath;
    }

    if (outPath)
        free(outPath);

    GetSubsystem<Graphics>()->RaiseWindow();

    return GetInternalPath(projectPath);

}

void ProjectUtils::RevealInFinder(const String& fullpath)
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    if (fs->DirExists(fullpath))
        fs->SystemOpen(fullpath);
    else if (fs->FileExists(fullpath))
        fs->SystemOpen(GetPath(fullpath));
}

ProjectUtils::~ProjectUtils()
{
}

}
