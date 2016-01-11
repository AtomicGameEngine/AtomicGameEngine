
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

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

}
