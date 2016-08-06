//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifdef ATOMIC_PLATFORM_WINDOWS
#include <windows.h>
#undef LoadString
#endif

#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_browser.h>
#include <include/wrapper/cef_helpers.h>
#include <include/base/cef_bind.h>
#include <include/wrapper/cef_closure_task.h>
#include "include/wrapper/cef_message_router.h"

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Input/Input.h>

#include <Atomic/Graphics/Graphics.h>

#include "Internal/WebAppBrowser.h"

#include "WebBrowserHost.h"
#include "WebMessageHandler.h"
#include "WebClient.h"
#include "WebKeyboard.h"
#include "WebViewEvents.h"
#include "WebString.h"

#include <SDL/include/SDL.h>

#ifdef ATOMIC_PLATFORM_LINUX
  #ifndef SDL_VIDEO_DRIVER_X11
  #define SDL_VIDEO_DRIVER_X11
  #endif
#endif

#include <ThirdParty/SDL/include/SDL_syswm.h>

namespace Atomic
{

#ifdef ATOMIC_PLATFORM_OSX
void* GetNSWindowContentView(void* window);
#endif

class WebClientPrivate : public CefClient,
        public CefLifeSpanHandler,
        public CefLoadHandler,
        public CefDisplayHandler,
        public CefRequestHandler,
        public CefKeyboardHandler
{
    friend class WebClient;

public:

    WebClientPrivate(WebClient* client)
    {

        webClient_ = client;
        webBrowserHost_ = webClient_->GetSubsystem<WebBrowserHost>();

        CefMessageRouterConfig config;
        config.js_query_function = WebBrowserHost::GetJSMessageQueryFunctionName().CString();
        config.js_cancel_function = WebBrowserHost::GetJSMessageQueryCancelFunctionName().CString();
        browserSideRouter_ = CefMessageRouterBrowserSide::Create(config);

    }

    virtual ~WebClientPrivate()
    {
    }

    CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE
    {

        if (webClient_.Null() || webClient_->renderHandler_.Null())
            return nullptr;

        return webClient_->renderHandler_->GetCEFRenderHandler();

    }

    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE
    {
        return this;
    }

    CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE
    {
        return this;
    }

    CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE
    {
        return this;
    }

    CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE
    {
        return this;
    }

    CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE
    {
        return this;
    }


    // CefKeyboardHandler

    virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                               const CefKeyEvent& event,
                               CefEventHandle os_event,
                               bool* is_keyboard_shortcut) OVERRIDE
    {
        return false;
    }



    // CefRequestHandler methods

    void OnRenderViewReady(CefRefPtr<CefBrowser> browser) OVERRIDE
    {
    }

    bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefRequest> request,
                        bool is_redirect) OVERRIDE
    {
        CEF_REQUIRE_UI_THREAD();

        browserSideRouter_->OnBeforeBrowse(browser, frame);

        return false;

    }

    bool OnProcessMessageReceived(
            CefRefPtr<CefBrowser> browser,
            CefProcessId source_process,
            CefRefPtr<CefProcessMessage> message) OVERRIDE
    {

        CEF_REQUIRE_UI_THREAD();

        const CefString& message_name = message->GetName();

        if (message_name == "atomic_eval_javascript_result")
        {
            if (webClient_.Null())
                return false;

            unsigned evalID = (unsigned) message->GetArgumentList()->GetInt(0);
            bool result = message->GetArgumentList()->GetBool(1);
            String value;
            ConvertCEFString(message->GetArgumentList()->GetString(2), value);

            webClient_->EvalJavaScriptResult(evalID, result, value);

            return true;
        }


        if (browserSideRouter_->OnProcessMessageReceived(browser, source_process, message))
        {
            return true;
        }

        return false;
    }


    void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                   TerminationStatus status) OVERRIDE
    {
        CEF_REQUIRE_UI_THREAD();
        browserSideRouter_->OnRenderProcessTerminated(browser);
    }

    // CefLoadHandler

    void OnLoadStart(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame, TransitionType transition_type) OVERRIDE
    {
        if (webClient_.Null() || !frame->IsMain())
            return;

        VariantMap eventData;
        eventData[WebViewLoadStart::P_CLIENT] = webClient_;

        CefString cefURL = frame->GetURL();
        String url;
        ConvertCEFString(cefURL, url);
        eventData[WebViewLoadStart::P_URL] = url;

        webClient_->SendEvent(E_WEBVIEWLOADSTART, eventData);

    }

    void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   int httpStatusCode) OVERRIDE
    {
        if (webClient_.Null() || !frame->IsMain())
            return;

        VariantMap eventData;
        eventData[WebViewLoadEnd::P_CLIENT] = webClient_;

        CefString cefURL = frame->GetURL();
        String url;
        ConvertCEFString(cefURL, url);
        eventData[WebViewLoadEnd::P_URL] = url;

        webClient_->SendEvent(E_WEBVIEWLOADEND, eventData);

    }

    void OnLoadError(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     ErrorCode errorCode,
                     const CefString& errorText,
                     const CefString& failedUrl) OVERRIDE
    {
        if (webClient_.Null())
            return;

    }

    void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                              bool isLoading,
                              bool canGoBack,
                              bool canGoForward) OVERRIDE
    {

        if (webClient_.Null())
            return;

        VariantMap eventData;
        eventData[WebViewLoadStateChange::P_CLIENT] = webClient_;
        eventData[WebViewLoadStateChange::P_LOADING] = isLoading;
        eventData[WebViewLoadStateChange::P_CANGOBACK] = canGoBack;
        eventData[WebViewLoadStateChange::P_CANGOFORWARD] = canGoForward;

        webClient_->SendEvent(E_WEBVIEWLOADSTATECHANGE, eventData);

    }

    // CefDisplayHandler

    void OnAddressChange(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         const CefString& url) OVERRIDE
    {
        if (webClient_.Null() || !frame->IsMain())
            return;

        VariantMap eventData;
        eventData[WebViewAddressChange::P_CLIENT] = webClient_;

        String _url;
        ConvertCEFString(url, _url);
        eventData[WebViewAddressChange::P_URL] = _url;

        webClient_->SendEvent(E_WEBVIEWADDRESSCHANGE, eventData);

    }

    void OnTitleChange(CefRefPtr<CefBrowser> browser,
                       const CefString& title) OVERRIDE
    {
        if (webClient_.Null())
            return;

        VariantMap eventData;
        eventData[WebViewTitleChange::P_CLIENT] = webClient_;

        String _title;
        ConvertCEFString(title, _title);
        eventData[WebViewTitleChange::P_TITLE] = _title;

        webClient_->SendEvent(E_WEBVIEWTITLECHANGE, eventData);

    }

    ///
    // Called to display a console message. Return true to stop the message from
    // being output to the console.
    ///
    /*--cef(optional_param=message,optional_param=source)--*/
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                  const CefString& message,
                                  const CefString& source,
                                  int line) OVERRIDE
    {
        if (webClient_.Null())
            return false;

        String _message;
        ConvertCEFString(message, _message);
        String _source;
        ConvertCEFString(source, _source);

        ATOMIC_LOGINFOF("WebViewJS: %s (%s:%i)", _message.CString(), _source.CString(), line);

        return false;
    }

    bool CreateBrowser(const String& initialURL, int width, int height)
    {
        if (browser_.get())
        {
            ATOMIC_LOGERROR("WebClient::CreateBrowser - Browser already created");
            return false;
        }

        if (webClient_->renderHandler_.Null())
        {
            ATOMIC_LOGERROR("WebClient::CreateBrowser - No render handler specified");
            return false;
        }

        CefWindowInfo windowInfo;
        CefBrowserSettings browserSettings;

        browserSettings.webgl = STATE_ENABLED;
        browserSettings.file_access_from_file_urls = STATE_ENABLED;
        browserSettings.universal_access_from_file_urls = STATE_ENABLED;
        browserSettings.web_security = WebBrowserHost::GetWebSecurity() ? STATE_ENABLED : STATE_DISABLED;

        windowInfo.width = width;
        windowInfo.height = height;        
        windowInfo.transparent_painting_enabled = 1;

        Graphics* graphics = webClient_->GetSubsystem<Graphics>();

        if (graphics)
        {
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
                windowInfo.SetAsWindowless(info.info.win.window, /*transparent*/ true);
#endif
#ifdef ATOMIC_PLATFORM_LINUX
                if ( info.subsystem == SDL_SYSWM_X11 )
                    windowInfo.SetAsWindowless(info.info.x11.window, true);
#endif
            }

        }
        else
        {
#ifndef ATOMIC_PLATFORM_LINUX
            // headless
            windowInfo.SetAsWindowless(nullptr, true);
#endif
        }

        // TODO: There seems to be a CEF bug when loading a string into a browser
        // which was created with an empty URL, this workaround gets things going
        // NOTE: I also tried loading the string, delaying 5 seconds and still won't
        // load a string until a URL has been passed into the view
        String _initialURL = initialLoadString_.Length() ? "x" : initialURL;

        webClient_->renderHandler_->SetSize(width, height);
        CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(windowInfo, this,
                                                                          _initialURL.CString(), browserSettings, nullptr);

        if (!browser.get())
            return false;

        browser_ = browser;

        if (initialLoadString_.Length())
        {
            webClient_->LoadString(initialLoadString_, initialLoadStringURL_);
        }

        return true;


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

        browser_ = nullptr;

    }

    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               const CefString& target_url,
                               const CefString& target_frame_name,
                               CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                               bool user_gesture,
                               const CefPopupFeatures& popupFeatures,
                               CefWindowInfo& windowInfo,
                               CefRefPtr<CefClient>& client,
                               CefBrowserSettings& settings,
                               bool* no_javascript_access)  OVERRIDE
    {
        // Called on the IO thread, cancel and convert to popup request

        assert(!CefCurrentlyOn(TID_UI));

        // Execute on the UI thread.
        CefPostTask(TID_UI,
                    base::Bind(&WebClientPrivate::OnPopupRequest, this, target_url));

        return true;
    }

    void OnPopupRequest(const CefString& target_url)
    {
        if (webClient_.Null())
            return;

        String url;
        ConvertCEFString(target_url, url);

        VariantMap eventData;
        eventData[WebViewPopupRequest::P_CLIENT] = webClient_;
        eventData[WebViewPopupRequest::P_URL] = url;

        webClient_->SendEvent(E_WEBVIEWPOPUPREQUEST, eventData);

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

    void SetInitialLoadString(const String& loadString, const String& url)
    {
        initialLoadString_ = loadString;
        initialLoadStringURL_ = url;
    }

    IMPLEMENT_REFCOUNTING(WebClientPrivate);

    void ClearReferences()
    {
        browser_ = nullptr;
        webBrowserHost_ = nullptr;
        webClient_ = nullptr;
        browserSideRouter_ = nullptr;
    }

private:

    String initialLoadString_;
    String initialLoadStringURL_;

    CefRefPtr<CefBrowser> browser_;
    WeakPtr<WebBrowserHost> webBrowserHost_;
    WeakPtr<WebClient> webClient_;
    CefRefPtr<CefMessageRouterBrowserSide> browserSideRouter_;

};


