
#include <SDL/include/SDL.h>
#include <ThirdParty/SDL/include/SDL_syswm.h>

#include <ThirdParty/CEF/include/cef_app.h>
#include <ThirdParty/CEF/include/cef_client.h>
#include <ThirdParty/CEF/include/cef_browser.h>
#include <ThirdParty/CEF/include/wrapper/cef_helpers.h>
#include <ThirdParty/CEF/include/base/cef_bind.h>
#include <ThirdParty/CEF/include/wrapper/cef_closure_task.h>


#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IO/Log.h>

#include <Atomic/Graphics/Graphics.h>

#include "WebBrowserHost.h"
#include "WebClient.h"

namespace Atomic
{

#ifdef ATOMIC_PLATFORM_OSX
void* GetNSWindowContentView(void* window);
#endif

class WebClientPrivate : public CefClient, public CefLifeSpanHandler
{
    friend class WebClient;

public:

    WebClientPrivate(WebClient* client)
    {

        webClient_ = client;
        webBrowserHost_ = webClient_->GetSubsystem<WebBrowserHost>();
    }

    CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE
    {

        if (webClient_->renderHandler_.Null())
            return nullptr;

        return webClient_->renderHandler_->GetCEFRenderHandler();

    }

    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE
    {
        return this;
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) OVERRIDE
    {
        return false;
    }

    bool CreateBrowser()
    {
        CefWindowInfo windowInfo;
        CefBrowserSettings browserSettings;

        Graphics* graphics = webClient_->GetSubsystem<Graphics>();

        SDL_Window* sdlWindow = static_cast<SDL_Window*>(graphics->GetSDLWindow());
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);

        if(SDL_GetWindowWMInfo(sdlWindow, &info))
        {
            NSView* view = (NSView*) GetNSWindowContentView(info.info.cocoa.window);
            windowInfo.SetAsWindowless(view, false);

            CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(windowInfo, this,
                                                                              "https://youtube.com/", browserSettings, nullptr);

            if (!browser.get())
                return false;

            browsers_.Push(browser);

            return true;
        }

        return false;

    }

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE
    {
        CEF_REQUIRE_UI_THREAD();
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
                        base::Bind(&WebClientPrivate::CloseAllBrowsers, this, force_close));

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

    IMPLEMENT_REFCOUNTING(WebClientPrivate);

private:

    Vector<CefRefPtr<CefBrowser>> browsers_;
    WeakPtr<WebBrowserHost> webBrowserHost_;
    WeakPtr<WebClient> webClient_;

};


WebClient::WebClient(Context* context) : Object(context)
{
    d_ = new WebClientPrivate(this);
}

WebClient::~WebClient()
{
    renderHandler_ = 0;
    //d_->Release();
}

void WebClient::SendMouseClickEvent(int x, int y, unsigned button, bool mouseUp, unsigned modifier) const
{
    if (!d_->browsers_.Size())
        return;

    CefRefPtr<CefBrowser> browser = d_->browsers_[0];
    CefRefPtr<CefBrowserHost> host = browser->GetHost();

    CefMouseEvent mevent;
    mevent.x = x;
    mevent.y = y;
    mevent.modifiers = 0;

    //MBT_LEFT   = 0,
    //MBT_MIDDLE,
    //MBT_RIGHT,

    host->SendMouseClickEvent(mevent, (CefBrowserHost::MouseButtonType) button, mouseUp, 1);

}

void WebClient::SendMouseMoveEvent(int x, int y, unsigned modifier, bool mouseLeave) const
{
    if (!d_->browsers_.Size())
        return;

    CefRefPtr<CefBrowser> browser = d_->browsers_[0];
    CefRefPtr<CefBrowserHost> host = browser->GetHost();

    CefMouseEvent mevent;
    mevent.x = x;
    mevent.y = y;
    mevent.modifiers = 0;

    //MBT_LEFT   = 0,
    //MBT_MIDDLE,
    //MBT_RIGHT,

    host->SendMouseMoveEvent(mevent, mouseLeave);

}

void WebClient::WasResized()
{
    if (!d_->browsers_.Size())
        return;

    d_->browsers_[0]->GetHost()->WasResized();;
}

bool WebClient::CreateBrowser()
{
    return d_->CreateBrowser();
}

void WebClient::SetWebRenderHandler(WebRenderHandler* handler)
{
    handler->SetWebClient(this);
    renderHandler_ = handler;
}

CefClient* WebClient::GetCefClient()
{
    return d_;
}


}
