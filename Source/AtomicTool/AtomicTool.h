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

#include <Atomic/Engine/Application.h>

using namespace Atomic;

#include <ToolCore/Command/Command.h>

using namespace ToolCore;

namespace AtomicTool
{

class AtomicTool : public Application
{
    ATOMIC_OBJECT(AtomicTool, Application);

public:

    AtomicTool(Context* context);
    virtual ~AtomicTool();

    /// Setup before engine initialization. Verify that a script file has been specified.
    virtual void Setup();
    /// Setup after engine initialization. Load the script and execute its start function.
    virtual void Start();
    /// Cleanup after the main loop. Run the script's stop function if it exists.
    virtual void Stop();

    void ErrorExit(const String& message = String::EMPTY);

private:

    void HandleLicenseEulaRequired(StringHash eventType, VariantMap& eventData);
    void HandleLicenseActivationRequired(StringHash eventType, VariantMap& eventData);
    void HandleLicenseSuccess(StringHash eventType, VariantMap& eventData);
    void HandleLicenseError(StringHash eventType, VariantMap& eventData);

    void HandleLicenseActivationError(StringHash eventType, VariantMap& eventData);
    void HandleLicenseActivationSuccess(StringHash eventType, VariantMap& eventData);
    void HandleLicenseDeactivationError(StringHash eventType, VariantMap& eventData);
    void HandleLicenseDeactivationSuccess(StringHash eventType, VariantMap& eventData);

    void HandleCommandFinished(StringHash eventType, VariantMap& eventData);
    void HandleCommandError(StringHash eventType, VariantMap& eventData);

    void DoActivation();
    void DoDeactivation();

    // use a variant map instead?
    String cliDataPath_;
    String activationKey_;
    bool deactivate_;

    SharedPtr<Command> command_;

};

}
