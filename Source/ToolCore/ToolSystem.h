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

#include "Platform/Platform.h"

using namespace Atomic;

namespace ToolCore
{

class Platform;
class Project;

class ToolSystem : public Object
{
    ATOMIC_OBJECT(ToolSystem, Object);

public:

    ToolSystem(Context* context);
    virtual ~ToolSystem();

    bool LoadProject(const String& fullpath);
    Project* GetProject() { return project_; }
    void CloseProject();

    const String& GetDataPath() { return dataPath_; }
    void SetDataPath(const String& path) { dataPath_ = path; }

    // Platforms
    void RegisterPlatform(Platform* platform);
    Platform* GetPlatformByID(PlatformID platform);
    Platform* GetPlatformByName(const String& name);

    void SetCurrentPlatform(PlatformID platform);
    Platform* GetCurrentPlatform();

    void SetCLI() { cli_ = true; }
    bool IsCLI() { return cli_; }

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    /// Full path to data files
    String dataPath_;

    SharedPtr<Platform> currentPlatform_;

    // PlatformID -> platform
    HashMap<unsigned, SharedPtr<Platform> > platforms_;

    SharedPtr<Project> project_;

    bool cli_;

    float updateDelta_;

};

}
