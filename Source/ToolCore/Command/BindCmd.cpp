
#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"

#include "../JSBind/JSBind.h"

#include "BindCmd.h"

namespace ToolCore
{

BindCmd::BindCmd(Context* context) : Command(context)
{

}

BindCmd::~BindCmd()
{

}

bool BindCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    sourceRootFolder_ = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;
    packageFolder_ = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;
    bindPlatform_ = startIndex + 3 < arguments.Size() ? arguments[startIndex + 3] : String::EMPTY;

    if (argument != "bind")
    {
        errorMsg = "Unable to parse bind command";
        return false;
    }

    if (!sourceRootFolder_.Length())
    {
        errorMsg = "Unable to parse bind command";
        return false;
    }


    if (!packageFolder_.Length())
    {
        errorMsg = "Unable to parse bind command";
        return false;
    }

    if (!bindPlatform_.Length())
    {
        errorMsg = "Unable to parse bind command";
        return false;
    }


    return true;
}

void BindCmd::Run()
{
    ToolEnvironment* env = GetSubsystem<ToolEnvironment>();

    //bindPlatform_ = "MACOSX";
    sourceRootFolder_ = env->GetRootSourceDir();
    //packageFolder_ = "Source/AtomicJS/Packages/Atomic/";

    SharedPtr<JSBind> jsbind(new JSBind(context_));

    context_->RegisterSubsystem(jsbind);

    LOGINFOF("Generating JS Bindings");

    jsbind->GenerateBindings(sourceRootFolder_, packageFolder_, bindPlatform_);

    Finished();

}

}
