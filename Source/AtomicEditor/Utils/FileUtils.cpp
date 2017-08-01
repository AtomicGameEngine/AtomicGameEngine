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

    String upath;

#ifdef ATOMIC_PLATFORM_LINUX
    upath = GetSubsystem<FileSystem>()->GetUserDocumentsDir();
#endif

    nfdresult_t result = NFD_OpenDialog( "atomic",
                                upath.Length() ? GetNativePath(upath).CString() : "",
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

    String upath;
    
#ifdef ATOMIC_PLATFORM_LINUX
    upath = GetSubsystem<FileSystem>()->GetUserDocumentsDir();
#endif

    nfdresult_t result = NFD_PickFolder(upath.Length() ? GetNativePath(upath).CString() : "", &outPath);

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
    
    nfdresult_t result = NFD_PickFolder(defaultPath.Length() ? GetNativePath(defaultPath).CString() : "", &outPath);

    if (outPath && result == NFD_OKAY)
    {
        buildPath = outPath;
    }

    if (outPath)
        free(outPath);

    GetSubsystem<Graphics>()->RaiseWindow();

    return GetInternalPath(buildPath);

}

String FileUtils::GetAntPath(const String& defaultPath)
{
    String antPath;

    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_PickFolder(defaultPath.Length() ? GetNativePath(defaultPath).CString() : "", &outPath);

    if (outPath && result == NFD_OKAY)
    {
        antPath = outPath;
    }

    if (outPath)
        free(outPath);

    GetSubsystem<Graphics>()->RaiseWindow();

    return GetInternalPath(antPath);
}

String FileUtils::GetMobileProvisionPath()
{
    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_OpenDialog( "mobileprovision",
                                "",
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

String FileUtils::FindPath(const String& title, const String& defaultPath)
{
    String resultPath;
    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_PickFolder(defaultPath.Length() ? GetNativePath(defaultPath).CString() : "", &outPath);

    if (outPath && result == NFD_OKAY)
    {
        resultPath = outPath;
    }

    if (outPath)
        free(outPath);

    GetSubsystem<Graphics>()->RaiseWindow();

    return GetInternalPath(resultPath);
}

String FileUtils::FindFile (const String& filterlist, const String& defaultPath)
{
    String fullpath;
    nfdchar_t *outPath = NULL;

    nfdresult_t result = NFD_OpenDialog( filterlist.CString(),
        defaultPath.Length() ? GetNativePath(defaultPath).CString() : "",
        &outPath);

    if (outPath && result == NFD_OKAY)
    {
        fullpath = outPath;
    }

    GetSubsystem<Graphics>()->RaiseWindow();

    if (outPath)
        free(outPath);

    return fullpath;
}

}
