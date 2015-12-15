
#include <SDL/include/SDL.h>
#include <ThirdParty/SDL/include/SDL_syswm.h>

#include <ThirdParty/CEF/include/cef_app.h>
#include <ThirdParty/CEF/include/cef_client.h>
#include <ThirdParty/CEF/include/cef_render_handler.h>
#include <ThirdParty/CEF/include/wrapper/cef_helpers.h>

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IO/Log.h>

#include <Atomic/Graphics/Graphics.h>

#include "WebClient.h"
#include "WebBrowserHost.h"

class SimpleApp : public CefApp,
        public CefBrowserProcessHandler {
public:
    SimpleApp();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
    OVERRIDE { return this; }

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() OVERRIDE;

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SimpleApp);
};

SimpleApp::SimpleApp() {
}

void SimpleApp::OnContextInitialized() {

    CEF_REQUIRE_UI_THREAD();

}

namespace Atomic
{

#ifdef ATOMIC_PLATFORM_OSX
void* GetNSWindowContentView(void* window);
#endif

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

    SubscribeToEvent(E_BEGINFRAME, HANDLER(WebBrowserHost, HandleBeginFrame));

}

WebBrowserHost::~WebBrowserHost()
{
    CefShutdown();
}

bool WebBrowserHost::CreateBrowser(WebClient* webClient)
{
    CefWindowInfo windowInfo;
    CefBrowserSettings browserSettings;

    Graphics* graphics = GetSubsystem<Graphics>();

    SDL_Window* sdlWindow = static_cast<SDL_Window*>(graphics->GetSDLWindow());
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);

    if(SDL_GetWindowWMInfo(sdlWindow, &info))
    {
        NSView* view = (NSView*) GetNSWindowContentView(info.info.cocoa.window);
        windowInfo.SetAsWindowless(view, false);

        return CefBrowserHost::CreateBrowser(windowInfo, (CefClient*) webClient->d_,
                                             "https://html5test.com/", browserSettings, nullptr);
    }

    return false;

}

void WebBrowserHost::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
{
    CefDoMessageLoopWork();
}

}
