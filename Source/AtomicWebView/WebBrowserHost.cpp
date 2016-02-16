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

#include <SDL/include/SDL.h>
#include <ThirdParty/SDL/include/SDL_syswm.h>

#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_helpers.h>
#include <include/base/cef_bind.h>
#include <include/wrapper/cef_closure_task.h>

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IO/Log.h>

#include <Atomic/Graphics/Graphics.h>

#include "Internal/WebAppBrowser.h"

#include "WebSchemeHandler.h"
#include "WebClient.h"
#include "WebBrowserHost.h"

#ifdef ATOMIC_PLATFORM_LINUX

#include <X11/Xlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static int XErrorHandlerImpl(Display *display, XErrorEvent *event)
{
    return 0;
}

static int XIOErrorHandlerImpl(Display *display)
{
    return 0;
}

static void TerminationSignalHandler(int signatl)
{

}

#endif


namespace Atomic
{

class WebBrowserHostPrivate
{
    friend class WebBrowserHost;

public:

    WebBrowserHostPrivate(WebBrowserHost* host)
    {

        host_ = host;
        app_ = new WebAppBrowser();
    }

    virtual ~WebBrowserHostPrivate()
    {
        host_ = 0;
    }

private:

    WeakPtr<WebBrowserHost> host_;
    CefRefPtr<CefApp> app_;

};

WebBrowserHost::WebBrowserHost(Context* context) : Object (context)
{

    const Vector<String>& arguments = GetArguments();

#ifdef ATOMIC_PLATFORM_LINUX
    XSetErrorHandler(XErrorHandlerImpl);
    XSetIOErrorHandler(XIOErrorHandlerImpl);

    // Install a signal handler so we clean up after ourselves.
    signal(SIGINT, TerminationSignalHandler);
    signal(SIGTERM, TerminationSignalHandler);

#endif


    // IMPORTANT: Cef::App contains virtual void OnBeforeCommandLineProcessing(), which should make it possible
    // to setup args on Windows

#ifdef ATOMIC_PLATFORM_OSX
    const char* _argv[3] = { "", "--enable-media-stream", "--enable-usermedia-screen-capturing" };
    CefMainArgs args(3, (char**) &_argv);
#elif ATOMIC_PLATFORM_LINUX
    static const char* _argv[3] = { "AtomicWebView", "--disable-setuid-sandbox", "--off-screen-rendering-enabled" };
    CefMainArgs args(3, (char**) &_argv);
#else
    CefMainArgs args;
#endif

    CefSettings settings;
    settings.windowless_rendering_enabled = true;

    d_ = new WebBrowserHostPrivate(this);

    // If losing OSX system menu, it means we're calling this
    // before initializing graphics subsystem
    if (!CefInitialize(args, settings, d_->app_, nullptr))
    {
        LOGERROR("CefInitialize - Error");
    }

    RegisterWebSchemeHandlers(this);

    SubscribeToEvent(E_BEGINFRAME, HANDLER(WebBrowserHost, HandleBeginFrame));

}

WebBrowserHost::~WebBrowserHost()
{
    CefClearSchemeHandlerFactories();
    CefShutdown();
}

void WebBrowserHost::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
{
    CefDoMessageLoopWork();
}

}
