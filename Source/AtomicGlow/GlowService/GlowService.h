//
// Copyright (c) 2014-2017 THUNDERBEAST GAMES LLC
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
#include <Atomic/Scene/Scene.h>

#include <AtomicGlow/Common/GlowSettings.h>

using namespace Atomic;

namespace AtomicGlow
{

class GlowProcess;

class GlowService : public Object
{
    friend class GlowProcess;

    ATOMIC_OBJECT(GlowService, Object)

public:
    /// Construct.
    GlowService(Context* context);
    /// Destruct.
    virtual ~GlowService();

    bool Start();

    bool Bake(const String& projectPath, Scene* scene, const GlowSettings& settings);

    void CancelBake();

    bool GetServiceExecutable(String& execPath, Vector<String>& baseArgs) const;

private:

    bool LocateServiceExecutable();

    void OnBakeError(const String& result);
    void OnBakeSuccess();

    void ProcessBakeData(VectorBuffer& bakeData);

    void OnGlowProcessExited();

    String glowBinaryPath_;
    StringVector glowBaseArgs_;

    WeakPtr<Scene> scene_;
    SharedPtr<GlowProcess> glowProcess_;

    String result_;
    bool bakeCanceled_;
    String projectPath_;

};

}
