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

#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Resource/JSONValue.h>

using namespace Atomic;

namespace ToolCore
{

    class DesktopProjectSettings : public RefCounted
    {
        ATOMIC_REFCOUNTED(DesktopProjectSettings)

    public:

        DesktopProjectSettings() {}

        void Write(JSONValue& parent) {}
        void Read(JSONValue& parent) {}

    private:

    };

    class AndroidProjectSettings : public RefCounted
    {
        ATOMIC_REFCOUNTED(AndroidProjectSettings)

    public:

        AndroidProjectSettings() {}

        void Write(JSONValue& parent) {}
        void Read(JSONValue& parent) {}

    private:

    };

    class ProjectSettings : public Object
    {
        ATOMIC_OBJECT(ProjectSettings, Object)

    public:
        /// Construct.
        ProjectSettings(Context* context);
        /// Destruct.
        virtual ~ProjectSettings();

        DesktopProjectSettings* GetDesktopSettings() { return desktopSettings_; }
        AndroidProjectSettings* GetAndroidPlatformSettings() { return androidSettings_; }

        bool Load(const String& path);
        void Save(const String& path);

        const String& GetName() const { return name_;  }

        bool ValidPlatform(const String& platform) const;

        bool GetSupportsPlatform(const String& platform) const;
        bool GetSupportsDesktop() const;
        bool GetSupportsAndroid() const;
        bool GetSupportsIOS() const;
        bool GetSupportsWeb() const;

        void AddSupportedPlatform(const String& platform);

    private:

        void SetDefault();

        String name_;
        List<String> platforms_;

        SharedPtr<DesktopProjectSettings> desktopSettings_;
        SharedPtr<AndroidProjectSettings> androidSettings_;

    };

}
