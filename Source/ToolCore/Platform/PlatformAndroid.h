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

#include "Platform.h"
#include "../Subprocess/Subprocess.h"

namespace ToolCore
{

ATOMIC_EVENT(E_ANDROIDTARGETSREFRESHED, AndroidTargetsRefreshed)
{

}

class PlatformAndroid : public Platform
{
    ATOMIC_OBJECT(PlatformAndroid, Platform);

    struct AndroidTarget
    {
        unsigned id;
    };

public:

    PlatformAndroid(Context* context);
    virtual ~PlatformAndroid();

    virtual bool GetLicense();

    String GetName() { return "ANDROID"; }
    PlatformID GetPlatformID() { return PLATFORMID_ANDROID; }

    String GetAndroidCommand() const;
    String GetADBCommand() const;

    void RefreshAndroidTargets();
    const Vector<String>& GetAndroidTargets() { return androidTargets_; }

    BuildBase* NewBuild(Project* project);

private:

    void HandleRefreshAndroidTargetsEvent(StringHash eventType, VariantMap& eventData);

    SharedPtr<Subprocess> refreshAndroidTargetsProcess_;
    String targetOutput_;

    Vector<String> androidTargets_;

    void PrependAndroidCommandArgs(Vector<String> args);

};

}
