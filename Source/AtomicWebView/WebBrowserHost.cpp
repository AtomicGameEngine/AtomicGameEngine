
#include <list>

#include <SDL/include/SDL.h>
#include <ThirdParty/SDL/include/SDL_syswm.h>

#include <ThirdParty/CEF/include/cef_app.h>
#include <ThirdParty/CEF/include/cef_client.h>
#include <ThirdParty/CEF/include/wrapper/cef_helpers.h>
#include <ThirdParty/CEF/include/base/cef_bind.h>
#include <ThirdParty/CEF/include/wrapper/cef_closure_task.h>


#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IO/Log.h>

#include <Atomic/Graphics/Graphics.h>

#include "WebClient.h"
#include "WebBrowserHost.h"

namespace Atomic
{

class WebBrowserHostPrivate
{
    friend class WebBrowserHost;

public:

    WebBrowserHostPrivate(WebBrowserHost* host)
    {

        host_ = host;
    }

    virtual ~WebBrowserHostPrivate()
    {
        host_ = 0;
    }

private:

    WeakPtr<WebBrowserHost> host_;

};

WebBrowserHost::WebBrowserHost(Context* context) : Object (context)
{
    const Vector<String>& arguments = GetArguments();

    const char** _argv { 0 };

    PODVector<const char*> argv;
    for (unsigned i = 0; i < arguments.Size(); i++)
        argv.Push(arguments[i].CString());

    CefMainArgs args(arguments.Size(), arguments.Size() ? (char**) &argv[0] : (char **) _argv);

    int result = CefExecuteProcess(args, nullptr, nullptr);

    if (result >= 0)
    {
        LOGERROR("CEFExecuteProcess - Error");
    }

    CefSettings settings;

    settings.windowless_rendering_enabled = true;

    // If losing OSX system menu, it means we're calling this
    // before initializing graphics subsystem
    if (!CefInitialize(args, settings, nullptr, nullptr))
    {
        LOGERROR("CefInitialize - Error");
    }

    d_ = new WebBrowserHostPrivate(this);

    SubscribeToEvent(E_BEGINFRAME, HANDLER(WebBrowserHost, HandleBeginFrame));

}

WebBrowserHost::~WebBrowserHost()
{
    CefShutdown();
}

void WebBrowserHost::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
{
    CefDoMessageLoopWork();
}

}
