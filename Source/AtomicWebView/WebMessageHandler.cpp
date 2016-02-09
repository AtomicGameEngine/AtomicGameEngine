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

#include "include/wrapper/cef_message_router.h"

#include <Atomic/IO/Log.h>

#include "WebViewEvents.h"
#include "WebString.h"
#include "WebClient.h"
#include "WebMessageHandler.h"

namespace Atomic
{

class WebMessageHandlerPrivate : public CefMessageRouterBrowserSide::Handler
{
public:

    WebMessageHandlerPrivate(WebMessageHandler* webMessageHandler) : webMessageHandler_(webMessageHandler), queryHandled_(false)
    {

    }

    void Success(const String& response)
    {
        if (!currentCallback_)
        {
            LOGERROR("WebMessageHandlerPrivate::Success - Called with no current callback");
            return;
        }

        queryHandled_ = true;
        currentCallback_->Success(CefString(response.CString()));

    }

    void Failure(int errorCode, const String& errorMessage)
    {
        if (!currentCallback_)
        {
            LOGERROR("WebMessageHandlerPrivate::Failure - Called with no current callback");
            return;
        }

        queryHandled_ = true;
        currentCallback_->Failure(errorCode, CefString(errorMessage.CString()));

    }

    // Called due to atomicQuery execution.
    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64 query_id,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) OVERRIDE
    {
        currentCallback_ = callback;

        VariantMap eventData;
        using namespace WebMessage;

        String request_;
        ConvertCEFString(request, request_);

        eventData[P_HANDLER] = webMessageHandler_;

        eventData[P_QUERYID] = (double) query_id;
        eventData[P_REQUEST] = request_;
        eventData[P_PERSISTENT] = persistent;

        eventData[P_CEFBROWSER] = (void*) browser;
        eventData[P_CEFFRAME] = (void*) frame;

        queryHandled_ = false;
        webMessageHandler_->SendEvent(E_WEBMESSAGE, eventData);

        currentCallback_ = nullptr;

        if (!queryHandled_)
        {
            LOGERROR("WebMessageHandlerPrivate::OnQuery - WebQuery was not handled");
            return false;
        }

        return true;
    }

private:

    CefRefPtr<Callback> currentCallback_;
    bool queryHandled_;

    WeakPtr<WebMessageHandler> webMessageHandler_;
};

WebMessageHandler::WebMessageHandler(Context* context) : Object(context)
{
    d_ = new WebMessageHandlerPrivate(this);
}

WebMessageHandler::~WebMessageHandler()
{
    delete d_;
    d_ = nullptr;
}

void WebMessageHandler::Success(const String& response)
{
    d_->Success(response);
}

void WebMessageHandler::Failure(int errorCode, const String& errorMessage)
{
    d_->Failure(errorCode, errorMessage);
}


}
