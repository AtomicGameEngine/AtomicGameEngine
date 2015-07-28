// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/MemoryBuffer.h>
#include "../AEEditor.h"
#include "../Project/AEProject.h"
#include "../Project/ProjectUtils.h"

#include "BuildAndroid.h"
#include "AndroidProjectGenerator.h"

// TODO: We're going to want headless builds
#include "AEEditor.h"
#include "AEEvents.h"
#include "AEPreferences.h"
#include "Subprocess/AESubprocessSystem.h"
#include "UI/Modal/UIModalOps.h"

#include "BuildSystem.h"

namespace AtomicEditor
{

BuildAndroid::BuildAndroid(Context * context) : BuildBase(context)
{

}

BuildAndroid::~BuildAndroid()
{

}

void BuildAndroid::Initialize()
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_OSX
    String bundleResources = fileSystem->GetAppBundleResourceFolder();
#else
    String bundleResources = fileSystem->GetProgramDir();
#endif

    String projectResources = project->GetResourcePath();
    String coreDataFolder = bundleResources + "CoreData/";

    AddResourceDir(coreDataFolder);
    AddResourceDir(projectResources);

    BuildResourceEntries();
}

void BuildAndroid::HandleEvent(StringHash eventType, VariantMap& eventData)
{

    if (eventType == E_SUBPROCESSOUTPUT)
    {
        if (currentBuildPhase_ == ADBListDevices)
        {
            deviceListText_ += eventData[SubprocessOutput::P_TEXT].GetString();
        }
    }


    if (eventType == E_SUBPROCESSCOMPLETE)
    {
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

        int code = eventData[SubprocessComplete::P_RETCODE].GetInt();

        if (!code || currentBuildPhase_ == ADBListDevices)
        {
            // success
            if (currentBuildPhase_ == AndroidUpdateProject)
            {
                RunAntDebug();
            }
            else if (currentBuildPhase_ == AntBuildDebug)
            {
                RunADBListDevices();
            }
            else if (currentBuildPhase_ == ADBListDevices)
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
                    // can't proceed
                    buildSystem->BuildComplete(AE_PLATFORM_ANDROID, buildPath_);
                }

            }
            else if (currentBuildPhase_ == ADBInstall)
            {
                RunStartActivity();
            }
            else if (currentBuildPhase_ == ADBStartActivity)
            {
                buildSystem->BuildComplete(AE_PLATFORM_ANDROID, buildPath_);
            }

        }
        else
        {

            buildSystem->BuildComplete(AE_PLATFORM_ANDROID, buildPath_, false);
        }


    }
}

// adb shell am start -n com.package.name/com.package.name.ActivityName

void BuildAndroid::RunStartActivity()
{
    Editor* editor = GetSubsystem<Editor>();
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    AndroidBuildSettings buildSettings = buildSystem->GetBuildSettings()->GetAndroidSettings();


    String adbCommand = editor->GetPreferences()->GetAndroidSDKPath();

#ifdef ATOMIC_PLATFORM_OSX
    adbCommand += "platform-tools/adb";
#else
    adbCommand += "/platform-tools/adb.exe";
#endif
    String stringArgs;
    const char* cpackage = buildSettings.package.CString();
    stringArgs.AppendWithFormat("shell am start -n %s/%s.AtomicGameEngine",cpackage, cpackage);

    Vector<String> args = stringArgs.Split(' ');

    currentBuildPhase_ = ADBStartActivity;
    Subprocess* subprocess = subs->Launch(adbCommand, args, buildPath_);
    if (!subprocess)
    {
        assert(0);
        // ERROR
        return;
    }

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(BuildAndroid, HandleEvent));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(BuildAndroid, HandleEvent));


    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->SetProgramOutputSubprocess(subprocess);

    ops->PrintToProgramOutput("\n\nStarting Android Activity\n\n");

}

void BuildAndroid::RunADBInstall()
{
    Editor* editor = GetSubsystem<Editor>();
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

    String adbCommand = editor->GetPreferences()->GetAndroidSDKPath();
#ifdef ATOMIC_PLATFORM_OSX
    adbCommand += "platform-tools/adb";
#else
    adbCommand += "/platform-tools/adb.exe";
#endif

    Vector<String> args = String("install -r ./bin/Atomic-debug-unaligned.apk").Split(' ');

    currentBuildPhase_ = ADBInstall;
    Subprocess* subprocess = subs->Launch(adbCommand, args, buildPath_);

    if (!subprocess)
    {
        assert(0);
        // ERROR
        return;
    }

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(BuildAndroid, HandleEvent));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(BuildAndroid, HandleEvent));

    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->SetProgramOutputSubprocess(subprocess);

    ops->PrintToProgramOutput("\n\n<color #D4FB79>Installing on Android Device</color>\n\n");

}

void BuildAndroid::RunADBListDevices()
{
    Editor* editor = GetSubsystem<Editor>();
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

    deviceListText_.Clear();

    String adbCommand = editor->GetPreferences()->GetAndroidSDKPath();
#ifdef ATOMIC_PLATFORM_OSX
    adbCommand += "platform-tools/adb";
#else
    adbCommand += "/platform-tools/adb.exe";
#endif

    Vector<String> args = String("devices").Split(' ');

    currentBuildPhase_ = ADBListDevices;
    Subprocess* subprocess = subs->Launch(adbCommand, args, "");

    if (!subprocess)
    {
        assert(0);
        // ERROR
        return;
    }

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(BuildAndroid, HandleEvent));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(BuildAndroid, HandleEvent));


    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->SetProgramOutputSubprocess(subprocess);

    ops->PrintToProgramOutput("\n\n<color #D4FB79>Listing Android Devices</color>\n\n");

}

