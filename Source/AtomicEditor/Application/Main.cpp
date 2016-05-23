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

#if defined(ATOMIC_PLATFORM_WINDOWS) || defined (ATOMIC_PLATFORM_LINUX)
#ifdef ATOMIC_WEBVIEW
#include <AtomicWebView/AtomicWebView.h>
#endif
#endif

#if defined(WIN32) && !defined(ATOMIC_WIN32_CONSOLE)
#include <Atomic/Core/MiniDump.h>
#include <windows.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#endif

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>

#include "AEEditorApp.h"
#include "AEPlayerApp.h"

using namespace AtomicEditor;

static int RunEditorApplication()
{
    Atomic::SharedPtr<Atomic::Context> context(new Atomic::Context());
    Atomic::SharedPtr<AEEditorApp> application(new AEEditorApp(context));
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

#ifdef ATOMIC_WEBVIEW
    int exit_code = Atomic::WebMain(0, nullptr);

    if (exit_code >= 0)
    {
        // The sub-process has completed so return here.
        return exit_code;
    }
#endif

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
#ifdef ATOMIC_WEBVIEW
    int exit_code = Atomic::WebMain(0, nullptr);

    if (exit_code >= 0)
    {
        // The sub-process has completed so return here.
        return exit_code;
    }
#endif

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

#if defined(ATOMIC_PLATFORM_WINDOWS) || defined (ATOMIC_PLATFORM_LINUX)
#ifdef ATOMIC_WEBVIEW

    int exit_code = Atomic::WebMain(argc, argv);

    if (exit_code >= 0)
    {
        // The sub-process has completed so return here.
        return exit_code;
    }

#endif
#endif

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