WebClient::WebClient(Context* context) : Object(context)
{
    d_ = new WebClientPrivate(this);
    d_->AddRef();

    SubscribeToEvent(E_WEBVIEWGLOBALPROPERTIESCHANGED, ATOMIC_HANDLER(WebClient, HandleWebViewGlobalPropertiesChanged));
}

WebClient::~WebClient()
{
    if (d_)
    {
        List<SharedPtr<WebMessageHandler>>::Iterator itr = messageHandlers_.Begin();
        while (itr != messageHandlers_.End())
        {
            CefMessageRouterBrowserSide::Handler* handler = static_cast<CefMessageRouterBrowserSide::Handler*>((*itr)->GetCefHandler());
            d_->browserSideRouter_->RemoveHandler(handler);
            itr++;
        }

        d_->CloseBrowser(true);
        d_->ClearReferences();
        d_->Release();
    }

    d_ = nullptr;
    renderHandler_ = 0;    
}

void WebClient::SendMouseClickEvent(int x, int y, unsigned button, bool mouseUp, unsigned modifier, int clickCount) const
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

    host->SendMouseClickEvent(mevent, (CefBrowserHost::MouseButtonType) button, mouseUp, clickCount);

}

void WebClient::SendMousePressEvent(int x, int y, unsigned button, unsigned modifier, int clickCount) const
{
    SendMouseClickEvent(x, y, button, false, modifier, clickCount);
    SendMouseClickEvent(x, y, button, true, modifier, clickCount);
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

    deltaY = -deltaY * 5;

#ifndef ATOMIC_PLATFORM_OSX
    deltaY *= 5;
#endif

    host->SendMouseWheelEvent(mevent, deltaX, deltaY);

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

    if (!ConvertKeyEvent(GetSubsystem<Input>(), eventType, eventData, keyEvent))
        return;

    host->SendKeyEvent(keyEvent);

#ifdef ATOMIC_PLATFORM_WINDOWS

    // RETURN KEY: We need to send both keydown and char for return key
    // this allows it to be used both to confirm entry on popups,
    // while also being used for text input
    if (keyEvent.windows_key_code == 13)
    {
        keyEvent.type = KEYEVENT_CHAR;
        host->SendKeyEvent(keyEvent);
    }

#endif

#ifdef ATOMIC_PLATFORM_OSX

    // RETURN KEY: We need to send both keydown and char for return key
    // this allows it to be used both to confirm entry on popups,
    // while also being used for text input
    if (keyEvent.native_key_code == 36)
    {
        keyEvent.type = KEYEVENT_CHAR;
        host->SendKeyEvent(keyEvent);
    }

    // Send an empty key event on OSX, which seems to fix
    // keyboard problems on OSX with cefclient
    // ./cefclient --off-screen-rendering-enabled
    // return does not work at all on cef client with offscreen
    // bad interaction with arrow keys (for example here, after
    // hitting arrow keys, return/text takes a couple presses to register

    memset((void*)&keyEvent, 0, sizeof(keyEvent));

    keyEvent.type = KEYEVENT_KEYDOWN;
    keyEvent.modifiers = 0;
    keyEvent.native_key_code = 0;
    host->SendKeyEvent(keyEvent);
#endif

#ifdef ATOMIC_PLATFORM_LINUX

    if (keyEvent.windows_key_code == 0x0D)
    {
        keyEvent.type = KEYEVENT_CHAR;
        host->SendKeyEvent(keyEvent);
    }

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

// Javascript
void WebClient::ExecuteJavaScript(const String& script)
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GetMainFrame()->ExecuteJavaScript(CefString(script.CString()), "", 0);
}

