//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Poco/File.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"
#include "../Project/ProjectBuildSettings.h"
#include "BuildMac.h"
#include "BuildSystem.h"

#include "AndroidProjectGenerator.h"

namespace ToolCore
{

AndroidProjectGenerator::AndroidProjectGenerator(Context* context) :
    Object(context)
{

}

AndroidProjectGenerator::~AndroidProjectGenerator()
{

}

bool AndroidProjectGenerator::Generate()
{
    if (!GenerateAndroidManifest())
        return false;

    if (!GenerateStringXML())
        return false;

    if (!GenerateLocalProperties())
        return false;

    if (!GenerateProjectProperties())
        return false;

    if (!GenerateActivitySource())
        return false;

    return true;
}

bool AndroidProjectGenerator::GenerateActivitySource()
{
    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    Project* project = toolSystem->GetProject();
    AndroidBuildSettings* settings = project->GetBuildSettings()->GetAndroidBuildSettings();

    String packageName = settings->GetPackageName();

    if (!packageName.Length())
    {
        errorText_ = "Invalid Package Name";
        return false;
    }

    Vector<String> elements = settings->GetPackageName().Split('.');
    String path;
    path.Join(elements, "/");

    path = buildPath_ + "/src/" + path;

    Poco::File dirs(path.CString());
    dirs.createDirectories();

    if (!dirs.exists())
    {
        errorText_ = "Unable to create ";
        return false;
    }

    String source;
    source.AppendWithFormat("package %s;\n", packageName.CString());

    source += "import org.libsdl.app.SDLActivity;\n";

    source += "public class AtomicGameEngine extends SDLActivity {\n";

    source += "}\n";

    File file(context_, path + "/AtomicGameEngine.java", FILE_WRITE);

    if (!file.IsOpen())
        return false;

    file.Write(source.CString(), source.Length());

    return true;

}

bool AndroidProjectGenerator::GenerateLocalProperties()
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolPrefs* prefs = tenv->GetToolPrefs();
    String sdkPath = prefs->GetAndroidSDKPath();

    if (!sdkPath.Length())
    {
        errorText_ = "Invalid Android SDK Path";
        return false;
    }

    String props;
    props.AppendWithFormat("sdk.dir=%s", sdkPath.CString());

    File file(context_, buildPath_ + "/local.properties", FILE_WRITE);

    if (!file.IsOpen())
        return false;

    file.Write(props.CString(), props.Length());

    return true;

}

bool AndroidProjectGenerator::GenerateProjectProperties()
{
    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    Project* project = toolSystem->GetProject();
    AndroidBuildSettings* settings = project->GetBuildSettings()->GetAndroidBuildSettings();

    String apiString = settings->GetSDKVersion();

    if (!apiString.Length())
    {
        errorText_ = "Invalid Android API";
        return false;
    }


    String props;
    props.AppendWithFormat("target=%s", apiString.CString());

    File file(context_, buildPath_ + "/project.properties", FILE_WRITE);

    if (!file.IsOpen())
        return false;

    file.Write(props.CString(), props.Length());

    return true;

}

bool AndroidProjectGenerator::GenerateStringXML()
{
    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    Project* project = toolSystem->GetProject();
    AndroidBuildSettings* settings = project->GetBuildSettings()->GetAndroidBuildSettings();

    String appName = settings->GetAppName();

    if (!appName.Length())
    {
        errorText_ = "Invalid App Name";
        return false;
    }

    String strings  = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

    strings += "<resources>\n";

    strings.AppendWithFormat("<string name=\"app_name\">%s</string>\n", appName.CString());

    strings += "</resources>\n";

    File file(context_, buildPath_ + "/res/values/strings.xml", FILE_WRITE);

    if (!file.IsOpen())
        return false;

    file.Write(strings.CString(), strings.Length());

    return true;

}

bool AndroidProjectGenerator::GenerateAndroidManifest()
{
    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    Project* project = toolSystem->GetProject();
    AndroidBuildSettings* settings = project->GetBuildSettings()->GetAndroidBuildSettings();

    String package = settings->GetPackageName();


    if (!package.Length())
    {
        errorText_ = "Invalid Package Name";
        return false;
    }

    // TODO: from settings
    String activityName = "AtomicGameEngine";
    if (!activityName.Length())
    {
        errorText_ = "Invalid Activity Name";
        return false;
    }

    String manifest  = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    manifest += "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    manifest.AppendWithFormat("package=\"%s\"\n", package.CString());
    manifest += "android:versionCode=\"1\"\n";
    manifest += "android:versionName=\"1.0\">\n";

    manifest += "<uses-permission android:name=\"android.permission.INTERNET\" />\n";
    manifest += "<uses-feature android:glEsVersion=\"0x00020000\" />\n";
    manifest += "<uses-sdk android:targetSdkVersion=\"12\" android:minSdkVersion=\"10\" />\n";

    manifest += "<application android:label=\"@string/app_name\" android:icon=\"@drawable/icon\">\n";
    manifest.AppendWithFormat("<activity android:name=\".%s\"\n", activityName.CString());

    manifest += "android:label=\"@string/app_name\"\n";
    manifest += "android:theme=\"@android:style/Theme.NoTitleBar.Fullscreen\"\n";
    manifest += "android:configChanges=\"keyboardHidden|orientation\"\n";
    manifest += "android:screenOrientation=\"landscape\">\n";
    manifest += "<intent-filter>\n";
    manifest += "<action android:name=\"android.intent.action.MAIN\" />\n";
    manifest += "<category android:name=\"android.intent.category.LAUNCHER\" />\n";
    manifest += "</intent-filter>\n";
    manifest += "</activity>\n";
    manifest += "</application>\n";
    manifest += "</manifest>\n";

    File file(context_, buildPath_ + "/AndroidManifest.xml", FILE_WRITE);

    if (!file.IsOpen())
        return false;

    file.Write(manifest.CString(), manifest.Length());

    return true;

}

}

/*

*/
