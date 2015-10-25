//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Atomic/Core/Context.h>
#include <Atomic/Core/Object.h>

#include "CSComponent.h"

namespace Atomic
{

class NETCore;
class NETVariantMap;

typedef void (*NETUpdateFunctionPtr)(float timeStep);

typedef CSComponent* (*CSComponentCreateFunctionPtr)(const char* assemblyName, const char* csComponentTypeName);
typedef void (*CSComponentApplyFieldsPtr)(CSComponent* componentPtr, NETVariantMap* fieldMapPtr);

typedef void (*CSComponentCallMethodFunctionPtr)(unsigned id, CSComponentMethod method, float value);
typedef void (*CSBeginSendEventFunctionPtr)(unsigned senderRefID, unsigned eventType, VariantMap* eventData);

class ATOMIC_API CSManaged : public Object
{
    OBJECT(CSManaged);

public:
    /// Construct.
    CSManaged(Context* context);
    /// Destruct.
    virtual ~CSManaged();

    bool Initialize();

    void NETUpdate(float timeStep);

    CSComponent* CSComponentCreate(const String& assemblyName, const String& componentName);
    void CSComponentApplyFields(CSComponent* component, NETVariantMap* fieldMapPtr);


    void CSComponentCallMethod(unsigned id, CSComponentMethod methodID, float value = 0.0f);
    void CSBeginSendEvent(unsigned senderRefID, unsigned eventType, VariantMap* eventData);

private:

    CSComponentCreateFunctionPtr CSComponentCreate_;
    CSComponentApplyFieldsPtr CSComponentApplyFields_;
    CSComponentCallMethodFunctionPtr CSComponentCallMethod_;
    CSBeginSendEventFunctionPtr CSBeginSendEvent_;
    NETUpdateFunctionPtr NETUpdate_;

};

}
