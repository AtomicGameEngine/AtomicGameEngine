
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Engine/Engine.h>

#include <ToolCore/ToolSystem.h>

#include <ToolCore/License/LicenseSystem.h>

#include "AtomicTool.h"

using namespace ToolCore;

DEFINE_APPLICATION_MAIN(AtomicTool::AtomicTool);

namespace AtomicTool
{

AtomicTool::AtomicTool(Context* context) :
    Application(context)
{

}

AtomicTool::~AtomicTool()
{

}

void AtomicTool::Setup()
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

void AtomicTool::Start()
{
    context_->RegisterSubsystem(new ToolSystem(context_));

    // BEGIN LICENSE MANAGEMENT
    GetSubsystem<LicenseSystem>()->Initialize();
    // END LICENSE MANAGEMENT
}

void AtomicTool::Stop()
{

}


}



