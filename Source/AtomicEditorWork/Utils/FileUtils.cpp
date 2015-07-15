
#include <Poco/File.h>

#include "AtomicEditor.h"

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

void FileUtils::RevealInFinder(const String& fullpath)
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    if (fs->DirExists(fullpath))
        fs->SystemOpen(fullpath);
    else if (fs->FileExists(fullpath))
        fs->SystemOpen(GetPath(fullpath));
}


}