void BuildAndroid::RunAntDebug()
{
    Editor* editor = GetSubsystem<Editor>();
    AEPreferences* prefs = editor->GetPreferences();
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

     Poco::Process::Env env;

#ifdef ATOMIC_PLATFORM_OSX
    String antCommand = "/usr/local/bin/ant";
    Vector<String> args;
    args.Push("debug");
#else
     // C:\ProgramData\Oracle\Java\javapath;
    Vector<String> args;
    String antCommand = "cmd";
    String antPath = prefs->GetAntPath() + "/ant.bat";
    env["JAVA_HOME"] = prefs->GetJDKRootPath().CString();
    // ant is a batch file on windows, so have to run with cmd /c
    args.Push("/c");
    args.Push("\"" + antPath + "\"");
    args.Push("debug");
#endif

    currentBuildPhase_ = AntBuildDebug;
    Subprocess* subprocess = subs->Launch(antCommand, args, buildPath_, env);

    if (!subprocess)
    {
        assert(0);
        // ERROR
        return;
    }

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(BuildAndroid, HandleEvent));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(BuildAndroid, HandleEvent));


    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->ShowProgramOutput(subprocess);

    ops->PrintToProgramOutput("<color #D4FB79>Starting Android Deployment</color>\n\n");
}

void BuildAndroid::RunAndroidUpdate()
{
    Editor* editor = GetSubsystem<Editor>();
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

    String androidCommand = editor->GetPreferences()->GetAndroidSDKPath();
    androidCommand += "tools/android";

    Vector<String> args = String("update project -p . -t 33").Split(' ');

    currentBuildPhase_ = AndroidUpdateProject;
    Subprocess* subprocess = subs->Launch(androidCommand, args, buildPath_);

    if (!subprocess)
    {
        assert(0);
        // ERROR
        return;
    }

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(BuildAndroid, HandleEvent));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(BuildAndroid, HandleEvent));


    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->ShowProgramOutput(subprocess);

}

void BuildAndroid::Build(const String& buildPath)
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    buildPath_ = buildPath + "/Android-Build";

    androidSDKPath_ = editor->GetPreferences()->GetAndroidSDKPath();

#ifdef ATOMIC_PLATFORM_OSX
    androidCommand_ = androidSDKPath_ + "platform/android";
    adbCommand_ = androidSDKPath_ + "platform-tools/adb";
#else
    androidCommand_ = androidSDKPath_ + "platform/android.bat";
    adbCommand_ = androidSDKPath_ + "platform-tools/adb.exe";
#endif

    Initialize();

    //generate manifest file
    String manifest;
    for (unsigned i = 0; i < resourceEntries_.Size(); i++)
    {
        BuildResourceEntry* entry = resourceEntries_[i];
        manifest += entry->packagePath_;
        if ( i != resourceEntries_.Size() - 1)
            manifest += ";";
    }

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (fileSystem->DirExists(buildPath_))
        fileSystem->RemoveDir(buildPath_, true);

    if (fileSystem->DirExists(buildPath_))
    {
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
        buildSystem->BuildComplete(AE_PLATFORM_ANDROID, buildPath_, false);
        editor->PostModalInfo("Build Error", ToString("Build Folder:\n\n %s\n\nis in use and could not be removed, please select another build folder.", buildPath_.CString()));
        return;
    }

#ifdef ATOMIC_PLATFORM_OSX
    String buildSourceDir = fileSystem->GetAppBundleResourceFolder();
#else
    String buildSourceDir = fileSystem->GetProgramDir();
#endif

    String androidProject = buildSourceDir + "Deployment/Android";
    String projectResources = project->GetResourcePath();
    String coreDataFolder = buildSourceDir + "CoreData/";

    fileSystem->CopyDir(androidProject, buildPath_);

    if (!fileSystem->DirExists(buildPath_))
    {
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
        buildSystem->BuildComplete(AE_PLATFORM_ANDROID, buildPath_, false);
        editor->PostModalInfo("Build Error", ToString("Build Folder:\n\n %s\n\ncould not be written to, is it in use?", buildPath_.CString()));
        return;
    }

    fileSystem->CopyDir(projectResources, buildPath_ + "/assets/AtomicResources");    
    fileSystem->CopyDir(coreDataFolder, buildPath_ + "/assets/CoreData");

    // write the manifest
    SharedPtr<File> mfile(new File(context_, buildPath_ + "/assets/AtomicManifest", FILE_WRITE));
    mfile->WriteString(manifest);
    mfile->Close();

    AndroidProjectGenerator gen(context_);
    gen.SetBuildPath(buildPath_);

    if (!gen.Generate())
    {
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
        buildSystem->BuildComplete(AE_PLATFORM_ANDROID, buildPath_, false);
        editor->PostModalInfo("Build Error", ToString("Unable to generate Android project, please check build settings and try again"));
        return;
    }

    // RunAndroidUpdate();
    RunAntDebug();
}

}
