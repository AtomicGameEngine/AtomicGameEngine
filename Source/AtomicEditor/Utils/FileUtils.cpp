//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Poco/File.h>

#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Graphics/Graphics.h>
#include "FileUtils.h"
#include "nfd.h"

namespace AtomicEditor
{

FileUtils::FileUtils(Context* context) :
    Object(context)
{

}

FileUtils::~FileUtils()
{
}

String FileUtils::OpenProjectFileDialog()
{
    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_OpenDialog( "atomic",
                                NULL,
                                &outPath);

    String fullpath;

    if (outPath && result == NFD_OKAY)
    {
        fullpath = outPath;
    }

    GetSubsystem<Graphics>()->RaiseWindow();

    if (outPath)
        free(outPath);

    return fullpath;

}

bool FileUtils::CreateDirs(const String& folder)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    Poco::File dirs(folder.CString());
    dirs.createDirectories();

    return fileSystem->DirExists(folder);

}

String FileUtils::NewProjectFileDialog()
{

    String projectPath;

    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_ChooseDirectory( "Please choose the root folder for your project",
                                NULL,
                                &outPath);


    if (outPath && result == NFD_OKAY)
    {
        projectPath = outPath;
    }

    GetSubsystem<Graphics>()->RaiseWindow();

    if (outPath)
        free(outPath);

    return projectPath;

}

String FileUtils::GetBuildPath(const String& defaultPath)
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

String FileUtils::GetAndroidSDKPath(const String& defaultPath)
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

String FileUtils::GetAntPath(const String& defaultPath)
{
    String antPath;

    nfdchar_t *outPath = NULL;

#ifdef ATOMIC_PLATFORM_WINDOWS
    String msg = "Please select the folder which contains ant.bat";
#else
    String msg = "Please select the folder which contains the ant executable";
#endif

    nfdresult_t result = NFD_ChooseDirectory(msg.CString(),
        defaultPath.Length() ? defaultPath.CString() : NULL,
        &outPath);

    if (outPath && result == NFD_OKAY)
    {
        antPath = outPath;
    }

    if (outPath)
        free(outPath);

    GetSubsystem<Graphics>()->RaiseWindow();

    return GetInternalPath(antPath);
}

String FileUtils::GetJDKRootPath(const String& defaultPath)
{
    String jdkPath;

    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_ChooseDirectory("Please choose the root folder of your JDK",
        defaultPath.Length() ? defaultPath.CString() : NULL,
        &outPath);

    if (outPath && result == NFD_OKAY)
    {
        jdkPath = outPath;
    }

    if (outPath)
        free(outPath);

    GetSubsystem<Graphics>()->RaiseWindow();

    return GetInternalPath(jdkPath);

}

String FileUtils::GetMobileProvisionPath()
{
    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_OpenDialog( "mobileprovision",
                                NULL,
                                &outPath);

    String fullpath;

    if (outPath && result == NFD_OKAY)
    {
         fullpath = outPath;
    }

    GetSubsystem<Graphics>()->RaiseWindow();

    if (outPath)
        free(outPath);

    return fullpath;

}



void FileUtils::RevealInFinder(const String& fullpath)
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    if (fs->DirExists(fullpath))
        fs->SystemOpen(fullpath);
    else if (fs->FileExists(fullpath))
        fs->SystemOpen(GetPath(fullpath));
}


}
