
#include <ThirdParty/CEF/include/cef_app.h>
#include <ThirdParty/CEF/include/cef_client.h>
#include <ThirdParty/CEF/include/cef_render_handler.h>

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>

#include "WebBrowserHost.h"

namespace Atomic
{

WebBrowserHost::WebBrowserHost(Context* context) : Object (context)
{
    const Vector<String>& arguments = GetArguments();

    const char** _argv { 0 };

    PODVector<const char*> argv;
    for (unsigned i = 0; i < arguments.Size(); i++)
        argv.Push(arguments[i].CString());

    CefMainArgs args(arguments.Size(), arguments.Size() ? (char**) &argv[0] : (char **) _argv);

    // TODO: We're losing the main system menu items on OSX and cmd-q no longer works
    // when we init CEF

    int result = CefExecuteProcess(args, nullptr, nullptr);

    if (result >= 0)
    {
        LOGERROR("CEFExecuteProcess - Error");
    }

    CefSettings settings;

    if (!CefInitialize(args, settings, nullptr, nullptr))
    {
        LOGERROR("CefInitialize - Error");
    }

}

WebBrowserHost::~WebBrowserHost()
{

}

}
