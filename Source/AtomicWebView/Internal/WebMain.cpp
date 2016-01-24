
#include <include/cef_app.h>
#include "WebAppOther.h"
#include "WebAppRenderer.h"

namespace Atomic
{

using namespace Atomic;

int WebMain(int argc, char* argv[])
{
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();

    // Provide CEF with command-line arguments.
#ifdef ATOMIC_PLATFORM_WINDOWS
    CefMainArgs main_args;
    command_line->InitFromString(::GetCommandLineW());
#else
    CefMainArgs main_args(argc, argv);
    command_line->InitFromArgv(argc, argv);
#endif

    // Create a ClientApp of the correct type.
    CefRefPtr<CefApp> app;
    WebApp::ProcessType process_type = WebApp::GetProcessType(command_line);
    if (process_type == WebApp::RendererProcess)
    {
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
