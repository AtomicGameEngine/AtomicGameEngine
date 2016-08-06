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

#include "../Platform/Platform.h"

namespace ToolCore
{

class ProjectUserPrefs : public Object
{
    friend class Project;

    ATOMIC_OBJECT(ProjectUserPrefs, Object);

public:
    /// Construct.
    ProjectUserPrefs(Context* context);
    /// Destruct.
    virtual ~ProjectUserPrefs();

    // platform used when not specified
    PlatformID GetDefaultPlatform() const { return defaultPlatform_; }

    const String& GetLastBuildPath() { return lastBuildPath_; }
    void SetLastBuildPath(const String& path) { lastBuildPath_ = path; }

    float GetSnapTranslationX() const;
    float GetSnapTranslationY() const;
    float GetSnapTranslationZ() const;
    float GetSnapRotation() const;
    float GetSnapScale() const;

    void SetSnapTranslationX(float value);
    void SetSnapTranslationY(float value);
    void SetSnapTranslationZ(float value);
    void SetSnapRotation(float value);
    void SetSnapScale(float value);

private:

    bool Load(const String& path);
    void Save(const String& path);

    PlatformID defaultPlatform_;
    String lastBuildPath_;

    float snapTranslationX_;
    float snapTranslationY_;
    float snapTranslationZ_;
    float snapRotation_;
    float snapScale_;

};

}
