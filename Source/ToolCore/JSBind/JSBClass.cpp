#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/JSONFile.h>

#include "JSBModule.h"
#include "JSBClass.h"
#include "JSBFunction.h"

namespace ToolCore
{

JSBClass::JSBClass(Context* context, JSBModule *module, const String& name, const String& nativeName) : Object(context),
    module_(module), name_(name), nativeName_(nativeName),
    isAbstract_(false)
{

}

JSBClass::~JSBClass()
{

}

}
