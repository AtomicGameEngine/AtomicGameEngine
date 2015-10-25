//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
