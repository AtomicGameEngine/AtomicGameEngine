
#if defined(WIN32) && !defined(ATOMIC_WIN32_CONSOLE)
#include <Atomic/Core/MiniDump.h>
#include <windows.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#endif

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>

#include "AEApplication.h"
#include "Player/AEPlayerApplication.h"

using namespace AtomicEditor;

static int RunEditorApplication()
{
    Atomic::SharedPtr<Atomic::Context> context(new Atomic::Context());
    Atomic::SharedPtr<AEApplication> application(new AEApplication(context));
    return application->Run();
}

static int RunPlayerApplication()
{
    Atomic::SharedPtr<Atomic::Context> context(new Atomic::Context());
    Atomic::SharedPtr<AEPlayerApplication> application(new AEPlayerApplication(context));
    return application->Run();
}


// Define a platform-specific main function, which in turn executes the user-defined function

// MSVC debug mode: use memory leak reporting
#if defined(_MSC_VER) && defined(_DEBUG) && !defined(ATOMIC_WIN32_CONSOLE)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    Atomic::ParseArguments(GetCommandLineW());

    const Vector<String>& arguments = GetArguments();

    bool runPlayer = false;
    for (unsigned i = 0; i < arguments.Size();i++)
    {
        if (arguments.At(i) == "--player")
        {
            runPlayer = true;
            break;
        }
    }

    if (runPlayer)
        return RunPlayerApplication();

    return RunEditorApplication();

}
// MSVC release mode: write minidump on crash
#elif defined(_MSC_VER) && defined(ATOMIC_MINIDUMPS) && !defined(ATOMIC_WIN32_CONSOLE)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    Atomic::ParseArguments(GetCommandLineW());
    int exitCode;
    __try
    {
        exitCode = function;
    }
    __except(Atomic::WriteMiniDump("Atomic", GetExceptionInformation()))
    {
    }
    return exitCode;
}
// Other Win32 or minidumps disabled: just execute the function
#elif defined(WIN32) && !defined(ATOMIC_WIN32_CONSOLE)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    Atomic::ParseArguments(GetCommandLineW());

    const Vector<String>& arguments = GetArguments();

    bool runPlayer = false;
    for (unsigned i = 0; i < arguments.Size();i++)
    {
        if (arguments.At(i) == "--player")
        {
            runPlayer = true;
            break;
        }
    }

    if (runPlayer)
        return RunPlayerApplication();

    return RunEditorApplication();
}
// Linux or OS X: use main
#else
int main(int argc, char** argv)
{
    Atomic::ParseArguments(argc, argv);

    const Vector<String>& arguments = GetArguments();

    bool runPlayer = false;
    for (unsigned i = 0; i < arguments.Size();i++)
    {
        if (arguments.At(i) == "--player")
        {
            runPlayer = true;
            break;
        }
    }

    if (runPlayer)
        return RunPlayerApplication();

    return RunEditorApplication();
}
#endif
