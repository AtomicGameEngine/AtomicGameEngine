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
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/Graphics/Graphics.h>

#include "Internal/WebAppBrowser.h"

#include "WebViewEvents.h"
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
    if ( display && event )
    {
        char msg[132];
        XGetErrorText(display, event->error_code, msg, sizeof(msg));
        fprintf(stderr, "X11 Error %d (%s): request %d.%d \n",
                        event->error_code, 
                        msg, 
                        event->request_code,
                        event->minor_code );
    }
    return 0;
}

static int XIOErrorHandlerImpl(Display *display)
{
    if ( display )
      fprintf(stderr, "XIO Error on display %p, quitting.\n", (void*)display );
    return 0;
}

static void TerminationSignalHandler(int signatl)
{
    fprintf(stderr,"Received signal %d, quitting.\n", signatl );
    exit(0);
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

GlobalPropertyMap WebBrowserHost::globalProperties_;
WeakPtr<WebBrowserHost> WebBrowserHost::instance_;
String WebBrowserHost::rootCacheFolder_;
String WebBrowserHost::cacheName_;
String WebBrowserHost::userAgent_;
String WebBrowserHost::productVersion_;
String WebBrowserHost::jsMessageQueryFunctionName_ = "atomicQuery";
String WebBrowserHost::jsMessageQueryCancelFunctionName_ = "atomicQueryCancel";

int WebBrowserHost::debugPort_ = 3335;
bool WebBrowserHost::webSecurity_ = true;


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

    // IMPORTANT: See flags being set in implementation of void WebAppBrowser::OnBeforeCommandLineProcessing
    // these include "--enable-media-stream", "--enable-usermedia-screen-capturing", "--off-screen-rendering-enabled", "--transparent-painting-enabled"

#ifdef ATOMIC_PLATFORM_LINUX
    static const char* _argv[2] = { "AtomicWebView", "--disable-setuid-sandbox" };
    CefMainArgs args(2, (char**) &_argv);
#else
    CefMainArgs args;
#endif

    CefSettings settings;
    settings.windowless_rendering_enabled = 1;    

    FileSystem* fs = GetSubsystem<FileSystem>();

    // Set CEF log file to existing log folder if any avoid attempting to write
    // to executable folder, which is likely not writeable

    Log* log = GetSubsystem<Log>();
    if (log && log->GetLogFile())
    {
        const File* logFile = log->GetLogFile();
        String logPath = logFile->GetName ();
        if (logPath.Length())
        {
            String pathName, fileName, ext;
            SplitPath(logPath, pathName, fileName, ext);
            if (pathName.Length())
            {
                pathName = AddTrailingSlash(pathName) + "CEF.log";
                CefString(&settings.log_file).FromASCII(GetNativePath(pathName).CString());
            }
            
        }
        
    }        

    // default background is white, add a setting
    // settings.background_color = 0;

    if (productVersion_.Length())
    {
        CefString(&settings.product_version).FromASCII(productVersion_.CString());
    }

    if (userAgent_.Length())
    {
        CefString(&settings.user_agent).FromASCII(userAgent_.CString());
    }

    String fullPath;

    // If we've specified the absolute path to a root cache folder, use it
    if (rootCacheFolder_.Length() && cacheName_.Length())
    {        
        fullPath = rootCacheFolder_ + "/" + cacheName_;
        CefString(&settings.cache_path).FromASCII(fullPath.CString());
    }
    else
    {
        fullPath = fs->GetAppPreferencesDir(cacheName_.CString(), "WebCache");
    }


    CefString(&settings.cache_path).FromASCII(fullPath.CString());

    settings.remote_debugging_port = debugPort_;

    d_ = new WebBrowserHostPrivate(this);

    // If losing OSX system menu, it means we're calling this
    // before initializing graphics subsystem
    if (!CefInitialize(args, settings, d_->app_, nullptr))
    {
        ATOMIC_LOGERROR("CefInitialize - Error");
    }

    RegisterWebSchemeHandlers(this);

    // Ensure cookie manager is created
    CefCookieManager::GetGlobalManager(nullptr);

    SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(WebBrowserHost, HandleUpdate));

    instance_ = this;

}

WebBrowserHost::~WebBrowserHost()
{
    instance_ = 0;

    // TODO: Better place for this?  If there are issues with cookies not persisting
    // it is possible the async nature of this call could be a culprit
    CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
    if (manager.get())
        manager->FlushStore(nullptr);

    CefClearSchemeHandlerFactories();
    CefShutdown();
}

void WebBrowserHost::SetGlobalBoolProperty(const String& globalVar, const String& property, bool value)
{
    Variant v(value);
    SetGlobalProperty(globalVar, property, v);
}

void WebBrowserHost::SetGlobalStringProperty(const String& globalVar, const String& property, const String& value)
{
    Variant v(value);
    SetGlobalProperty(globalVar, property, v);
}

void WebBrowserHost::SetGlobalNumberProperty(const String& globalVar, const String& property, double value)
{
    Variant v(value);
    SetGlobalProperty(globalVar, property, v);
}

void WebBrowserHost::SetGlobalProperty(const String& globalVar, const String &property, Variant& value)
{
    globalProperties_[globalVar][property] = value;

    if (!instance_.NotNull())
        instance_->SendEvent(E_WEBVIEWGLOBALPROPERTIESCHANGED);
}


void WebBrowserHost::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    CefDoMessageLoopWork();
}

void WebBrowserHost::ClearCookies(const String& url, const String& cookieName)
{
    CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);

    if (!manager.get())
        return;

    CefString cefUrl;
    cefUrl.FromASCII(url.CString());

    CefString cefCookieName;
    cefCookieName.FromASCII(cookieName.CString());

    manager->DeleteCookies(cefUrl, cefCookieName, nullptr);

}


}
