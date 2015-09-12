//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
    OBJECT(AtomicTool);

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
