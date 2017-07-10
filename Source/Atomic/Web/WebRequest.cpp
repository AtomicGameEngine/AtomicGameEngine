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
#include "../Container/HashMap.h"
#include "../IO/BufferQueue.h"
#include "../IO/Log.h"
#include "../Web/Web.h"
#include "../Web/WebEvents.h"
#include "../Web/WebRequest.h"

#ifdef EMSCRIPTEN

#include "../DebugNew.h"
// Add code to use an XMLHttpRequest or ActiveX XMLHttpRequest here.

#else

#include "../Web/WebInternalConfig.h"
#include <asio.hpp>
#include <functional>
#include <curl/curl.h>

#include "../DebugNew.h"

namespace Atomic
{

struct WebRequestInternalState
{
    /// The WebRequest external state
    WebRequest& es;
    /// The work queue.
    asio::io_service* service;
    /// URL.
    String url;
    /// Verb.
    String verb;
    /// Response headers.
    HashMap<StringHash, Pair<String, String>> responseHeaders;
    /// Upload stream.
    SharedPtr<BufferQueue> upload;
    /// Download stream.
    SharedPtr<BufferQueue> download;
    /// Request Headers.
    curl_slist* headers = NULL;
    /// Connection state.
    WebRequestState state;
    /// cURL multi handle.
    CURLM* curlm;
    /// cURL easy handle.
    CURL* curl;
    /// A flag to know if the request has contents (has data to upload).
    curl_off_t requestContentSize;
    /// A flag to know if the operation has been aborted.
    bool isAborted;
    /// A flag to know if the easy handle has been added to the Web class's multi handle.
    bool isAddedToMulti;
    /// Error string. Empty if no error.
    char error[CURL_ERROR_SIZE];

    WebRequestInternalState(WebRequest &es_) :
        es(es_)
    {
    }

    ~WebRequestInternalState()
    {
    }

