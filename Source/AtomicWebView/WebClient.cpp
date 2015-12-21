
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

    bool CreateBrowser(const String& initialURL, int width, int height)
    {
        if (webClient_->renderHandler_.Null())
        {
            LOGERROR("WebClient::CreateBrowser - No render handler specified");
            return false;
        }

        CefWindowInfo windowInfo;
        CefBrowserSettings browserSettings;

        //browserSettings.webgl = STATE_ENABLED;

        windowInfo.width = width;
        windowInfo.height = height;

        Graphics* graphics = webClient_->GetSubsystem<Graphics>();

        SDL_Window* sdlWindow = static_cast<SDL_Window*>(graphics->GetSDLWindow());
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);

        if(SDL_GetWindowWMInfo(sdlWindow, &info))
        {
            NSView* view = (NSView*) GetNSWindowContentView(info.info.cocoa.window);
            windowInfo.SetAsWindowless(view, false);

            webClient_->renderHandler_->SetSize(width, height);
            CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(windowInfo, this,
                                                                              initialURL.CString(), browserSettings, nullptr);

            if (!browser.get())
                return false;

            browser_ = browser;

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

        if (browser->IsSame(browser_))
            browser_ = nullptr;

    }

    void CloseBrowser(bool force_close)
    {
        if (!CefCurrentlyOn(TID_UI))
        {
            // Execute on the UI thread.
            CefPostTask(TID_UI,
                        base::Bind(&WebClientPrivate::CloseBrowser, this, force_close));

            return;
        }

        if (!browser_.get())
            return;

        browser_->GetHost()->CloseBrowser(force_close);
    }

    IMPLEMENT_REFCOUNTING(WebClientPrivate);

private:

    CefRefPtr<CefBrowser> browser_;
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
    if (!d_->browser_.get())
        return;

    CefRefPtr<CefBrowserHost> host = d_->browser_->GetHost();

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
    if (!d_->browser_.get())
        return;

    CefRefPtr<CefBrowserHost> host = d_->browser_->GetHost();

    CefMouseEvent mevent;
    mevent.x = x;
    mevent.y = y;
    mevent.modifiers = 0;

    //MBT_LEFT   = 0,
    //MBT_MIDDLE,
    //MBT_RIGHT,

    host->SendMouseMoveEvent(mevent, mouseLeave);

}

void WebClient::SendMouseWheelEvent(int x, int y, unsigned modifier,int deltaX, int deltaY) const
{
    if (!d_->browser_.get())
        return;

    CefRefPtr<CefBrowserHost> host = d_->browser_->GetHost();

    CefMouseEvent mevent;
    mevent.x = x;
    mevent.y = y;
    mevent.modifiers = 0;

#ifdef ATOMIC_PLATFORM_OSX
    deltaY = -deltaY;
#endif

    host->SendMouseWheelEvent(mevent, deltaX, deltaY * 5);

}



void WebClient::WasResized()
{
    if (!d_->browser_.get())
        return;

    CefRefPtr<CefBrowserHost> host = d_->browser_->GetHost();
    host->WasResized();;
}

bool WebClient::CreateBrowser(const String& initialURL, int width, int height)
{
    return d_->CreateBrowser(initialURL, width, height);
}

void WebClient::SetSize(int width, int height)
{
    if (renderHandler_.Null())
        return;

    if (renderHandler_->GetWidth() == width && renderHandler_->GetHeight() == height)
        return;

    renderHandler_->SetSize(width, height);

    WasResized();

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
