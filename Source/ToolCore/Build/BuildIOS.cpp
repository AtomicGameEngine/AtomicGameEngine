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

#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"
#include "../Project/ProjectBuildSettings.h"
#include "../Assets/AssetDatabase.h"

#include "../Subprocess/SubprocessSystem.h"

#include "BuildIOS.h"
#include "BuildEvents.h"
#include "BuildSystem.h"

namespace ToolCore
{

BuildIOS::BuildIOS(Context * context, Project *project) : BuildBase(context, project, PLATFORMID_IOS)
{

}

BuildIOS::~BuildIOS()
{

}

String BuildIOS::GenerateInfoPlist()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();
    ProjectBuildSettings* buildSettings = project->GetBuildSettings();
    IOSBuildSettings* iosSettings = buildSettings->GetIOSBuildSettings();

    String plist = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    plist += "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
    plist += "<plist version=\"1.0\">\n";
    plist += "<dict>\n";

    plist += "<key>CFBundleDevelopmentRegion</key>\n";
    plist += "<string>English</string>\n";

    plist += "<key>CFBundleExecutable</key>\n";
    plist += "<string>AtomicPlayer</string>\n";

    plist += "<key>CFBundleGetInfoString</key>\n";
    plist += "<string>\"Atomic Player\"</string>\n";

    plist += "<key>CFBundleIconFile</key>\n";
    plist += "<string></string>\n";

    plist += "<key>CFBundleIdentifier</key>\n";
    plist.AppendWithFormat("<string>%s</string>\n", iosSettings->GetPackageName().CString());

    plist += "<key>CFBundleInfoDictionaryVersion</key>\n";
    plist += "<string>6.0</string>\n";

    plist += "<key>CFBundleLongVersionString</key>\n";
    plist += "<string></string>\n";

    plist +=  "<key>CFBundleName</key>\n";
    plist += "<string></string>\n";
    plist += "<key>CFBundlePackageType</key>\n";
    plist += "<string>APPL</string>\n";
    plist += "<key>CFBundleShortVersionString</key>\n";
    plist += "<string></string>\n";
    plist += "<key>CFBundleSignature</key>\n";
    plist += "<string>????</string>\n";
    plist += "<key>CFBundleVersion</key>\n";
    plist += "<string></string>\n";
    plist += "<key>CSResourcesFileMapped</key>\n";
    plist += "<true/>\n";
    plist += "<key>LSRequiresCarbon</key>\n";
    plist += "<true/>\n";
    plist += "<key>NSHumanReadableCopyright</key>\n";
    plist += "<string>\"(c) Your Company\"</string>\n";
    plist += "<key>CFBundleIconFiles</key>\n";
    plist += "<array>\n";
    plist += "<string></string>\n";
    plist += "</array>\n";
    plist += "</dict>\n";
    plist += "</plist>\n";

    return plist;

}

String BuildIOS::GenerateEntitlements()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();
    ProjectBuildSettings* buildSettings = project->GetBuildSettings();
    IOSBuildSettings* iosSettings = buildSettings->GetIOSBuildSettings();

    String app_identifer = iosSettings->GetAppIDPrefix() + ".";
    app_identifer += iosSettings->GetPackageName();

    String team_identifier = iosSettings->GetAppIDPrefix();

    String keychain_access_group = app_identifer;

    String entitlements = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    entitlements += "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
    entitlements += "<plist version=\"1.0\">\n";
    entitlements += "<dict>\n";
    entitlements += "<key>application-identifier</key>\n";
    entitlements.AppendWithFormat("<string>%s</string>\n", app_identifer.CString());
    entitlements += "<key>com.apple.developer.team-identifier</key>\n";
    entitlements.AppendWithFormat("<string>%s</string>\n", team_identifier.CString());
    entitlements += "<key>get-task-allow</key>\n";
    entitlements += "<true/>\n";
    entitlements += "<key>keychain-access-groups</key>\n";
    entitlements += "<array>\n";
    entitlements.AppendWithFormat("<string>%s</string>\n", keychain_access_group.CString());
    entitlements += "</array>\n";
    entitlements += "</dict>\n";
    entitlements += "</plist>\n";

    return entitlements;
}

void BuildIOS::Initialize()
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

void BuildIOS::RunConvertPList()
{
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

    Vector<String> args = String("-convert binary1 ./AtomicPlayer.app/Info.plist").Split(' ');

    currentBuildPhase_ = ConvertPList;
    Subprocess* subprocess = subs->Launch("/usr/bin/plutil", args, buildPath_);

    if (!subprocess)
    {
        FailBuild("BuildFailed::RunConvertPList");
        return;
    }

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Starting iOS Deployment</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);


    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, ATOMIC_HANDLER(BuildIOS, HandleConvertPListComplete));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, ATOMIC_HANDLER(BuildBase, HandleSubprocessOutputEvent));

}

