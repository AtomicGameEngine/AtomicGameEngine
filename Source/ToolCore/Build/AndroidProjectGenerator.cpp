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

AndroidProjectGenerator::AndroidProjectGenerator(Context* context, BuildBase* buildBase) :
    Object(context),
    buildBase_(buildBase)
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

    if (!CopyUserIcons())
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
        errorText_ = "Invalid App Package name. The general naming convention is com.company.appname";
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
        errorText_ = "Project generator unable to create dirs " + path;
        return false;
    }

    String source;
    source.AppendWithFormat("package %s;\n", packageName.CString());

    source += "import org.libsdl.app.SDLActivity;\n";

    source += "public class AtomicGameEngine extends SDLActivity {\n";

    source += "}\n";

    File file(context_, path + "/AtomicGameEngine.java", FILE_WRITE);

    if (!file.IsOpen())
    {
        errorText_ = "Project generator unable to open file " + path + "/AtomicGameEngine.java";
        return false;
    }
    file.Write(source.CString(), source.Length());

    return true;

}

bool AndroidProjectGenerator::GenerateLocalProperties( )
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolPrefs* prefs = tenv->GetToolPrefs();
    String sdkPath = prefs->GetAndroidSDKPath();

    if (!sdkPath.Length())
    {
        errorText_ = "Invalid Android SDK Path, select the path in Build Settings.";
        return false;
    }

    String props;
    props.AppendWithFormat("sdk.dir=%s", sdkPath.CString());

    File file(context_, buildPath_ + "/local.properties", FILE_WRITE);

    if (!file.IsOpen())
    {
        errorText_ = "Project generator unable to open file " + buildPath_ + "/local.properties ";
        return false;
    }
    
    file.Write(props.CString(), props.Length());


    if ( prefs->GetReleaseCheck() > 0 ) // if release flag is set ...
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        String Reldir = prefs->GetReleasePath();
        if (!fileSystem->DirExists(Reldir))
        {
            errorText_ = "Invalid Release Path, select the path in Build Settings.";
            return false;
        }
        
        String antname = Reldir + "/ant.properties";
        if ( !fileSystem->FileExists ( antname) ) 
        {
            errorText_ = "The file ant.properties not found in " + Reldir + ", unable to generate Release APK.";
            return false;
        }

        if ( !buildBase_->BuildCopyFile ( antname, buildPath_ + "/ant.properties" ))
            return false;

    }
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
        errorText_ = "Invalid Android API level, Press Refresh and select a valid level.";
        return false;
    }


    String props;
    props.AppendWithFormat("target=%s", apiString.CString());

    File file(context_, buildPath_ + "/project.properties", FILE_WRITE);

    if (!file.IsOpen())
    {
        errorText_ = "Project generator unable to open file project.properties in " + buildPath_;
        return false;
    }
    file.Write(props.CString(), props.Length());

    return true;

}

bool AndroidProjectGenerator::GenerateStringXML()
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    Project* project = toolSystem->GetProject();
    AndroidBuildSettings* settings = project->GetBuildSettings()->GetAndroidBuildSettings();

    String appName = settings->GetAppName();

    if (!appName.Length())
    {
        errorText_ = "Invalid App Name, select the App Name in Build Settings.";
        return false;
    }

    String strings  = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

    strings += "<resources>\n";

    strings.AppendWithFormat("<string name=\"app_name\">%s</string>\n", appName.CString());

    strings += "</resources>\n";
    
    // Create res/values if it doesn't exist
    if (!fs->DirExists(buildPath_ + "/res/values"))
    {
        fs->CreateDirsRecursive(buildPath_ + "/res/values");
    }
    
    // Check that we successfully created it
    if (!fs->DirExists(buildPath_ + "/res/values"))
    {
        errorText_ = "Unable to create directory: " + buildPath_ + "/res/values";
        return false;
    }
    
    File file(context_, buildPath_ + "/res/values/strings.xml", FILE_WRITE);

    if (!file.IsOpen())
    {
        errorText_ = "Unable to write: " + buildPath_ + "/res/values/strings.xml";
        return false;
    }

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
        errorText_ = "Invalid App Package name. The general naming convention is com.company.appname";
        return false;
    }

    // TODO: from settings -- should this be ProductName ?
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
    manifest += "android:installLocation=\"auto\">\n";

    manifest += "<uses-permission android:name=\"android.permission.WRITE_EXTERNAL_STORAGE\"/>\n";
    manifest += "<uses-permission android:name=\"android.permission.INTERNET\" />\n";
    manifest += "<uses-feature android:glEsVersion=\"0x00020000\" />\n";
    manifest += "<uses-sdk android:targetSdkVersion=\"12\" android:minSdkVersion=\"10\" />\n";

    manifest += "<application\n";
    manifest += "android:label=\"@string/app_name\"\n";
    manifest += "android:icon=\"@drawable/icon\"\n";
    manifest += "android:theme=\"@android:style/Theme.NoTitleBar.Fullscreen\"\n";
    manifest += "android:hardwareAccelerated=\"true\"\n";
    manifest += "android:allowBackup=\"false\">\n";

    manifest.AppendWithFormat("<activity android:name=\".%s\"\n", activityName.CString());

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

bool AndroidProjectGenerator::CopyUserIcons()
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    Project* project = toolSystem->GetProject();
    AndroidBuildSettings* settings = project->GetBuildSettings()->GetAndroidBuildSettings();

    String userIconPath = settings->GetIconPath();
    if (!fileSystem->DirExists(userIconPath))               // dont do anything if there is no path defined.
        return true;
            
    String userIconDir = userIconPath + "/drawable";        // 1st target dir 
    String userIconFile = userIconDir + "/logo_large.png";  // 1st target file
    String destDir = buildPath_ + "/res/drawable";          // where it should be in the build
    if ( fileSystem->FileExists (userIconFile) )            // is there a file there?
    {
        if ( !buildBase_->BuildCopyFile ( userIconFile, destDir + "/logo_large.png" ))
            return false;
    }

    userIconDir = userIconPath + "/drawable-ldpi"; 
    userIconFile = userIconDir + "/icon.png"; 
    destDir = buildPath_ + "/res/drawable-ldpi";
    if ( fileSystem->FileExists (userIconFile) )
    {
        if ( !buildBase_->BuildCopyFile ( userIconFile, destDir + "/icon.png"))
            return false;
    } 

    userIconDir = userIconPath + "/drawable-mdpi"; 
    userIconFile = userIconDir + "/icon.png"; 
    destDir = buildPath_ + "/res/drawable-mdpi";
    {
        if ( !buildBase_->BuildCopyFile ( userIconFile, destDir + "/icon.png" ))
            return false;
    } 

    userIconDir = userIconPath + "/drawable-hdpi"; 
    userIconFile = userIconDir + "/icon.png"; 
    destDir = buildPath_ + "/res/drawable-hdpi";
    if ( fileSystem->FileExists (userIconFile) )
    {
        if ( !buildBase_->BuildCopyFile ( userIconFile, destDir + "/icon.png" ))
            return false;
    }

    return true;
}

}
