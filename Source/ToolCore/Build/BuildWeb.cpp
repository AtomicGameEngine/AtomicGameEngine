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
#include <stdlib.h>

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/ResourceCache.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"
#include "../Project/ProjectBuildSettings.h"
#include "../Assets/AssetDatabase.h"

#include "BuildEvents.h"
#include "BuildSystem.h"
#include "BuildWeb.h"

namespace ToolCore
{

BuildWeb::BuildWeb(Context* context, Project* project) : BuildBase(context, project, PLATFORMID_WEB)
{

}

BuildWeb::~BuildWeb()
{

}

void BuildWeb::Initialize()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    Project* project = tsystem->GetProject();

    Vector<String> defaultResourcePaths;
    GetDefaultResourcePaths(defaultResourcePaths);

    for (unsigned i = 0; i < defaultResourcePaths.Size(); i++)
    {
        AddResourceDir(defaultResourcePaths[i]);
    }
    BuildDefaultResourceEntries();

    // TODO: smart filtering of cache
    String projectResources = project->GetResourcePath();
    AddProjectResourceDir(projectResources);
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    String cachePath = db->GetCachePath();
    AddProjectResourceDir(cachePath);

    BuildProjectResourceEntries();
}

void BuildWeb::Build(const String& buildPath)
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    Project* project = toolSystem->GetProject();
    WebBuildSettings* settings = project->GetBuildSettings()->GetWebBuildSettings();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Starting Web Deployment</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    Initialize();

    if (!BuildClean(buildPath_))
        return;

    String dataPath = tenv->GetToolDataDir();

    String buildSourceDir  = dataPath + "Deployment/Web";

    if (!BuildCreateDirectory(buildPath_))
        return;

    String resourcePackagePath = buildPath_ + "/AtomicResources.data";
    GenerateResourcePackage(resourcePackagePath);

    if (!BuildCopyFile(buildSourceDir + "/AtomicPlayer.html", buildPath_ + "/AtomicPlayer.html"))
        return;
    if (!BuildCopyFile(buildSourceDir + "/AtomicPlayer.html.mem", buildPath_ + "/AtomicPlayer.html.mem"))
        return;
    if (!BuildCopyFile(buildSourceDir + "/AtomicPlayer.js", buildPath_ + "/AtomicPlayer.js"))
        return;
    if (!BuildCopyFile(buildSourceDir + "/AtomicLoader.js", buildPath_ + "/AtomicLoader.js"))
        return;
        
    // look to see if they specified an ico file, otherwise they get atomic     
    String myIcon = settings->GetFaviconName();
    if (myIcon.Empty())
        myIcon = buildSourceDir + "/fav_atomic.ico";
        
    if (!BuildCopyFile( myIcon, buildPath_ + "/favicon.ico"))
        return;

    // get the title
    String myTitle = settings->GetAppName();
   // get the (text) size
    String myWidth = settings->GetGameWidth();    
    String myHeight = settings->GetGameHeight();     

    if ( settings->GetPageTheme() == 0) // editor dark theme
    {
        if (!ReadReplace ( buildSourceDir + "/index-dark.html", buildPath_ + "/index.html", myTitle, myWidth, myHeight))
            return;
         if (!BuildCopyFile(buildSourceDir + "/Atomic_Logo_Header-i.png", buildPath_ + "/Atomic_Logo_Header-i.png"))
            return;        
    }
    else if ( settings->GetPageTheme() == 1) // editor light theme
    {
        if (!ReadReplace(buildSourceDir + "/index-light.html", buildPath_ + "/index.html", myTitle, myWidth, myHeight))
            return;
        if (!BuildCopyFile(buildSourceDir + "/Atomic_Logo_Header-w.png", buildPath_ + "/Atomic_Logo_Header-w.png"))
            return;        
    }
    else if ( settings->GetPageTheme() == 2) // Atomic gradient theme
    {
        if (!ReadReplace(buildSourceDir + "/index-image.html", buildPath_ + "/index.html", myTitle, myWidth, myHeight))
            return;
        if (!BuildCopyFile(buildSourceDir + "/atomic-gradient.jpg", buildPath_ + "/background.jpg"))
            return;
        if (!BuildCopyFile(buildSourceDir + "/Atomic_Logo_Header-i.png", buildPath_ + "/Atomic_Logo_Header-i.png"))
            return;        
    }
    else if ( settings->GetPageTheme() == 3) // NY Lake theme
    {
         if (!ReadReplace(buildSourceDir + "/index-image.html", buildPath_ + "/index.html", myTitle, myWidth, myHeight))
            return;
        if (!BuildCopyFile(buildSourceDir + "/ny-lake.jpg", buildPath_ + "/background.jpg"))
            return;
        if (!BuildCopyFile(buildSourceDir + "/Atomic_Logo_Header-i.png", buildPath_ + "/Atomic_Logo_Header-i.png"))
            return;        
    }
    else if ( settings->GetPageTheme() == 4) // Fireworks theme
    {
        if (!ReadReplace(buildSourceDir + "/index-image.html", buildPath_ + "/index.html", myTitle, myWidth, myHeight))
            return;
        if (!BuildCopyFile(buildSourceDir + "/fireworks.jpg", buildPath_ + "/background.jpg"))
            return;
        if (!BuildCopyFile(buildSourceDir + "/Atomic_Logo_Header-i.png", buildPath_ + "/Atomic_Logo_Header-i.png"))
            return;        
    }
    else // theme 5, the retro (original/old) theme
    {
        if (!BuildCopyFile(buildSourceDir + "/index.html", buildPath_ + "/index.html"))
            return;
        if (!BuildCopyFile(buildSourceDir + "/Atomic_Logo_Header.png", buildPath_ + "/Atomic_Logo_Header.png"))
            return;
    }
    
    File file(context_, buildSourceDir + "/AtomicResources_js.template", FILE_READ);
    unsigned size = file.GetSize();

    SharedArrayPtr<char> buffer(new char[size + 1]);
    file.Read(buffer.Get(), size);
    buffer[size] = '\0';

    String resourcejs = (const char*) buffer.Get();

    file.Close();

    file.Open(buildPath_ + "/AtomicResources.data", FILE_READ);
    int rsize = (int) file.GetSize();
    file.Close();

    String request;
    request.AppendWithFormat("new DataRequest(0, %i, 0, 0).open('GET', '/AtomicResources%s');", rsize, PAK_EXTENSION);

    resourcejs.Replace("$$REMOTE_PACKAGE_SIZE$$", ToString("%i", rsize));
    resourcejs.Replace("$$ATOMIC_RESOURCES_DATA_REQUEST$$", request);

    file.Open(buildPath_ + "/AtomicResources.js", FILE_WRITE);
    file.Write(resourcejs.CString(), resourcejs.Length());
    file.Close();

    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Web Deployment Complete</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);


    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    buildSystem->BuildComplete(PLATFORMID_WEB, buildPath_);

    //fileSystem->SystemCommandAsync("/Applications/Firefox.app/Contents/MacOS/firefox");

}

bool BuildWeb::ReadReplace ( String infile, String outfile, String title, String width, String height )
{
    File file(context_, infile, FILE_READ);
    if (file.IsOpen())
    {
        unsigned size = file.GetSize();
        SharedArrayPtr<char> buffer(new char[size + 1]);
        file.Read(buffer.Get(), size);
        buffer[size] = '\0';
        String indexstr = (const char*) buffer.Get();
        file.Close();

        if (!title.Empty())
            indexstr.Replace("Atomic Game Engine Web Player", title);
        if (!width.Empty())
        {
            // see if the string resolves to a positive integer value
            int num = atoi(width.CString());
            if (num > 0 &&  num < 16384)
                indexstr.Replace("width=\"800\"", "width=\""+ width + "\"" );
        }
        if (!height.Empty())
        {
            int num = atoi(height.CString());
            if (num > 0 &&  num < 16384)
                indexstr.Replace("height=\"512\"", "height=\""+ height + "\"" );
        }

        file.Open(outfile, FILE_WRITE);
        file.Write(indexstr.CString(), indexstr.Length());
        file.Close();
        return true;
    }
    return false;
}


}