void BuildIOS::HandleConvertPListComplete(StringHash eventType, VariantMap& eventData)
{
    int code = eventData[SubprocessComplete::P_RETCODE].GetInt();

    if (!code)
    {
        RunCodeSign();
    }
    else
    {
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
        buildSystem->BuildComplete(PLATFORMID_IOS, buildPath_, false);
    }

}

void BuildIOS::RunCodeSign()
{
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

    Vector<String> args;
    args.Push("--force");
    args.Push("--sign");
    args.Push("iPhone Developer");
    args.Push("--entitlements");
    args.Push("./AtomicPlayer.app.xcent");
    args.Push("./AtomicPlayer.app");

    Poco::Process::Env env;
    env["CODESIGN_ALLOCATE"] =  "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/codesign_allocate";

    currentBuildPhase_ = CodeSign;
    Subprocess* subprocess = subs->Launch("/usr/bin/codesign", args, buildPath_, env);

    if (!subprocess)
    {
        FailBuild("BuildFailed::RunCodeSign");
        return;
    }

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Code Signing iOS Deployment</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, ATOMIC_HANDLER(BuildIOS, HandleCodeSignComplete));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, ATOMIC_HANDLER(BuildBase, HandleSubprocessOutputEvent));

}

void BuildIOS::HandleCodeSignComplete(StringHash eventType, VariantMap& eventData)
{
    int code = eventData[SubprocessComplete::P_RETCODE].GetInt();

    if (!code)
    {
        RunDeploy();
    }
    else
    {
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
        buildSystem->BuildComplete(PLATFORMID_IOS, buildPath_, false);
    }

}

void BuildIOS::RunDeploy()
{
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    String iosDeploy = tenv->GetIOSDeployBinary();

    Vector<String> args;

    // https://github.com/phonegap/ios-deploy/issues/152
    // args.Push("--uninstall");

    args.Push("--bundle");
    args.Push("./AtomicPlayer.app");

    currentBuildPhase_ = Deploy;

    Subprocess* subprocess = subs->Launch(iosDeploy.CString(), args, buildPath_);

    if (!subprocess)
    {
        FailBuild("BuildFailed::RunDeploy");
        return;
    }

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Deploying to iOS Device</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, ATOMIC_HANDLER(BuildIOS, HandleDeployComplete));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, ATOMIC_HANDLER(BuildBase, HandleSubprocessOutputEvent));


}

void BuildIOS::HandleDeployComplete(StringHash eventType, VariantMap& eventData)
{
    int code = eventData[SubprocessComplete::P_RETCODE].GetInt();

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    buildSystem->BuildComplete(PLATFORMID_IOS, buildPath_, true);

}


void BuildIOS::Build(const String& buildPath)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();
    ProjectBuildSettings* buildSettings = project->GetBuildSettings();
    IOSBuildSettings* iosSettings = buildSettings->GetIOSBuildSettings();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    Initialize();

    if (!BuildClean(buildPath_))
        return;

    String buildSourceDir = tenv->GetToolDataDir();

    String buildAppSourceDir = buildSourceDir + "Deployment/IOS/AtomicPlayer.app";

    if (!BuildCreateDirectory(buildPath_))
        return;

    String buildDestDist = buildPath_ + "/AtomicPlayer.app";

    if (!BuildCreateDirectory(buildDestDist))
        return;

    String resourcePackagePath = buildDestDist + "/AtomicResources" + PAK_EXTENSION;
    GenerateResourcePackage(resourcePackagePath);

    if (!BuildCopyFile(buildAppSourceDir + "/AtomicPlayer", buildDestDist + "/AtomicPlayer"))
        return;
    if (!BuildCopyFile(buildAppSourceDir + "/PkgInfo", buildDestDist + "/PkgInfo"))
        return;
    if (!BuildCopyFile(iosSettings->GetProvisionFile(), buildDestDist + "/embedded.mobileprovision"))
        return;
        
    String entitlements = GenerateEntitlements();
    String plist = GenerateInfoPlist();

    File file(context_, buildPath_ + "/AtomicPlayer.app.xcent", FILE_WRITE);

    if (file.IsOpen())
    {
        file.Write(entitlements.CString(), entitlements.Length());
        file.Close();
    }

    File pfile(context_, buildDestDist + "/Info.plist", FILE_WRITE);

    if (pfile.IsOpen())
    {
        pfile.Write(plist.CString(), plist.Length());
        pfile.Close();
    }

    RunConvertPList();

}

}
