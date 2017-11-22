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

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/Resource/ResourceCache.h>

#include <ToolCore/ToolSystem.h>
#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/Build/BuildSystem.h>
#include <ToolCore/License/LicenseEvents.h>
#include <ToolCore/License/LicenseSystem.h>
#include <ToolCore/Command/Command.h>
#include <ToolCore/Command/CommandParser.h>

#include "AtomicTool.h"

ATOMIC_DEFINE_APPLICATION_MAIN(AtomicTool::AtomicTool);

using namespace ToolCore;

namespace AtomicTool
{

AtomicTool::AtomicTool(Context* context) :
    Application(context),
    deactivate_(false)
{

}

AtomicTool::~AtomicTool()
{

}

void AtomicTool::Setup()
{
    const Vector<String>& arguments = GetArguments();

    if (arguments.Contains("-toolbootstrap"))
        ToolEnvironment::SetBootstrapping();

    engineParameters_["Headless"] = true;
    engineParameters_["LogLevel"] = LOG_INFO;

    for (unsigned i = 0; i < arguments.Size(); i++)
    {
        if (arguments[i].Length() > 1 && arguments[i][0] == '-')
        {
            String argument = arguments[i].Substring(1).ToLower();
            String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

            if (argument == "toolbootstrap")
            {
                ToolEnvironment::SetBootstrapping();
            }
            else if (argument == "loglevel")
            {
                engineParameters_["LogLevel"] = Variant(VariantType::VAR_INT, value);
                i++;
            }
            else if (argument == "logname")
            {
                engineParameters_["LogName"] = Variant(VariantType::VAR_STRING, value);
                i++;
            }
        }
    }

    // no default resources, AtomicTool may be run outside of source tree
    engineParameters_["ResourcePaths"] = "";
}

void AtomicTool::HandleCommandFinished(StringHash eventType, VariantMap& eventData)
{
    GetSubsystem<Engine>()->Exit();
}

void AtomicTool::HandleCommandError(StringHash eventType, VariantMap& eventData)
{
    String error = "Command Error";

    const String& message = eventData[CommandError::P_MESSAGE].ToString();
    if (message.Length())
        error = message;

    ErrorExit(error);
}

void AtomicTool::HandleLicenseEulaRequired(StringHash eventType, VariantMap& eventData)
{
    ErrorExit("\nActivation Required: Please run: atomic-cli activate\n");
}

void AtomicTool::HandleLicenseActivationRequired(StringHash eventType, VariantMap& eventData)
{
    ErrorExit("\nActivation Required: Please run: atomic-cli activate\n");
}

void AtomicTool::HandleLicenseSuccess(StringHash eventType, VariantMap& eventData)
{
    if (command_.Null())
    {
        GetSubsystem<Engine>()->Exit();
        return;
    }

    command_->Run();
}

void AtomicTool::HandleLicenseError(StringHash eventType, VariantMap& eventData)
{
    ErrorExit("\nActivation Required: Please run: atomic-cli activate\n");
}

void AtomicTool::HandleLicenseActivationError(StringHash eventType, VariantMap& eventData)
{
    String message = eventData[LicenseActivationError::P_MESSAGE].ToString();
    ErrorExit(message);
}

void AtomicTool::HandleLicenseActivationSuccess(StringHash eventType, VariantMap& eventData)
{
    ATOMIC_LOGRAW("\nActivation successful, thank you!\n\n");
    GetSubsystem<Engine>()->Exit();
}

void AtomicTool::DoActivation()
{
    LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();

    licenseSystem->LicenseAgreementConfirmed();

    SubscribeToEvent(E_LICENSE_ACTIVATIONERROR, ATOMIC_HANDLER(AtomicTool, HandleLicenseActivationError));
    SubscribeToEvent(E_LICENSE_ACTIVATIONSUCCESS, ATOMIC_HANDLER(AtomicTool, HandleLicenseActivationSuccess));

}

void AtomicTool::HandleLicenseDeactivationError(StringHash eventType, VariantMap& eventData)
{
    String message = eventData[LicenseDeactivationError::P_MESSAGE].ToString();
    ErrorExit(message);
}

void AtomicTool::HandleLicenseDeactivationSuccess(StringHash eventType, VariantMap& eventData)
{
    ATOMIC_LOGRAW("\nDeactivation successful\n\n");
    GetSubsystem<Engine>()->Exit();
}

void AtomicTool::DoDeactivation()
{
}

void AtomicTool::Start()
{
    // Subscribe to events
    SubscribeToEvent(E_COMMANDERROR, ATOMIC_HANDLER(AtomicTool, HandleCommandError));
    SubscribeToEvent(E_COMMANDFINISHED, ATOMIC_HANDLER(AtomicTool, HandleCommandFinished));

    SubscribeToEvent(E_LICENSE_EULAREQUIRED, ATOMIC_HANDLER(AtomicTool, HandleLicenseEulaRequired));
    SubscribeToEvent(E_LICENSE_ACTIVATIONREQUIRED, ATOMIC_HANDLER(AtomicTool, HandleLicenseActivationRequired));
    SubscribeToEvent(E_LICENSE_ERROR, ATOMIC_HANDLER(AtomicTool, HandleLicenseError));
    SubscribeToEvent(E_LICENSE_SUCCESS, ATOMIC_HANDLER(AtomicTool, HandleLicenseSuccess));

    const Vector<String>& arguments = GetArguments();

    ToolSystem* tsystem = new ToolSystem(context_);
    context_->RegisterSubsystem(tsystem);

    ToolEnvironment* env = new ToolEnvironment(context_);
    context_->RegisterSubsystem(env);

    // Initialize the ToolEnvironment
    if (!env->Initialize(true))
    {
        ErrorExit("Unable to initialize tool environment");
        return;
    }

    if (activationKey_.Length())
    {
        DoActivation();
        return;
    } else if (deactivate_)
    {
        DoDeactivation();
        return;
    }

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    SharedPtr<CommandParser> parser(new CommandParser(context_));

    SharedPtr<Command> cmd(parser->Parse(arguments));
    if (!cmd)
    {
        String error = "No command found";

        if (parser->GetErrorMessage().Length())
            error = parser->GetErrorMessage();

        ErrorExit(error);
        return;
    }

    if (cmd->RequiresProjectLoad())
    {
        if (!cmd->LoadProject())
        {
            ErrorExit(ToString("Failed to load project: %s", cmd->GetProjectPath().CString()));
            return;
        }

        String projectPath = cmd->GetProjectPath();

        // Set the build path
        String buildFolder = projectPath + "/" + "Build";
        buildSystem->SetBuildPath(buildFolder);

        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        if (!fileSystem->DirExists(buildFolder))
        {
            fileSystem->CreateDir(buildFolder);

            if (!fileSystem->DirExists(buildFolder))
            {
                ErrorExit(ToString("Failed to create build folder: %s", buildFolder.CString()));
                return;
            }
        }

    }

    command_ = cmd;

    // BEGIN LICENSE MANAGEMENT
    if (cmd->RequiresLicenseValidation())
    {
        GetSubsystem<LicenseSystem>()->Initialize();
    }
    else
    {
        if (command_.Null())
        {
            GetSubsystem<Engine>()->Exit();
            return;
        }

        command_->Run();
    }
    // END LICENSE MANAGEMENT

}

void AtomicTool::Stop()
{

}

void AtomicTool::ErrorExit(const String& message)
{
    engine_->Exit(); // Close the rendering window
    exitCode_ = EXIT_FAILURE;

    // Only for WIN32, otherwise the error messages would be double posted on Mac OS X and Linux platforms
    if (!message.Length())
    {
        #ifdef WIN32
        Atomic::ErrorExit(startupErrors_.Length() ? startupErrors_ :
            "Application has been terminated due to unexpected error.", exitCode_);
        #endif
    }
    else
        Atomic::ErrorExit(message, exitCode_);
}



}
