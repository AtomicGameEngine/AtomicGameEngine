
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

private:

    void HandleLicenseEulaRequired(StringHash eventType, VariantMap& eventData);
    void HandleLicenseActivationRequired(StringHash eventType, VariantMap& eventData);
    void HandleLicenseSuccess(StringHash eventType, VariantMap& eventData);
    void HandleLicenseError(StringHash eventType, VariantMap& eventData);

    void HandleCommandFinished(StringHash eventType, VariantMap& eventData);
    void HandleCommandError(StringHash eventType, VariantMap& eventData);

    String cliDataPath_;

    SharedPtr<Command> command_;

};

}
