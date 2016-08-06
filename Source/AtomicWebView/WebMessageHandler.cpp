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
            ATOMIC_LOGERROR("WebMessageHandlerPrivate::Success - Called with no current callback");
            return;
        }

        queryHandled_ = true;
        currentCallback_->Success(CefString(response.CString()));

    }

    void Failure(int errorCode, const String& errorMessage)
    {
        if (!currentCallback_)
        {
            ATOMIC_LOGERROR("WebMessageHandlerPrivate::Failure - Called with no current callback");
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

        // defaults to not being deferred
        eventData[P_DEFERRED] = false;

        queryHandled_ = false;
        webMessageHandler_->SendEvent(E_WEBMESSAGE, eventData);

        bool deferred = eventData[P_DEFERRED].GetBool();

        if (!deferred)
        {
            if (!queryHandled_)
            {
                currentCallback_ = nullptr;
                ATOMIC_LOGERROR("WebMessageHandlerPrivate::OnQuery - WebQuery was not handled");
                return false;
            }
        }
        else
        {
            DeferredWebMessage* deferred = new DeferredWebMessage();
            deferred->browser_ = browser;
            deferred->frame_  = frame;
            deferred->queryID_ = query_id;
            deferred->request_ = request_;
            deferred->persistent_ = persistent;
            deferred->callback_ = currentCallback_;
            deferredWebMessages_.Push(deferred);
        }

        currentCallback_ = nullptr;

        return true;
    }

    void HandleDeferredResponse(double queryID, bool success, const String& response)
    {
        List<DeferredWebMessage*>::Iterator itr = deferredWebMessages_.Begin();
        while (itr != deferredWebMessages_.End())
        {
            if ((*itr)->queryID_ == queryID)
                break;

            itr++;
        }

        if (itr == deferredWebMessages_.End())
        {
            ATOMIC_LOGERRORF("WebMessageHandlerPrivate::HandleDeferredResponse - unable to find queryid: %d", queryID);
            return;
        }

        if (success)
        {
            (*itr)->callback_->Success(CefString(response.CString()));
        }
        else
        {
            (*itr)->callback_->Failure(0, CefString(response.CString()));
        }

        DeferredWebMessage* ptr = (*itr);
        deferredWebMessages_.Erase(itr);
        delete ptr;

    }

private:

    CefRefPtr<Callback> currentCallback_;
    bool queryHandled_;

    struct DeferredWebMessage
    {
        CefRefPtr<CefBrowser> browser_;
        CefRefPtr<CefFrame> frame_;
        int64 queryID_;
        String request_;
        bool persistent_;
        CefRefPtr<Callback> callback_;
    };

    List<DeferredWebMessage*> deferredWebMessages_;

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

void WebMessageHandler::HandleDeferredResponse(double queryID, bool success, const String& response)
{
    d_->HandleDeferredResponse(queryID, success, response);
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
