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

#include <Atomic/Core/Object.h>
#include <Atomic/Core/Context.h>

namespace Atomic
{

class ATOMIC_API CSEventDispatcher : public Object, public GlobalEventListener
{
    OBJECT(CSEventDispatcher);

public:
    /// Construct.
    CSEventDispatcher(Context* context);
    /// Destruct.
    virtual ~CSEventDispatcher();

private:

    void BeginSendEvent(Context* context, Object* sender, StringHash eventType, VariantMap& eventData);
    void EndSendEvent(Context* context, Object* sender, StringHash eventType, VariantMap& eventData);

};

/*
class ATOMIC_API CSEventHelper : public Object
{
    OBJECT(CSEventHelper);

public:
    /// Construct.
    CSEventHelper(Context* context, Object* object);
    /// Destruct.
    virtual ~CSEventHelper();

    void AddEventHandler(StringHash eventType);
    void AddEventHandler(Object* sender, StringHash eventType);

private:

    void HandleEvent(StringHash eventType, VariantMap& eventData);

    WeakPtr<Object> object_;

};
*/


}
