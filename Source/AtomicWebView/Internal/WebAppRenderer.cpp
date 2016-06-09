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


#include <include/wrapper/cef_message_router.h>

#include "WebAppRenderer.h"

namespace Atomic
{

// Must match the value in client_handler.cc.
static const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

class WebRenderDelegate : public WebAppRenderer::Delegate
{

public:

    static CefRefPtr<CefDictionaryValue> globalProperties_;
    static CefString jsMessageQueryFunctionName_;
    static CefString jsMessageQueryCancelFunctionName_;

    WebRenderDelegate()
        : last_node_is_editable_(false)
    {

    }

    virtual void OnWebKitInitialized(CefRefPtr<WebAppRenderer> app) OVERRIDE
    {
        // Create the renderer-side router for query handling.
        CefMessageRouterConfig config;
        config.js_query_function = jsMessageQueryFunctionName_.length() ? jsMessageQueryFunctionName_ : "atomicQuery";
        config.js_cancel_function = jsMessageQueryCancelFunctionName_.length() ? jsMessageQueryCancelFunctionName_ : "atomicQueryCancel";

        message_router_ = CefMessageRouterRendererSide::Create(config);
    }

    virtual void OnRenderThreadCreated(CefRefPtr<WebAppRenderer> app,
                                       CefRefPtr<CefListValue> extra_info) OVERRIDE
    {
        // extra info comes from void WebAppBrowser::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info)

        // index 0 is global properties
        if (extra_info->GetSize() > 0)
        {
            if (extra_info->GetType(0) == CefValueType::VTYPE_DICTIONARY)
                globalProperties_ = extra_info->GetDictionary(0)->Copy(false);            
        }

        // index 1 is name for jsMessageQuery function
        if (extra_info->GetSize() > 1)
        {            
            jsMessageQueryFunctionName_ = extra_info->GetString(1);
        }

        // index 2 is name for jsMessageQueryCancel function
        if (extra_info->GetSize() > 2)
        {
            jsMessageQueryCancelFunctionName_ = extra_info->GetString(2);
        }

    }

    virtual bool OnBeforeNavigation(CefRefPtr<WebAppRenderer> app,
                                    CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    cef_navigation_type_t navigation_type,
                                    bool is_redirect) OVERRIDE
    {
        return false;
    }

    void UpdateGlobalProperties(CefRefPtr<CefV8Context> context)
    {

        if (!globalProperties_.get())
        {
            return;
        }

        context->Enter();

        CefDictionaryValue::KeyList keys;
        globalProperties_->GetKeys(keys);

        for (unsigned i = 0; i < keys.size(); i++)
        {
            const CefString& globalVarName = keys[i];

            CefRefPtr<CefV8Value> globalVar = CefV8Value::CreateObject(nullptr);

            CefDictionaryValue::KeyList pkeys;
            CefRefPtr<CefDictionaryValue> props = globalProperties_->GetDictionary(globalVarName);
            props->GetKeys(pkeys);

            for (unsigned j = 0; j < pkeys.size(); j++)
            {
                const CefString& keyName = pkeys[j];

                CefValueType type = props->GetType(keyName);

                if (type == VTYPE_BOOL)
                {
                    globalVar->SetValue(keyName, CefV8Value::CreateBool(props->GetBool(keyName)), V8_PROPERTY_ATTRIBUTE_NONE);
                }
                else if (type == VTYPE_DOUBLE)
                {
                    globalVar->SetValue(keyName, CefV8Value::CreateDouble(props->GetDouble(keyName)), V8_PROPERTY_ATTRIBUTE_NONE);
                }
                else if (type == VTYPE_STRING)
                {
                    globalVar->SetValue(keyName, CefV8Value::CreateString(props->GetString(keyName)), V8_PROPERTY_ATTRIBUTE_NONE);
                }
            }

            context->GetGlobal()->SetValue(globalVarName, globalVar, V8_PROPERTY_ATTRIBUTE_NONE);
        }

        context->Exit();
    }

    virtual void OnBrowserCreated(CefRefPtr<WebAppRenderer> app,
                                  CefRefPtr<CefBrowser> browser) OVERRIDE
    {
    }

    virtual void OnContextCreated(CefRefPtr<WebAppRenderer> app,
                                  CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) OVERRIDE
    {
        message_router_->OnContextCreated(browser,  frame, context);

        UpdateGlobalProperties(context);
    }

    virtual void OnContextReleased(CefRefPtr<WebAppRenderer> app,
                                   CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context) OVERRIDE
    {
        message_router_->OnContextReleased(browser,  frame, context);
    }