void WebClient::EvalJavaScript(unsigned evalID, const String& script)
{
    if (!d_->browser_.get())
        return;

    // Create the message object.
    CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("atomic_eval_javascript");

    // Retrieve the argument list object.
    CefRefPtr<CefListValue> args = msg->GetArgumentList();

    // Populate the argument values.
    args->SetInt(0, (int) evalID);
    args->SetString(1, CefString(script.CString()));

    // Send the process message to the render process.
    // Use PID_BROWSER instead when sending a message to the browser process.
    d_->browser_->SendProcessMessage(PID_RENDERER, msg);
}

void WebClient::EvalJavaScriptResult(unsigned evalID, bool result, const String& value)
{
    using namespace WebViewJSEvalResult;

    VariantMap eventData;
    eventData[P_CLIENT] = this;
    eventData[P_EVALID] = evalID;
    eventData[P_RESULT] = result;
    eventData[P_VALUE] = value;

    SendEvent(E_WEBVIEWJSEVALRESULT, eventData);
}

void WebClient::AddMessageHandler(WebMessageHandler* handler, bool first)
{
    SharedPtr<WebMessageHandler> _handler(handler);

    if (handler->GetWebClient())
    {
        ATOMIC_LOGWARNING("WebClient::AddMessageHandler - message handler already added to another client");
        return;
    }

    if (messageHandlers_.Contains(_handler))
    {
        ATOMIC_LOGWARNING("WebClient::AddMessageHandler - message handler already added to this client");
        return;
    }

    _handler->SetWebClient(this);
    messageHandlers_.Push(_handler);
    d_->browserSideRouter_->AddHandler(static_cast<CefMessageRouterBrowserSide::Handler*>(handler->GetCefHandler()), first);

}

