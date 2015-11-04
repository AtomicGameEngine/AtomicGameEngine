//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include "JSBPackage.h"

#include "JSBType.h"

namespace ToolCore
{

JSBType* JSBType::Parse(const String& value)
{
    JSBType* type = NULL;

    if (JSBPackage::GetClassAllPackages(value))
    {
        type = new JSBClassType(JSBPackage::GetClassAllPackages(value));
    }
    else if (JSBPackage::GetEnumAllPackages(value))
    {
        type = new JSBEnumType(JSBPackage::GetEnumAllPackages(value));
    }
    else if (value == "int")
    {
        type = new JSBPrimitiveType(JSBPrimitiveType::Int);
    }
    else if (value == "bool")
    {
        type = new JSBPrimitiveType(JSBPrimitiveType::Bool);
    }
    else if (value == "unsigned")
    {
        type = new JSBPrimitiveType(JSBPrimitiveType::Int, true);

    }
    else if (value == "float")
    {
        type = new JSBPrimitiveType(JSBPrimitiveType::Float);
    }
    else if (value == "String")
    {
        type = new JSBStringType();

    }
    else if (value == "StringHash")
    {
        type = new JSBStringHashType();

    }
    return type;

}

}
