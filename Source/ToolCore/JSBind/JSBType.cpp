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
