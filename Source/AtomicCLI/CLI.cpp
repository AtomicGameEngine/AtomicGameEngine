
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Engine/Engine.h>

#include "CLI.h"

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

    engineParameters_["Headless"] = true;
    engineParameters_["ResourcePaths"] = "";
}

void CLI::Start()
{

}

void CLI::Stop()
{

}


}