    static int onProgress(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
    {
        WebRequestInternalState *is_(reinterpret_cast<WebRequestInternalState*>(clientp));
        if (is_->isAborted)
        {
            // This should probably be CURL_XFERINFO_ABORT, but that doesn't
            // exist. It probably would be the same numeric value, if it did.
            // The docs say that it just has to be a nonzero to abort.
            return CURL_READFUNC_ABORT;
        }

        using namespace WebRequestProgress;

        VariantMap eventData;
        eventData[P_REQUEST] = &is_->es;
        eventData[P_DOWNLOADTOTAL] = (int) dltotal;
        eventData[P_DOWNLOADED] = (int) dlnow;
        eventData[P_UPLOADTOTAL] = (int) ultotal;
        eventData[P_UPLOADED] = (int) ulnow;
        is_->es.SendEvent(E_WEBREQUESTPROGRESS, eventData);

        return 0;
    }

    static size_t onHeader(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        WebRequestInternalState *is_(reinterpret_cast<WebRequestInternalState*>(userdata));
        if (is_->isAborted)
        {
            is_->state = WR_CLOSED;
            // This should probably be CURL_HEADERFUNC_ABORT, but that doesn't
            // exist. It probably would be the same numeric value, if it did.
            // The docs say that it just has to be a number of bytes that is
            // not "size * nmemb" to abort.
            return CURL_READFUNC_ABORT;
        }

        // Find the size in bytes.
        size_t real_size(size * nmemb);

        // Check for some known values.
        if (real_size == 2 && ptr[0] == '\r' && ptr[1] == '\n')
        {
            return real_size;
        }
        if (real_size > 5 && !strncmp(ptr, "HTTP/", 5))
        {
            return real_size;
        }

        // Get the header key and value, and add them to the map.
        unsigned int key_end = 0;
        unsigned int value_begin = 2;
        while (value_begin < real_size)
        {
            if (ptr[key_end] == ':' && ptr[key_end + 1] == ' ')
            {
                break;
            }
            ++key_end;
            ++value_begin;
        }
        if (value_begin == real_size)
        {
            String key(ptr, (unsigned int)real_size);
            is_->responseHeaders.InsertNew(key.ToUpper(), MakePair(key, String()));
        }
        else
        {
            String key(ptr, (unsigned int)key_end);
            is_->responseHeaders.InsertNew(key.ToUpper(), MakePair(key, String(ptr + value_begin, (unsigned int)real_size - value_begin - 2)));
        }

        return real_size;
    }

    static size_t onWrite(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        WebRequestInternalState *is_(reinterpret_cast<WebRequestInternalState*>(userdata));
        is_->state = WR_OPEN;
        if (is_->isAborted)
        {
            is_->state = WR_CLOSED;
            // This should probably be CURL_WRITEFUNC_ABORT, but that doesn't
            // exist. It probably would be the same numeric value, if it did.
            // The docs say that it just has to be a number of bytes that is
            // not "size * nmemb" to abort.
            return CURL_READFUNC_ABORT;
        }

        // Find the size in bytes.
        size_t real_size(size * nmemb);

        // Write the date into the download buffer queue.
        is_->download->Write(ptr, (unsigned int)real_size);

        using namespace WebRequestDownloadChunk;
        VariantMap eventData;

        eventData[P_REQUEST] = &is_->es;
        eventData[P_DOWNLOAD] = is_->download;
        eventData[P_CHUNKSIZE] = (unsigned) real_size;

        is_->es.SendEvent(E_WEBREQUESTDOWNLOADCHUNK, eventData);

        return real_size;
    }

    static size_t onRead(char *buffer, size_t size, size_t nitems, void *instream)
    {
        WebRequestInternalState *is_(reinterpret_cast<WebRequestInternalState*>(instream));
        is_->state = WR_OPEN;
        if (is_->isAborted)
        {
            is_->state = WR_CLOSED;
            return CURL_READFUNC_ABORT;
        }

        // Find the size in bytes.
        size_t real_size(size * nitems);

        // Read as much as we can from the upload buffer queue.
        size_t size_queued(is_->upload->GetSize());
        size_t size_left(real_size);
        if ((size_left > 0) && (size_queued > 0))
        {
            size_t read_size(std::min(size_queued, size_left));
            is_->upload->Read(buffer, (unsigned int)read_size);
            size_left -= read_size;
        }

        // If we still have bytes to fill, then emit a E_WEBREQUESTUPLOADCHUNK event.
        if (size_left > 0)
        {
            using namespace WebRequestUploadChunk;

            VariantMap eventData;
            eventData[P_REQUEST] = &is_->es;
            eventData[P_UPLOAD] = is_->upload;
            eventData[P_BYTESREMAINING] = (unsigned)size_left;

            is_->es.SendEvent(E_WEBREQUESTUPLOADCHUNK, eventData);
        }

        // Read as much as we can from the upload buffer queue (again).
        size_queued = is_->upload->GetSize();
        size_left = real_size;
        if ((size_left > 0) && (size_queued > 0))
        {
            size_t read_size(std::min(size_queued, size_left));
            is_->upload->Read(buffer, (unsigned int)read_size);
            size_left -= read_size;
        }

        // If we still have bytes to fill, then something went wrong, so we should abort.
        if (size_left > 0)
        {
            is_->isAborted = true;
            return CURL_READFUNC_ABORT;
        }

        return real_size;
    }

    void onEnd(int code)
    {       
        using namespace WebRequestComplete;
        VariantMap eventData;

        eventData[P_REQUEST] = &es;

        if (code != CURLE_OK)
        {
            state = WR_ERROR;
            eventData[P_ERROR] = String(error, (unsigned int)strnlen(error, sizeof(error)));
        }
        else
        {
            state = WR_CLOSED;
            eventData[P_DOWNLOAD] = download;
            eventData[P_UPLOAD] = upload;
        }

        es.SendEvent(E_WEBREQUESTCOMPLETE, eventData);
    }
};

WebRequest::WebRequest(Context* context, const String& verb, const String& url, double requestContentSize) :
    Object(context),
    is_(new WebRequestInternalState(*this))
{
    is_->url = url.Trimmed();
    is_->verb = verb;
    is_->upload = new BufferQueue(context);
    is_->download = new BufferQueue(context);
    is_->state = WR_INITIALIZING;
    is_->curlm = NULL;
    is_->curl = NULL;
    is_->requestContentSize = curl_off_t(std::floor(requestContentSize));
    is_->isAborted = false;
    is_->isAddedToMulti = false;

    Web* web = GetSubsystem<Web>();
    web->setup(*this);
}

WebRequest::~WebRequest()
{
    curl_slist_free_all(is_->headers);
    if (is_->curlm == NULL)
    {
        return;
    }
    curl_easy_cleanup(is_->curl);
    delete is_;
}

void WebRequest::setup(asio::io_service *service, CURLM *curlm)
{
    is_->service = service;
    is_->curlm = curlm;
    is_->curl = curl_easy_init();

    curl_easy_setopt(is_->curl, CURLOPT_ERRORBUFFER, is_->error);
    is_->error[0] = '\0';

#if !(defined WIN32 || defined APPLE)
    // This line will eventually go away with a CA bundle in place, or other TLS options.
    curl_easy_setopt(is_->curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

    curl_easy_setopt(is_->curl, CURLOPT_URL, is_->url.CString());

    // All callbacks must look at is_->isAborted flag!

    curl_easy_setopt(is_->curl, CURLOPT_HEADERFUNCTION, &WebRequestInternalState::onHeader);
    curl_easy_setopt(is_->curl, CURLOPT_HEADERDATA, is_);

    curl_easy_setopt(is_->curl, CURLOPT_WRITEFUNCTION, &WebRequestInternalState::onWrite);
    curl_easy_setopt(is_->curl, CURLOPT_WRITEDATA, is_);

    curl_easy_setopt(is_->curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(is_->curl, CURLOPT_XFERINFOFUNCTION, &WebRequestInternalState::onProgress);
    curl_easy_setopt(is_->curl, CURLOPT_XFERINFODATA, is_);

    curl_easy_setopt(is_->curl, CURLOPT_CUSTOMREQUEST, is_->verb.CString());

    curl_easy_setopt(is_->curl, CURLOPT_PRIVATE, this);

    curl_easy_setopt(is_->curl, CURLOPT_READFUNCTION, &WebRequestInternalState::onRead);
    curl_easy_setopt(is_->curl, CURLOPT_READDATA, is_);

    if (is_->requestContentSize)
    {
        curl_easy_setopt(is_->curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(is_->curl, CURLOPT_INFILESIZE_LARGE, is_->requestContentSize);
    }
}

void WebRequest::internalNotify(WebRequest *wr, int code)
{
    wr->is_->onEnd(code);

    if (wr->is_->isAddedToMulti)
    {
        curl_multi_remove_handle(wr->is_->curlm, wr->is_->curl);
        wr->is_->isAddedToMulti = false;

        // release the reference held from the Send method
        wr->ReleaseRef();
    }
}

void WebRequest::Abort()
{
    is_->isAborted = true;
}

const String& WebRequest::GetURL() const
{
    return is_->url;
}

String WebRequest::GetError() const
{
    return String(is_->error);
}

WebRequestState WebRequest::GetState() const
{
    return is_->state;
}

String WebRequest::GetVerb() const
{
    return is_->verb;
}

bool WebRequest::HasDownloadChunkEvent()
{
    return true; // cURL based implementations always support the "download_chunk" event.
}

void WebRequest::SetRequestHeader(const String& key, const String& value)
{
    // Trim and only add non-empty header strings.
    String header;
    header += key.Trimmed();
    header += ": ";
    header += value;
    if (header.Length())
    {
        is_->headers = curl_slist_append(is_->headers, header.CString());
    }
}

void WebRequest::Send()
{
    if (!is_->isAddedToMulti && !is_->isAborted)
    {
        // Add a reference to ourselves during the Send, this is released
        // in notifyInternal,  if we're leaking WebRequests check that
        // this is being called
        AddRef();

        curl_easy_setopt(is_->curl, CURLOPT_HTTPHEADER, is_->headers);
        if (CURLM_OK != curl_multi_add_handle(is_->curlm, is_->curl))
        {
            ATOMIC_LOGERROR("WebRequest::Send() - ERROR SENDING REQUEST!");
        }
        is_->isAddedToMulti = true;
    }
}

StringVector WebRequest::GetResponseHeaderKeys()
{
    StringVector keys;
    for (auto it(is_->responseHeaders.Begin()),
        itEnd(is_->responseHeaders.End()); it != itEnd; ++it)
    {
        keys.Push(it->second_.first_);
    }
    return keys;
}

String WebRequest::GetResponseHeader(const String& header)
{
    auto it(is_->responseHeaders.Find(header.ToUpper()));
    if (it == is_->responseHeaders.End())
    {
        return "";
    }
    return it->second_.second_;
}

String WebRequest::GetAllResponseHeaders()
{
    String allHeaders;
    for (auto it(is_->responseHeaders.Begin()),
        itEnd(is_->responseHeaders.End()); it != itEnd; ++it)
    {
        allHeaders += it->second_.first_;
        allHeaders += ": ";
        allHeaders += it->second_.second_;
        allHeaders += "\r\n";
    }
    return allHeaders;
}

void WebRequest::SetPostData(const String& postData)
{
    // use the copy post fields option so we don't need to hold onto the string buffer
    curl_easy_setopt(is_->curl, CURLOPT_COPYPOSTFIELDS, postData.CString());
}

SharedPtr<BufferQueue> WebRequest::GetDownloadBufferQueue()
{
    return is_->download;
}

}


#endif
