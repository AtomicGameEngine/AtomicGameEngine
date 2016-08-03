//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Core/Context.h>
#include "CurlManager.h"
#include <curl/curl.h>

namespace ToolCore
{

CurlRequest::CurlRequest(Context* context, const String& url, const String& postData) :
    Object(context)
{
    curl_ = curl_easy_init();

    // take care, curl doesn't make copies of all data
    url_ = url;
    postData_ = postData;

#if !(defined WIN32 || defined APPLE)
    // This line will eventually go away with a CA bundle in place, or other TLS options.
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

    curl_easy_setopt(curl_, CURLOPT_URL, url_.CString());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, Writer);

    if (postData.Length())
    {
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, postData_.CString());
    }

    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *) this);

}

CurlRequest::~CurlRequest()
{
    curl_easy_cleanup(curl_);
}

void CurlRequest::ThreadFunction()
{
    CURLcode res;
    res = curl_easy_perform(curl_);

    if(res != CURLE_OK)
    {
        error_ = curl_easy_strerror(res);
    }

    shouldRun_ = false;
}

size_t CurlRequest::Writer(void *ptr, size_t size, size_t nmemb, void *crequest)
{
    CurlRequest* request = (CurlRequest*) crequest;

    size_t realsize = size * nmemb;
    const char* text = (const char*) ptr;

    for (size_t i = 0; i < realsize; i++)
    {
        request->response_ += text[i];
    }

    return realsize;

}


CurlManager::CurlManager(Context* context) :
    Object(context)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(CurlManager, HandleUpdate));
}

CurlManager::~CurlManager()
{
    curl_global_cleanup();
}

SharedPtr<CurlRequest> CurlManager::MakeRequest(const String& url, const String& postData)
{
    SharedPtr<CurlRequest> request(new CurlRequest(context_, url, postData));
    request->Run();
    requests_.Push(request);
    return request;
}

void CurlManager::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    Vector<CurlRequest*> remove;

    for (unsigned i = 0; i < requests_.Size(); i++)
    {
        CurlRequest* request = requests_[i];

        if (!request->shouldRun_)
        {
            remove.Push(request);
            VariantMap eventData;
            eventData[CurlComplete::P_CURLREQUEST] = request;
            request->SendEvent(E_CURLCOMPLETE, eventData);

        }
    }

    for (unsigned i = 0; i < remove.Size(); i++)
    {
        requests_.Remove(SharedPtr<CurlRequest>(remove[i]));
    }

}

}
