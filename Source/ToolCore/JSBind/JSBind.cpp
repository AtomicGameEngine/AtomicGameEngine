
#include "JSBPackage.h"
#include "JSBind.h"

namespace ToolCore
{

JSBind::JSBind(Context* context) : Object(context)
{

}

JSBind::~JSBind()
{

}

bool JSBind::GenerateBindings(const String& sourceRootFolder, const String& packageFolder, const String& platform)
{
    sourceRootFolder_ = sourceRootFolder;
    packageFolder_ = packageFolder;
    platform_ = platform;

    SharedPtr<JSBPackage> package (new JSBPackage(context_));

    package->Load(sourceRootFolder_ + packageFolder_);

    return true;
}

}
