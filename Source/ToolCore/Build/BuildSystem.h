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
#include "BuildBase.h"

#include "../Platform/Platform.h"

using namespace Atomic;

namespace ToolCore
{

class BuildSystem : public Object
{
    ATOMIC_OBJECT(BuildSystem, Object);

public:
    /// Construct.
    BuildSystem(Context* context);
    /// Destruct.
    virtual ~BuildSystem();

    void SetBuildPath(const String& path) { buildPath_ = path; }

    const String& GetBuildPath() const { return buildPath_; }

    void QueueBuild(BuildBase* buildBase);

    bool StartNextBuild();

    void BuildComplete(PlatformID platform, const String& buildFolder, bool success = true, const String& buildMessage = String::EMPTY);

private:

    String buildPath_;

    List<SharedPtr<BuildBase>> queuedBuilds_;
    SharedPtr<BuildBase> currentBuild_;

};


}
