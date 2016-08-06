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
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/MemoryBuffer.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Subprocess/SubprocessSystem.h"
#include "../Project/Project.h"
#include "../Project/ProjectBuildSettings.h"
#include "../Platform/PlatformAndroid.h"
#include "../Assets/AssetDatabase.h"

#include "AndroidProjectGenerator.h"

#include "BuildSystem.h"
#include "BuildEvents.h"
#include "BuildAndroid.h"

namespace ToolCore
{

BuildAndroid::BuildAndroid(Context* context, Project* project) : BuildBase(context, project, PLATFORMID_ANDROID)
{
    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    // this cast isn't great
    platformAndroid_ = (PlatformAndroid*) toolSystem->GetPlatformByID(PLATFORMID_ANDROID);

}

BuildAndroid::~BuildAndroid()
{

}

void BuildAndroid::HandleADBStartActivityComplete(StringHash eventType, VariantMap& eventData)
{

}


// adb shell am start -n com.package.name/com.package.name.ActivityName

void BuildAndroid::RunADBStartActivity()
{

    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();
    String adbCommand = platformAndroid_->GetADBCommand();

    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    Project* project = toolSystem->GetProject();
    AndroidBuildSettings* settings = project->GetBuildSettings()->GetAndroidBuildSettings();

    String stringArgs;
    const char* cpackage = settings->GetPackageName().CString();
    stringArgs.AppendWithFormat("shell am start -n %s/%s.AtomicGameEngine",cpackage, cpackage);

    Vector<String> args = stringArgs.Split(' ');

    currentBuildPhase_ = ADBStartActivity;
    Subprocess* subprocess = subs->Launch(adbCommand, args, buildPath_);
    if (!subprocess)
    {
        FailBuild("StartActivity operation did not launch successfully.");
        return;
    }

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Starting Android Activity</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, ATOMIC_HANDLER(BuildAndroid, HandleADBStartActivityComplete));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, ATOMIC_HANDLER(BuildBase, HandleSubprocessOutputEvent));

}


void BuildAndroid::HandleRunADBInstallComplete(StringHash eventType, VariantMap& eventData)
{
    int code = eventData[SubprocessComplete::P_RETCODE].GetInt();

    if (!code)
    {
        RunADBStartActivity();
    }
    else
    {
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
        buildSystem->BuildComplete(PLATFORMID_ANDROID, buildPath_, false);
    }

}

void BuildAndroid::RunADBInstall()
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolPrefs* prefs = tenv->GetToolPrefs();
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();
    String adbCommand = platformAndroid_->GetADBCommand();

    Vector<String> args;

    if ( prefs->GetReleaseCheck() > 0 ) // install release apk
        args = String("install -r ./bin/Atomic-release.apk").Split(' ');
    else
        args = String("install -r ./bin/Atomic-debug-unaligned.apk").Split(' ');

    currentBuildPhase_ = ADBInstall;
    Subprocess* subprocess = subs->Launch(adbCommand, args, buildPath_);
    if (!subprocess)
    {
        FailBuild("APK Device Installation operation did not launch successfully.");
        return;
    }

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Installing on Android Device</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, ATOMIC_HANDLER(BuildAndroid, HandleRunADBInstallComplete));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, ATOMIC_HANDLER(BuildBase, HandleSubprocessOutputEvent));

}

void BuildAndroid::HandleADBListDevicesComplete(StringHash eventType, VariantMap& eventData)
{
    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    int code = eventData[SubprocessComplete::P_RETCODE].GetInt();

    if (!code)
    {
        // check if we have any devices attached, otherwise adb install
        // will hang looking for devices
        bool noDevices = true;
        if (deviceListText_.Length())
        {
            MemoryBuffer reader(deviceListText_.CString(), deviceListText_.Length() + 1);
            while (!reader.IsEof())
            {
                String line = reader.ReadLine();
                if (line.Length() && line[0] >= '0' && line[0] <= '9')
                {
                    noDevices = false;
                    break;
                }
            }
        }

        if (!noDevices)
            RunADBInstall();
        else
        {
            // can't proceed, though success
            buildSystem->BuildComplete(PLATFORMID_ANDROID, buildPath_);
        }

    }
    else
    {
        buildSystem->BuildComplete(PLATFORMID_ANDROID, buildPath_, false);
    }

}

void BuildAndroid::HandleADBListDevicesOutputEvent(StringHash eventType, VariantMap& eventData)
{
    // E_SUBPROCESSOUTPUT
    const String& text = eventData[SubprocessOutput::P_TEXT].GetString();

    deviceListText_ += text;

    // convert to a build output event and forward to subscribers
    VariantMap buildOutputData;
    buildOutputData[BuildOutput::P_TEXT] = text;
    SendEvent(E_BUILDOUTPUT, buildOutputData);

}