    virtual bool OnProcessMessageReceived (
            CefRefPtr<WebAppRenderer> app,
            CefRefPtr<CefBrowser> browser,
            CefProcessId source_process,
            CefRefPtr<CefProcessMessage> message) OVERRIDE
    {

        const CefString& message_name = message->GetName();

        if (message_name == "atomic_eval_javascript")
        {
            CefRefPtr<CefV8Context> context = browser->GetMainFrame()->GetV8Context();
            CefRefPtr<CefV8Value> retval;
            CefRefPtr<CefV8Exception> exception;

            context->Enter();

            CefRefPtr<CefV8Value> json = context->GetGlobal()->GetValue("JSON");
            if (!json.get() || !json->IsObject())
            {
                context->Exit();
                return false;
            }

            CefRefPtr<CefV8Value> stringify = json->GetValue("stringify");
            if (!stringify.get() || !stringify->IsFunction())
            {
                context->Exit();
                return false;
            }

            unsigned evalID = (unsigned) message->GetArgumentList()->GetInt(0);
            CefString script = message->GetArgumentList()->GetString(1);

            bool result = context->Eval(script, retval, exception);

            // Create the result message object.
            CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("atomic_eval_javascript_result");

            // Retrieve the argument list object.
            CefRefPtr<CefListValue> args = msg->GetArgumentList();

            // Populate the argument values.
            args->SetInt(0, (int) evalID);
            args->SetBool(1, result);
            if (result)
            {
                CefV8ValueList stringifyArgs;
                stringifyArgs.push_back(retval);
                CefRefPtr<CefV8Value> sjson = stringify->ExecuteFunctionWithContext(context, NULL, stringifyArgs);
                if (!sjson.get() || !sjson->IsString())
                {
                    args->SetString(2, "WebAppRenderer::OnProcessMessageReceived() - Error Getting Return JSON");
                }
                else
                {
                    args->SetString(2, sjson->GetStringValue());
                }
            }
            else
                args->SetString(2, exception->GetMessage());

            browser->SendProcessMessage(PID_BROWSER, msg);

            context->Exit();
            return true;
        }
        else if (message_name == "atomic_set_globalproperties")
        {
            globalProperties_ = message->GetArgumentList()->GetDictionary(0)->Copy(false);
            UpdateGlobalProperties(browser->GetMainFrame()->GetV8Context());
            return true;
        }

        return message_router_->OnProcessMessageReceived(
                    browser, source_process, message);
    }

private:
    bool last_node_is_editable_;

    // Handles the renderer side of query routing.
    CefRefPtr<CefMessageRouterRendererSide> message_router_;

    IMPLEMENT_REFCOUNTING(WebRenderDelegate);
};

CefRefPtr<CefDictionaryValue> WebRenderDelegate::globalProperties_;
CefString WebRenderDelegate::jsMessageQueryFunctionName_;
CefString WebRenderDelegate::jsMessageQueryCancelFunctionName_;


WebAppRenderer::WebAppRenderer() {
}

void WebAppRenderer::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info)
{
    delegates_.Push(new WebRenderDelegate());

    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnRenderThreadCreated(this, extra_info);
}

void WebAppRenderer::OnWebKitInitialized()
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnWebKitInitialized(this);
}

void WebAppRenderer::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnBrowserCreated(this, browser);
}

void WebAppRenderer::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnBrowserDestroyed(this, browser);
}

CefRefPtr<CefLoadHandler> WebAppRenderer::GetLoadHandler()
{
    CefRefPtr<CefLoadHandler> load_handler;
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End() && !load_handler.get(); ++it)
        load_handler = (*it)->GetLoadHandler(this);

    return load_handler;
}

bool WebAppRenderer::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        NavigationType navigation_type,
                                        bool is_redirect)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it) {
        if ((*it)->OnBeforeNavigation(this, browser, frame, request, navigation_type, is_redirect))
        {
            return true;
        }
    }

    return false;
}

void WebAppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefV8Context> context)
{

    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnContextCreated(this, browser, frame, context);
}

void WebAppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       CefRefPtr<CefV8Context> context)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnContextReleased(this, browser, frame, context);
}

void WebAppRenderer::OnUncaughtException(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context,
        CefRefPtr<CefV8Exception> exception,
        CefRefPtr<CefV8StackTrace> stackTrace)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
    {
        (*it)->OnUncaughtException(this, browser, frame, context, exception, stackTrace);
    }
}

void WebAppRenderer::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefDOMNode> node)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnFocusedNodeChanged(this, browser, frame, node);
}

bool WebAppRenderer::OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message)
{
    assert(source_process == PID_BROWSER);

    bool handled = false;

    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End() && !handled; ++it) {
        handled = (*it)->OnProcessMessageReceived(this, browser, source_process, message);
    }

    return handled;
}
}
