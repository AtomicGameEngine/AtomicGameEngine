//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Physics/PhysicsEvents.h>
#include <Atomic/Script/ScriptPhysics.h>

#include "NETCore.h"
#include "NETEventDispatcher.h"

namespace Atomic
{


    NETEventDispatcher::NETEventDispatcher(Context* context) :
        Object(context)
    {

    }

    NETEventDispatcher::~NETEventDispatcher()
    {
    }

    void NETEventDispatcher::BeginSendEvent(Context* context, Object* sender, StringHash eventType, VariantMap& eventData)
    {
        if (eventType == E_UPDATE)
        {
            NETCore::DispatchUpdateEvent(eventData[Update::P_TIMESTEP].GetFloat());
        }

        if (!netEvents_.Contains(eventType))
            return;

        // Do any conversion that is necessary, will probably want to factor this into something better

        if (eventType == E_NODECOLLISION)
        {
            VariantMap ncEventData = eventData;

            SharedPtr<PhysicsNodeCollision> nodeCollison(new PhysicsNodeCollision());
            nodeCollison->SetFromNodeCollisionEvent(eventData);
            ncEventData[StringHash("PhysicsNodeCollision")] = nodeCollison;

            ncEventData[eventType] = ncEventData;

            NETCore::DispatchEvent(sender, eventType.Value(), &ncEventData);

            return;

        }

        NETCore::DispatchEvent(sender, eventType.Value(), &eventData);

    }

    void NETEventDispatcher::EndSendEvent(Context* context, Object* sender, StringHash eventType, VariantMap& eventData)
    {
        if (!netEvents_.Contains(eventType))
            return;
    }

}
