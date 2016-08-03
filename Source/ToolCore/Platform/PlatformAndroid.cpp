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

#include <Atomic/IO/MemoryBuffer.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>

#include "../ToolEnvironment.h"
#include "../Subprocess/SubprocessSystem.h"
#include "../License/LicenseSystem.h"
#include "../Build/BuildAndroid.h"

#include "PlatformAndroid.h"

namespace ToolCore
{

PlatformAndroid::PlatformAndroid(Context* context) : Platform(context)
{

}

PlatformAndroid::~PlatformAndroid()
{

}

BuildBase* PlatformAndroid::NewBuild(Project *project)
{
    return new BuildAndroid(context_, project);
}

void PlatformAndroid::PrependAndroidCommandArgs(Vector<String> args)
{
#ifdef ATOMIC_PLATFORM_WINDOWS
    // android is a batch file on windows, so have to run with cmd /c
    args.Push("/c");
    args.Push("\"" + GetAndroidCommand() + "\"");
#endif

}

void PlatformAndroid::HandleRefreshAndroidTargetsEvent(StringHash eventType, VariantMap& eventData)
{
    if (eventType == E_SUBPROCESSOUTPUT)
    {
        targetOutput_ += eventData[SubprocessOutput::P_TEXT].GetString();
    }
    else if (eventType == E_SUBPROCESSCOMPLETE)
    {
        refreshAndroidTargetsProcess_ = 0;

        androidTargets_.Clear();

        MemoryBuffer reader(targetOutput_.CString(), targetOutput_.Length() + 1);

        while (!reader.IsEof())
        {
            String line = reader.ReadLine();
            if (line.StartsWith("id:"))
            {
                //id: 33 or "Google Inc.:Google APIs (x86 System Image):19"
                Vector<String> elements = line.Split('\"');
                if (elements.Size() == 2)
                {
                    String api = elements[1];

                    androidTargets_.Push(api);
                }
            }
        }

        SendEvent(E_ANDROIDTARGETSREFRESHED);
    }

}

void PlatformAndroid::RefreshAndroidTargets()
{
    if (refreshAndroidTargetsProcess_.NotNull())
        return;

    ToolPrefs* prefs = GetSubsystem<ToolEnvironment>()->GetToolPrefs();
    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    String androidSDKPath = prefs->GetAndroidSDKPath();

    if (!fileSystem->DirExists(androidSDKPath))
    {
        ATOMIC_LOGERRORF("The Android SDK path %s does not exist", androidSDKPath.CString());
        return;
    }

    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

    String androidCommand = GetAndroidCommand();

    Vector<String> args;
    PrependAndroidCommandArgs(args);
    args.Push("list");
    args.Push("targets");

    targetOutput_.Clear();
    refreshAndroidTargetsProcess_ = subs->Launch(androidCommand, args);

    if (refreshAndroidTargetsProcess_.NotNull())
    {

        SubscribeToEvent(refreshAndroidTargetsProcess_, E_SUBPROCESSCOMPLETE, ATOMIC_HANDLER(PlatformAndroid, HandleRefreshAndroidTargetsEvent));
        SubscribeToEvent(refreshAndroidTargetsProcess_, E_SUBPROCESSOUTPUT, ATOMIC_HANDLER(PlatformAndroid, HandleRefreshAndroidTargetsEvent));


    }

}

String PlatformAndroid::GetADBCommand() const
{
    ToolPrefs* prefs = GetSubsystem<ToolEnvironment>()->GetToolPrefs();

    String adbCommand = prefs->GetAndroidSDKPath();

#ifdef ATOMIC_PLATFORM_OSX
    adbCommand += "/platform-tools/adb";
#endif
#ifdef ATOMIC_PLATFORM_WINDOWS
    adbCommand += "/platform-tools/adb.exe";
#endif
#ifdef ATOMIC_PLATFORM_LINUX
    adbCommand += "/platform-tools/adb";
#endif

    return adbCommand;

}

String PlatformAndroid::GetAndroidCommand() const
{
    ToolPrefs* prefs = GetSubsystem<ToolEnvironment>()->GetToolPrefs();

    String androidCommand = GetNativePath(prefs->GetAndroidSDKPath());

    if (!androidCommand.Length())
        return String::EMPTY;

#ifdef ATOMIC_PLATFORM_OSX
    //Vector<String> args = String("list targets").Split(' ');
    androidCommand += "/tools/android";
#endif
#ifdef ATOMIC_PLATFORM_WINDOWS

    // android is a batch file on windows, so have to run with cmd /c
    androidCommand += "\\tools\\android.bat";

    //androidCommand = "cmd";
#endif
#ifdef ATOMIC_PLATFORM_LINUX
    androidCommand += "/tools/android";
#endif
    return androidCommand;

}

bool PlatformAndroid::GetLicense()
{
// BEGIN LICENSE MANAGEMENT
    return GetSubsystem<LicenseSystem>()->GetLicenseAndroid();
// END LICENSE MANAGEMENT
}

}
