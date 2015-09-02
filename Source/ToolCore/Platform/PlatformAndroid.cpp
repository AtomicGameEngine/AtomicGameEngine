
#include <Atomic/IO/FileSystem.h>

#include "../ToolEnvironment.h"
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
    return 0;
}

void PlatformAndroid::PrependAndroidCommandArgs(Vector<String> args)
{
    // android is a batch file on windows, so have to run with cmd /c
    args.Push("/c");
    args.Push("\"" + GetAndroidCommand() + "\"");

}

String PlatformAndroid::GetAndroidCommand() const
{
    ToolPrefs* prefs = GetSubsystem<ToolEnvironment>()->GetToolPrefs();

    String androidCommand = GetNativePath(prefs->GetAndroidSDKPath());

    if (!androidCommand.Length())
        return String::EMPTY;

#ifdef ATOMIC_PLATFORM_OSX
    //Vector<String> args = String("list targets").Split(' ');
    androidCommand += "tools/android";
#else

    // android is a batch file on windows, so have to run with cmd /c
    androidCommand += "\\tools\\android.bat";

    //androidCommand = "cmd";
#endif

    return androidCommand;

}


}
