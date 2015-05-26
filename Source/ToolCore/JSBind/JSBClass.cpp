#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/JSONFile.h>

#include "JSBModule.h"
#include "JSBClass.h"
#include "JSBFunction.h"

namespace ToolCore
{

JSBFunctionOverride::JSBFunctionOverride(const String &name, const Vector<String>& sig) : parsed_(false)
{
    name_ = name;
    sig_ = sig;
}

void JSBFunctionOverride::Parse()
{
    if (parsed_)
        return;

    parsed_ = true;

    for (unsigned i = 0; i < sig_.Size(); i++)
    {
        JSBType* type = JSBType::Parse(sig_.At(i));
        if (!type)
        {
            ErrorExit("Unable to parse override type");
        }
        types_.Push(type);
    }

}

JSBClass::JSBClass(Context* context, JSBModule *module, const String& name, const String& nativeName) : Object(context),
    module_(module), name_(name), nativeName_(nativeName),
    isAbstract_(false), isObject_(false),
    numberArrayElements_(0), arrayElementType_("float")
{

    if (nativeName == "Object")
        SetObject(true);

    // detect array type, TODO: this could go in module json

    if (name_ == "Color")
        numberArrayElements_ = 4;
    else if (name_ == "Vector2")
        numberArrayElements_ = 2;
    else if (name_ == "Vector3")
        numberArrayElements_ = 3;
    else if (name_ == "Vector4")
        numberArrayElements_ = 4;
    else if (name_ == "Quaternion")
        numberArrayElements_ = 4;
    else if (name_ == "BoundingBox")
        numberArrayElements_ = 6;
    else if (name_ == "Rect")
        numberArrayElements_ = 4;
    else if (name_ == "IntRect")
    {
        numberArrayElements_ = 4;
        arrayElementType_ = "int";
    }
    else if (name_ == "IntVector2")
    {
        numberArrayElements_ = 2;
        arrayElementType_ = "int";
    }

}

JSBClass::~JSBClass()
{

}

void JSBClass::AddFunction(JSBFunction* function)
{
    functions_.Push(function);
}

void JSBClass::AddBaseClass(JSBClass *baseClass)
{
    baseClasses_.Push(baseClass);

    if (baseClass->GetNativeName() == "Object")
        SetObject(true);
}

void JSBClass::Dump()
{
    if (name_ != nativeName_)
    {
        LOGINFOF("Class: %s (%s)", name_.CString(), nativeName_.CString());
    }
    else
    {
        LOGINFOF("Class: %s", name_.CString());
    }

    LOGINFOF("   IsObject: %s", IsObject() ? "true" : "false");

    if (baseClasses_.Size())
    {
        LOGINFOF("   Bases:");
        for (unsigned i = 0; i < baseClasses_.Size(); i++)
        {

            LOGINFOF("      %s", baseClasses_.At(i)->GetName().CString());
        }
    }

    if (functions_.Size())
    {
        for (unsigned i = 0; i < functions_.Size(); i++)
        {
            functions_.At(i)->Dump();
        }
    }

}


}
