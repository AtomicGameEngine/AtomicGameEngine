
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
