// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

typedef void CURL;

#include <Atomic/Core/Object.h>
#include <Atomic/Container/Str.h>
#include <Atomic/Core/Mutex.h>
#include <Atomic/Core/Thread.h>

using namespace Atomic;

namespace AtomicEditor
{

class CurlRequest : public Object, public Thread
{
    friend class CurlManager;

    OBJECT(CurlRequest);

public:

    CurlRequest(Context* context, const String& url, const String& postData = String::EMPTY);
    virtual ~CurlRequest();

    const String& GetError() { return error_; }
    const String& GetResponse() { return response_; }

private:

    void ThreadFunction();
    mutable Mutex mutex_;

    static size_t Writer(void *ptr, size_t size, size_t nmemb, void *userp);

    CURL* curl_;
    String response_;
    String error_;

    String postData_;
    String url_;
};

class CurlManager : public Object
{
    OBJECT(CurlManager);

public:
    /// Construct.
    CurlManager(Context* context);
    /// Destruct.
    virtual ~CurlManager();

    SharedPtr<CurlRequest> MakeRequest(const String& url, const String& postData = String::EMPTY);

private:

    void HandleEditorShutdown(StringHash eventType, VariantMap& eventData);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    Vector< SharedPtr<CurlRequest> > requests_;

};

}




