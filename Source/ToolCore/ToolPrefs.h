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

using namespace Atomic;

namespace ToolCore
{

class ToolPrefs : public Object
{
    ATOMIC_OBJECT(ToolPrefs, Object)

public:

    ToolPrefs(Context* context);
    virtual ~ToolPrefs();

    const String& GetAndroidSDKPath() { return androidSDKPath_; }
    const String& GetJDKRootPath() { return jdkRootPath_; }
    const String& GetAntPath();
    const String& GetReleasePath() { return releasePath_; }
    const int GetReleaseCheck() { return releaseCheck_; }

    void SetAndroidSDKPath(const String& path) { androidSDKPath_ = path; }
    void SetJDKRootPath(const String& path) { jdkRootPath_ = path; }
    void SetAntPath(const String& path) { antPath_ = path; }
    void SetReleasePath(const String& path) { releasePath_ = path; }
    void SetReleaseCheck(const int value) { releaseCheck_ = value; }

    String GetPrefsPath();
    void Load();
    void Save();

private:

    String androidSDKPath_;
    String jdkRootPath_;
    String antPath_;
    String releasePath_;
    int releaseCheck_;
};

}
