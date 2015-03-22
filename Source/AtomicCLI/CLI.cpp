
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Engine/Engine.h>

#include <ToolCore/ToolSystem.h>

#include <ToolCore/License/LicenseSystem.h>

#include "CLI.h"

using namespace ToolCore;

DEFINE_APPLICATION_MAIN(AtomicCLI::CLI);

namespace AtomicCLI
{

CLI::CLI(Context* context) :
    Application(context)
{

}

CLI::~CLI()
{

}

void CLI::Setup()
{
    const Vector<String>& arguments = GetArguments();

    for (unsigned i = 0; i < arguments.Size(); ++i)
    {
        if (arguments[i].Length() > 1 /* && arguments[i][0] == '-'*/)
        {
            String argument = arguments[i].Substring(1).ToLower();
            String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

            LOGINFOF("%s", argument.CString());
        }
    }

    engineParameters_["Headless"] = true;
    engineParameters_["ResourcePaths"] = "";
}

void CLI::Start()
{
    context_->RegisterSubsystem(new ToolSystem(context_));

    // BEGIN LICENSE MANAGEMENT
    GetSubsystem<LicenseSystem>()->Initialize();
    // END LICENSE MANAGEMENT
}

void CLI::Stop()
{

}


}



