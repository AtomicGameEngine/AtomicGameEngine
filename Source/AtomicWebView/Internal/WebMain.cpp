//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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

#include <include/cef_app.h>
#include "WebAppOther.h"
#include "WebAppRenderer.h"

namespace Atomic
{

int WebMain(int argc, char* argv[])
{

    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();

    // Provide CEF with command-line arguments.
#ifdef ATOMIC_PLATFORM_WINDOWS
    CefMainArgs main_args;
    command_line->InitFromString(::GetCommandLineW());
#elif ATOMIC_PLATFORM_OSX
    CefMainArgs main_args(argc, argv);
    command_line->InitFromArgv(argc, argv);
#else
    CefMainArgs main_args(argc, argv);
    command_line->InitFromArgv(argc, argv);
#endif

    // Create a ClientApp of the correct type.
    CefRefPtr<CefApp> app;
    WebApp::ProcessType process_type = WebApp::GetProcessType(command_line);
    if (process_type == WebApp::RendererProcess || process_type == WebApp::ZygoteProcess)
    {
        // On Linux the zygote process is used to spawn other process types. Since
        // we don't know what type of process it will be give it the renderer
        // client.

        app = new WebAppRenderer();
    }
    else if (process_type == WebApp::OtherProcess)
    {
        app = new WebAppOther();
    }

    // Execute the secondary process.
    return CefExecuteProcess(main_args, app, NULL);
}


}