void BuildAndroid::RunADBListDevices()
{
    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

    String adbCommand = platformAndroid_->GetADBCommand();

    deviceListText_.Clear();

    Vector<String> args = String("devices").Split(' ');

    currentBuildPhase_ = ADBListDevices;
    Subprocess* subprocess = subs->Launch(adbCommand, args, "");

    if (!subprocess)
    {
        FailBuild("Android List Device operation did not launch successfully.");
        return;
    }

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Listing Android Devices</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, ATOMIC_HANDLER(BuildAndroid, HandleADBListDevicesComplete));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, ATOMIC_HANDLER(BuildAndroid, HandleADBListDevicesOutputEvent));

}

void BuildAndroid::HandleAntDebugComplete(StringHash eventType, VariantMap& eventData)
{
    int code = eventData[SubprocessComplete::P_RETCODE].GetInt();

    if (!code)
    {
        RunADBListDevices();
    }
    else
    {
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
        buildSystem->BuildComplete(PLATFORMID_ANDROID, buildPath_, false);
    }
}

void BuildAndroid::RunAntDebug()
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();
    ToolPrefs* tprefs = tenv->GetToolPrefs();

    Poco::Process::Env env;

    String buildApk = "debug";  // the default

    if ( tprefs->GetReleaseCheck() > 0 ) // create release apk
        buildApk = "release";


#ifdef ATOMIC_PLATFORM_OSX
    String antCommand = tprefs->GetAntPath();
    Vector<String> args;
    args.Push(buildApk);
#endif
#ifdef ATOMIC_PLATFORM_WINDOWS
    // C:\ProgramData\Oracle\Java\javapath;
    Vector<String> args;
    String antCommand = "cmd";
    String antPath = tprefs->GetAntPath() + "/ant.bat";
    env["JAVA_HOME"] = tprefs->GetJDKRootPath().CString();
    // ant is a batch file on windows, so have to run with cmd /c
    args.Push("/c");
    args.Push("\"" + antPath + "\"");
    args.Push(buildApk);
#endif
#ifdef ATOMIC_PLATFORM_LINUX 

    String antCommand = tprefs->GetAntPath();
    if ( antCommand.Empty() ) // user didnt fill it out, use installed one
    {
        antCommand = "/usr/bin/ant"; // system default if installed
    }
    else
    {
        antCommand.Append("/ant"); 
    }
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if ( !fileSystem->FileExists ( antCommand) ) 
    {
        FailBuild("The ant program can not be found, check the Ant Path in Build Settings.");
    }
    Vector<String> args;
    args.Push(buildApk);
#endif

    currentBuildPhase_ = AntBuildDebug;
    Subprocess* subprocess = subs->Launch(antCommand, args, buildPath_, env);

    if (!subprocess)
    {
        FailBuild("The ant build operation did not launch successfully.");
        return;
    }

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "<color #D4FB79>Starting Android " + buildApk + " Deployment</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, ATOMIC_HANDLER(BuildAndroid, HandleAntDebugComplete));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, ATOMIC_HANDLER(BuildBase, HandleSubprocessOutputEvent));

}

void BuildAndroid::Initialize()
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

void BuildAndroid::Build(const String& buildPath)
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    Initialize();
 
    if (!BuildClean(buildPath_))
        return;

    //generate manifest file
    String manifest;
    for (unsigned i = 0; i < resourceEntries_.Size(); i++)
    {
        BuildResourceEntry* entry = resourceEntries_[i];
        manifest += entry->packagePath_;
        if ( i != resourceEntries_.Size() - 1)
            manifest += ";";
    }

    String buildSourceDir = tenv->GetToolDataDir();

    String androidProject = buildSourceDir + "Deployment/Android";

    // Copy the base android project
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if( !BuildCopyDir(androidProject, buildPath_))
        return;

    Vector<String> defaultResourcePaths;
    GetDefaultResourcePaths(defaultResourcePaths);
    String projectResources = project->GetResourcePath();

    for (unsigned i = 0; i < defaultResourcePaths.Size(); i++)
    {
        if ( !BuildCopyDir(defaultResourcePaths[i], buildPath_ + "/assets/" + GetFileName(RemoveTrailingSlash(defaultResourcePaths[i]))))
            return;
    }

    if( !BuildCopyDir(project->GetProjectPath() + "Cache/", buildPath_ + "/assets/Cache"))
        return;
    if( !BuildCopyDir(projectResources, buildPath_ + "/assets/AtomicResources"))
        return;

    // write the manifest
    SharedPtr<File> mfile(new File(context_, buildPath_ + "/assets/AtomicManifest", FILE_WRITE));
    mfile->WriteString(manifest);
    mfile->Close();

    //check for Deployment/Android/libs/armeabi-v7a/libAtomicPlayer.so
    if ( !fileSystem->FileExists(androidProject + "/libs/armeabi-v7a/libAtomicPlayer.so")  )
    {
        FailBuild( "The file libAtomicPlayer.so is not found. This is required for APK generation." );
        return;
    }

    AndroidProjectGenerator gen(context_, this);
    gen.SetBuildPath(buildPath_);

    if (!gen.Generate())
    {
        FailBuild(gen.GetErrorText());
        return;
    }

    RunAntDebug();

}

}
