// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>
#include "../AEEditor.h"
#include "../Project/AEProject.h"
#include "../Project/ProjectUtils.h"

#include "AEEvents.h"

#include "Subprocess/AESubprocessSystem.h"

#include "BuildIOS.h"
#include "BuildSystem.h"
#include "UI/Modal/UIModalOps.h"

#include "Tools/External/AEExternalTooling.h"



namespace AtomicEditor
{

BuildIOS::BuildIOS(Context * context) : BuildBase(context)
{

}

BuildIOS::~BuildIOS()
{

}

void BuildIOS::Initialize()
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_WINDOWS
    String bundleResources = fileSystem->GetProgramDir();
#else
    String bundleResources = fileSystem->GetAppBundleResourceFolder();
#endif

    String projectResources = project->GetResourcePath();
    String coreDataFolder = bundleResources + "CoreData/";

    AddResourceDir(coreDataFolder);
    AddResourceDir(projectResources);

    BuildResourceEntries();

}

String BuildIOS::GenerateInfoPlist()
{

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    IOSBuildSettings settings = buildSystem->GetBuildSettings()->GetIOSSettings();

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
    plist.AppendWithFormat("<string>%s</string>\n", settings.package.CString());

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
    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    IOSBuildSettings settings = buildSystem->GetBuildSettings()->GetIOSSettings();
    String app_identifer = settings.appidPrefix + ".";
    app_identifer += settings.package;

    String team_identifier = settings.appidPrefix;

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

void BuildIOS::HandleEvent(StringHash eventType, VariantMap& eventData)
{

    if (eventType == E_SUBPROCESSOUTPUT)
    {

    }

    if (eventType == E_SUBPROCESSCOMPLETE)
    {
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

        int code = eventData[SubprocessComplete::P_RETCODE].GetInt();

        if (!code)
        {
            // success
            if (currentBuildPhase_ == ConvertPList)
            {
                RunCodeSign();
            }
            else if (currentBuildPhase_ == CodeSign)
            {
                RunDeploy();
            }
            else if (currentBuildPhase_ == Deploy)
            {
                buildSystem->BuildComplete(AE_PLATFORM_IOS, buildPath_);
            }

        }
        else
        {
            buildSystem->BuildComplete(AE_PLATFORM_IOS, buildPath_, false);

        }
    }
}

void BuildIOS::RunConvertPList()
{
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

    Vector<String> args = String("-convert binary1 ./AtomicPlayer.app/Info.plist").Split(' ');

    currentBuildPhase_ = ConvertPList;
    Subprocess* subprocess = subs->Launch("/usr/bin/plutil", args, buildPath_);

    if (!subprocess)
    {
        assert(0);
        // ERROR
        return;
    }

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(BuildIOS, HandleEvent));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(BuildIOS, HandleEvent));

    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->ShowProgramOutput(subprocess);

    ops->PrintToProgramOutput("<color #D4FB79>Starting iOS Deployment</color>\n\n");
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
        assert(0);
        // ERROR
        return;
    }

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(BuildIOS, HandleEvent));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(BuildIOS, HandleEvent));

    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->SetProgramOutputSubprocess(subprocess);

    ops->PrintToProgramOutput("\n\n<color #D4FB79>Signing iOS Deployment</color>\n\n");

}

void BuildIOS::RunDeploy()
{
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();
    ExternalTooling* tooling = GetSubsystem<ExternalTooling>();
    String iosDeploy = tooling->GetToolApplicationPath();
    iosDeploy += "CommandLine/ios-deploy";


    Vector<String> args;

    args.Push("--uninstall");
    args.Push("--bundle");
    args.Push("./AtomicPlayer.app");

    currentBuildPhase_ = Deploy;
    Subprocess* subprocess = subs->Launch(iosDeploy.CString(), args, buildPath_);

    if (!subprocess)
    {
        assert(0);
        // ERROR
        return;
    }

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(BuildIOS, HandleEvent));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(BuildIOS, HandleEvent));


    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->SetProgramOutputSubprocess(subprocess);

    ops->PrintToProgramOutput("\n\n<color #D4FB79>Deploying to iOS Device</color>\n\n");

}


void BuildIOS::Build(const String& buildPath)
{
    buildPath_ = buildPath + "/IOS-Build";

    Initialize();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (fileSystem->DirExists(buildPath_))
        fileSystem->RemoveDir(buildPath_, true);


 #ifdef ATOMIC_PLATFORM_WINDOWS
    String buildSourceDir = fileSystem->GetProgramDir();
 #else
    String buildSourceDir = fileSystem->GetAppBundleResourceFolder();
 #endif

    String buildAppSourceDir = buildSourceDir + "Deployment/IOS/AtomicPlayer.app";

    fileSystem->CreateDir(buildPath_);

    String buildDestDist = buildPath_ + "/AtomicPlayer.app";

    fileSystem->CreateDir(buildDestDist);

    String resourcePackagePath = buildDestDist + "/AtomicResources.pak";
    GenerateResourcePackage(resourcePackagePath);

    fileSystem->Copy(buildAppSourceDir + "/AtomicPlayer", buildDestDist + "/AtomicPlayer");
    fileSystem->Copy(buildAppSourceDir + "/PkgInfo", buildDestDist + "/PkgInfo");

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    IOSBuildSettings settings = buildSystem->GetBuildSettings()->GetIOSSettings();

    fileSystem->Copy(settings.provisionFile, buildDestDist + "/embedded.mobileprovision");

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
