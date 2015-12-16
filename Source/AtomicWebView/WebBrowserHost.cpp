
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

#ifdef ATOMIC_PLATFORM_OSX
void* GetNSWindowContentView(void* window);
#endif

class WebBrowserHostPrivate : public CefLifeSpanHandler
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

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE
    {
        CEF_REQUIRE_UI_THREAD();
        browsers_.Push(browser);
    }

    virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE
    {
        return false;
    }

    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE
    {
        CEF_REQUIRE_UI_THREAD();

        // Remove from the list of existing browsers.
        Vector<CefRefPtr<CefBrowser>>::Iterator itr = browsers_.Begin();
        while (itr != browsers_.End())
        {
            if ((*itr)->IsSame(browser))
            {
                browsers_.Erase(itr);
                break;
            }

            itr++;
        }
    }

    void CloseAllBrowsers(bool force_close)
    {
        if (!CefCurrentlyOn(TID_UI))
        {
            // Execute on the UI thread.
            CefPostTask(TID_UI,
                        base::Bind(&WebBrowserHostPrivate::CloseAllBrowsers, this, force_close));

            return;
        }

        if (!browsers_.Size())
            return;

        // make a copy of vector, as we'll be erasing as we go
        Vector<CefRefPtr<CefBrowser>> browsers = browsers_;

        Vector<CefRefPtr<CefBrowser>>::Iterator itr = browsers.Begin();

        while (itr != browsers.End())
        {
            (*itr)->GetHost()->CloseBrowser(force_close);
            itr++;
        }

        browsers_.Clear();
    }

    IMPLEMENT_REFCOUNTING(WebBrowserHostPrivate);

private:

    Vector<CefRefPtr<CefBrowser>> browsers_;

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
    d_->AddRef();

    SubscribeToEvent(E_BEGINFRAME, HANDLER(WebBrowserHost, HandleBeginFrame));

}

WebBrowserHost::~WebBrowserHost()
{
    d_->CloseAllBrowsers(true);
    d_->Release();
    CefShutdown();
}

CefLifeSpanHandler* WebBrowserHost::GetCefLifeSpanHandler()
{
    return d_;
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