void WebClient::RemoveMessageHandler(WebMessageHandler* handler)
{

    SharedPtr<WebMessageHandler> _handler(handler);

    List<SharedPtr<WebMessageHandler>>::Iterator itr = messageHandlers_.Find(_handler);

    if (itr == messageHandlers_.End())
    {
        ATOMIC_LOGWARNING("WebClient::RemoveMessageHandler - message handler not found");
        return;
    }

    d_->browserSideRouter_->RemoveHandler(static_cast<CefMessageRouterBrowserSide::Handler*>(handler->GetCefHandler()));
    messageHandlers_.Erase(itr);
}

// Navigation

void WebClient::LoadURL(const String& url)
{
    if (!d_->browser_.get())
    {
        return;
    }

    CefString _url(url.CString());
    d_->browser_->GetMainFrame()->LoadURL(_url);

}

void WebClient::LoadString(const String& source, const String& url)
{
    if (!d_->browser_.get())
    {
        d_->SetInitialLoadString(source, url);
        return;
    }

    // We need to make sure global properties are updated when loading web content from source string
    // This is handled differently internally then we requests
    UpdateGlobalProperties();

    d_->browser_->GetMainFrame()->LoadString(source.CString(), url.CString());

}


void WebClient::GoBack()
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GoBack();
}

void WebClient::GoForward()
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GoForward();
}

bool WebClient::IsLoading()
{
    if (!d_->browser_.get())
        return false;

    return d_->browser_->IsLoading();
}


void WebClient::Reload()
{
    if (!d_->browser_.get())
        return;

    d_->browser_->Reload();

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
    bool result = d_->CreateBrowser(initialURL, width, height);

    return result;
}

void WebClient::UpdateGlobalProperties()
{
    if (!d_->browser_.get())
        return;

    CefRefPtr<CefDictionaryValue> globalProps;
    if (!WebAppBrowser::CreateGlobalProperties(globalProps))
        return;

    // Create the message object.
    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("atomic_set_globalproperties");

    // Retrieve the argument list object.
    CefRefPtr<CefListValue> args = msg->GetArgumentList();

    args->SetDictionary(0, globalProps);

    // Send the process message to the render process.
    if (!d_->browser_->SendProcessMessage(PID_RENDERER, msg))
    {
        ATOMIC_LOGERROR("WebClient::UpdateGlobalProperties - Failed to send message");
    }

}

void WebClient::HandleWebViewGlobalPropertiesChanged(StringHash eventType, VariantMap& eventData)
{
    UpdateGlobalProperties();
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

void WebClient::SetZoomLevel(float zoomLevel)
{
    if (!d_->browser_.get())
        return;

    d_->browser_->GetHost()->SetZoomLevel(zoomLevel);
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
