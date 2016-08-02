//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include "../Precompiled.h"

#include "../Core/Profiler.h"
#include "../Core/CoreEvents.h"
#include "../IO/Log.h"
#include "../Web/Web.h"
#include "../Web/WebRequest.h"
#include "../Web/WebSocket.h"

#ifndef EMSCRIPTEN
#include "../Web/WebInternalConfig.h"
#include <asio/io_service.hpp>
#include <curl/curl.h>
#endif

#include "../DebugNew.h"

namespace Atomic
{

struct WebPrivate
{
#ifndef EMSCRIPTEN
    asio::io_service service;
    CURLM *curlm;
#endif
};

Web::Web(Context* context) :
    Object(context),
    d(new WebPrivate())
{
#ifndef EMSCRIPTEN
    d->curlm = curl_multi_init();
#endif
    SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(Web, internalUpdate));
}

Web::~Web()
{
    UnsubscribeFromEvent(E_UPDATE);
#ifndef EMSCRIPTEN
    curl_multi_cleanup(d->curlm);
#endif
    delete d;
}

void Web::internalUpdate(StringHash eventType, VariantMap& eventData)
{
#ifndef EMSCRIPTEN
    int runningHandles;
    curl_multi_perform(d->curlm, &runningHandles);

    CURLMsg *msg;
    int msgsLeft;
    while ((msg = curl_multi_info_read(d->curlm, &msgsLeft)))
    {
        if (msg->msg != CURLMSG_DONE)
        {
            continue;
        }

        WebRequest *wr;
        curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &wr);
        if (wr != NULL)
        {
            WebRequest::internalNotify(wr, int(msg->data.result));
        }
    }

    d->service.reset();
    d->service.poll();
#endif
}

SharedPtr<WebRequest> Web::MakeWebRequest(const String& verb, const String& url, double requestContentSize)
{
    ATOMIC_PROFILE(MakeWebRequest);

    // The initialization of the request will take time, can not know at this point if it has an error or not
    SharedPtr<WebRequest> webRequest(new WebRequest(context_, verb, url, requestContentSize));
#ifndef EMSCRIPTEN
    webRequest->setup(&d->service, d->curlm);
#endif
    return webRequest;
}

SharedPtr<WebSocket> Web::MakeWebSocket(const String& url)
{
  ATOMIC_PROFILE(MakeWebSocket);

  // The initialization of the WebSocket will take time, can not know at this point if it has an error or not
  SharedPtr<WebSocket> webSocket(new WebSocket(context_, url));
#ifndef EMSCRIPTEN
  webSocket->setup(&d->service);
#endif
  return webSocket;
}

}
