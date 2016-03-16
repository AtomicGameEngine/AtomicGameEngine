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

#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"
#include "../Project/ProjectUserPrefs.h"

#include "BuildSystem.h"
#include "BuildEvents.h"

namespace ToolCore
{

BuildSystem::BuildSystem(Context* context) :
    Object(context)
{

}

BuildSystem::~BuildSystem()
{

}

bool BuildSystem::StartNextBuild()
{
    if (!queuedBuilds_.Size())
        return false;

    currentBuild_ = queuedBuilds_.Front();
    queuedBuilds_.PopFront();

    currentBuild_->Build(buildPath_);

    return true;
}

void BuildSystem::QueueBuild(BuildBase* buildBase)
{
    queuedBuilds_.Push(SharedPtr<BuildBase>(buildBase));
}

void BuildSystem::BuildComplete(PlatformID platform, const String &buildFolder, bool success, const String& buildMessage)
{
    VariantMap eventData;

    eventData[BuildComplete::P_PLATFORMID] = (unsigned) platform;
    eventData[BuildComplete::P_BUILDFOLDER] = buildFolder;
    eventData[BuildComplete::P_SUCCESS] = success;
    eventData[BuildComplete::P_MESSAGE] = buildMessage;
    SendEvent(E_BUILDCOMPLETE, eventData);

    currentBuild_ = 0;
}



}
