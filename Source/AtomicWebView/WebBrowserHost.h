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

#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

class WebBrowserHostPrivate;
class WebClient;

typedef HashMap<String, HashMap<String, Variant>> GlobalPropertyMap;

/// Browser host subsystem, responsible for initializing CEF
class ATOMIC_API WebBrowserHost : public Object
{
    OBJECT(WebBrowserHost);

public:
    /// Construct.
    WebBrowserHost(Context* context);
    /// Destruct.
    virtual ~WebBrowserHost();

    /// Set global property object values, available as read only on page
    static void SetGlobalBoolProperty(const String& globalVar, const String& property, bool value);
    static void SetGlobalStringProperty(const String& globalVar, const String& property, const String& value);
    static void SetGlobalNumberProperty(const String& globalVar, const String& property, double value);

    static const GlobalPropertyMap& GetGlobalProperties() { return globalProperties_; }

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    static void SetGlobalProperty(const String& globalVar, const String& property, Variant& value);

    WebBrowserHostPrivate* d_;

    static WeakPtr<WebBrowserHost> instance_;

    static GlobalPropertyMap globalProperties_;

};

}
