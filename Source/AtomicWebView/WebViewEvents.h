
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

/// WebView load state change
EVENT(E_WEBVIEWLOADSTATECHANGE, WebViewLoadStateChange)
{
    PARAM(P_CLIENT, Client);   // WebClient*
    PARAM(P_LOADING, Loading);   // Boolean
    PARAM(P_CANGOBACK, CanGoBack);   // Boolean
    PARAM(P_CANGOFORWARD, CanGoForward);   // Boolean
}

/// WebView load start
EVENT(E_WEBVIEWLOADSTART, WebViewLoadStart)
{
    PARAM(P_CLIENT, Client);   // WebClient*
    PARAM(P_URL, Url);   // String
}

/// WebView load end
EVENT(E_WEBVIEWLOADEND, WebViewLoadEnd)
{
    PARAM(P_CLIENT, Client);   // WebClient*
    PARAM(P_URL, Url);   // String
}

/// WebView address change
EVENT(E_WEBVIEWADDRESSCHANGE, WebViewAddressChange)
{
    PARAM(P_CLIENT, Client);   // WebClient*
    PARAM(P_URL, Url);   // String
}

/// WebView title change
EVENT(E_WEBVIEWTITLECHANGE, WebViewTitleChange)
{
    PARAM(P_CLIENT, Client);   // WebClient*
    PARAM(P_TITLE, Title);   // String
}

/// WebView title change
EVENT(E_WEBMESSAGE, WebMessage)
{
    PARAM(P_HANDLER, Handler);          // WebMessageHandler*
    PARAM(P_QUERYID, QueryID);          // Double (Int64 CEF Side)
    PARAM(P_REQUEST, Request);          // String
    PARAM(P_PERSISTENT, Persistent);    // Bool

    PARAM(P_CEFBROWSER, Browser);       // CefBrowser*
    PARAM(P_CEFFRAME, Frame);           // CefFrame*
}


}
