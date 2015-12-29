
#include <SDL/include/SDL.h>
#include <ThirdParty/SDL/include/SDL_syswm.h>

#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_browser.h>
#include <include/wrapper/cef_helpers.h>
#include <include/base/cef_bind.h>
#include <include/wrapper/cef_closure_task.h>

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Input/Input.h>

#include <Atomic/Graphics/Graphics.h>

#include "WebBrowserHost.h"
#include "WebClient.h"
#include "WebKeyboard.h"

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
#ifdef ATOMIC_PLATFORM_OSX
            NSView* view = (NSView*) GetNSWindowContentView(info.info.cocoa.window);
            windowInfo.SetAsWindowless(view, false);
#endif

#ifdef ATOMIC_PLATFORM_WINDOWS
         windowInfo.SetAsWindowless(info.info.win.window, false);
#endif

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

    Input* input = GetSubsystem<Input>();

    if (input->GetMouseButtonDown(MOUSEB_LEFT))
        mevent.modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    if (input->GetMouseButtonDown(MOUSEB_MIDDLE))
        mevent.modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
    if (input->GetMouseButtonDown(MOUSEB_RIGHT))
        mevent.modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

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

/*

  EVENTFLAG_CAPS_LOCK_ON        = 1 << 0,
  EVENTFLAG_SHIFT_DOWN          = 1 << 1,
  EVENTFLAG_CONTROL_DOWN        = 1 << 2,
  EVENTFLAG_ALT_DOWN            = 1 << 3,
  EVENTFLAG_LEFT_MOUSE_BUTTON   = 1 << 4,
  EVENTFLAG_MIDDLE_MOUSE_BUTTON = 1 << 5,
  EVENTFLAG_RIGHT_MOUSE_BUTTON  = 1 << 6,
  // Mac OS-X command key.
  EVENTFLAG_COMMAND_DOWN        = 1 << 7,
  EVENTFLAG_NUM_LOCK_ON         = 1 << 8,
  EVENTFLAG_IS_KEY_PAD          = 1 << 9,
  EVENTFLAG_IS_LEFT             = 1 << 10,
  EVENTFLAG_IS_RIGHT            = 1 << 11,
} cef_event_flags_t;

*/
void WebClient::SendKeyEvent(const StringHash eventType, VariantMap& eventData)
{
    if (!d_->browser_.get())
        return;

    CefRefPtr<CefBrowserHost> host = d_->browser_->GetHost();

    CefKeyEvent keyEvent;

    if (!ConvertKeyEvent(eventType, eventData, keyEvent))
        return;

    host->SendKeyEvent(keyEvent);

#ifdef ATOMIC_PLATFORM_OSX
    // Send an empty key event on OSX, which seems to fix
    // keyboard problems on OSX with cefclient
    // ./cefclient --off-screen-rendering-enabled
    // return does not work at all on cef client with offscreen
    // bad interaction with arrow keys (for example here, after
    // hitting arrow keys, return/text takes a couple presses to register
    keyEvent.type = keyUp ? KEYEVENT_KEYUP : KEYEVENT_KEYDOWN;
    keyEvent.modifiers = 0;
    keyEvent.native_key_code = 0;
    host->SendKeyEvent(keyEvent);
#endif


}

void WebClient::SendTextInputEvent(const StringHash eventType, VariantMap& eventData)
{
    if (!d_->browser_.get())
        return;

    CefRefPtr<CefBrowserHost> host = d_->browser_->GetHost();

    CefKeyEvent keyEvent;

    if (!ConvertTextInputEvent(eventType, eventData, keyEvent))
        return;

    host->SendKeyEvent(keyEvent);
}

void WebClient::SendFocusEvent(bool focus)
{
    if (!d_->browser_.get())
        return;

    CefRefPtr<CefBrowserHost> host = d_->browser_->GetHost();
    host->SendFocusEvent(focus);
}

void WebClient::ShortcutCut()
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GetFocusedFrame()->Cut();

}


void WebClient::ShortcutCopy()
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GetFocusedFrame()->Copy();

}

void WebClient::ShortcutPaste()
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GetFocusedFrame()->Paste();

}

void WebClient::ShortcutSelectAll()
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GetFocusedFrame()->SelectAll();

}

void WebClient::ShortcutUndo()
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GetFocusedFrame()->Undo();

}

void WebClient::ShortcutRedo()
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GetFocusedFrame()->Redo();

}

void WebClient::ShortcutDelete()
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GetFocusedFrame()->Delete();

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
