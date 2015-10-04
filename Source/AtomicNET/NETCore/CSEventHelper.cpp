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

#include <Atomic/IO/Log.h>
#include <Atomic/UI/UIEvents.h>

//#include "AtomicSharpAPI.h"
#include "CSEventHelper.h"

namespace Atomic
{


CSEventDispatcher::CSEventDispatcher(Context* context) :
    Object(context)
{

}

CSEventDispatcher::~CSEventDispatcher()
{
}

void CSEventDispatcher::BeginSendEvent(Context* context, Object* sender, StringHash eventType, VariantMap& eventData)
{
    // change to setting a callback
    // CSBeginSendEvent(sender->GetRefID(), eventType.ToHash(), &eventData);
}

void CSEventDispatcher::EndSendEvent(Context* context, Object* sender, StringHash eventType, VariantMap& eventData)
{

}

/*
CSEventHelper::CSEventHelper(Context* context, Object* object) :
    Object(context),
    object_(object)
{

}

CSEventHelper::~CSEventHelper()
{

}

void CSEventHelper::AddEventHandler(StringHash eventType)
{
    GetSubsystem<CSEventDispatcher>()->RegisterCSEvent(eventType);

    // subscribe using object, so unsubscribing from object and not the event helper works
    object_->SubscribeToEvent(eventType, HANDLER(CSEventHelper, HandleEvent));
}

void CSEventHelper::AddEventHandler(Object* sender, StringHash eventType)
{
    GetSubsystem<CSEventDispatcher>()->RegisterCSEvent(eventType);

    // subscribe using object, so unsubscribing from object and not the event helper works
    object_->SubscribeToEvent(sender, eventType, HANDLER(CSEventHelper, HandleEvent));
}

void CSEventHelper::HandleEvent(StringHash eventType, VariantMap& eventData)
{
    if (object_.Null())
        return;
}
*/


}
