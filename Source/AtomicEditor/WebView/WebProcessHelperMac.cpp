// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include <AtomicWebView/Internal/WebAppOther.h>
#include <AtomicWebView/Internal/WebAppRenderer.h>

using namespace Atomic;

#include "include/cef_app.h"

// Entry point function for sub-processes.
int main(int argc, char* argv[])
{
    // Provide CEF with command-line arguments.
    CefMainArgs main_args(argc, argv);

    // Parse command-line arguments.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromArgv(argc, argv);

    // Create a ClientApp of the correct type.
    CefRefPtr<CefApp> app;
    WebApp::ProcessType process_type = WebApp::GetProcessType(command_line);
    if (process_type == WebApp::RendererProcess)
        app = new WebAppRenderer();
    else if (process_type == WebApp::OtherProcess)
        app = new WebAppOther();

    // Execute the secondary process.
    return CefExecuteProcess(main_args, app, NULL);
}
