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

#pragma once

#include <Atomic/Container/Str.h>

#include "../JSBFunctionWriter.h"

using namespace Atomic;

namespace ToolCore
{

class JSBPackage;
class JSBFunction;

class CSFunctionWriter : public JSBFunctionWriter
{

public:

    CSFunctionWriter(JSBFunction* function);

    void GenerateSource(String& sourceOut);

    void GenerateNativeSource(String& sourceOut);
    void GenerateManagedSource(String& sourceOut);


private:

    struct DefaultStructParameter
    {
        String type;
        String parameterName;
        String assignment;
    };

    Vector<DefaultStructParameter> defaultStructParameters_;

    void WriteDefaultStructParameters(String& source);

    void GenNativeCallParameters(String& sig);

    void WriteNativeFunction(String& source);
    void WriteNativeConstructor(String& source);
    void WriteNativeParameterMarshal(String& source);


    String MapDefaultParameter(JSBFunctionType* parameter);

    void GenManagedFunctionParameters(String& sig);

    void GenPInvokeCallParameters(String& sig);

    void WriteManagedConstructor(String& source);
    void WriteManagedDestructor(String& source);
    void WriteManagedFunction(String& source);

    void WriteManagedPInvokeFunctionSignature(String& source);

};

}
