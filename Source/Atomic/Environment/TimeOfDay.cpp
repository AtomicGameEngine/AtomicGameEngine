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

#include "Precompiled.h"
#include "../Core/Context.h"
#include "../Scene/Node.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneEvents.h"
#include "../Graphics/Light.h"
#include "../Environment/ProcSky.h"
#include "../Environment/TimeOfDay.h"


namespace Atomic
{

TimeOfDay::TimeOfDay(Context *context) : Component(context),
    timeOn_(0.0f),
    timeOff_(0.0f),
    on_(true)

{
}

TimeOfDay::~TimeOfDay()
{

}

void TimeOfDay::OnNodeSet(Node* node)
{
    Component::OnNodeSet(node);

    if (node && node->GetScene())
    {
        SubscribeToEvent(node->GetScene(), E_SCENEUPDATE, ATOMIC_HANDLER(TimeOfDay, HandleSceneUpdate));
    }

}

#include "stdio.h"

void TimeOfDay::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    float timeOfDay = ProcSky::GetTimeOfDay();

    if (timeOfDay >= timeOn_ || timeOfDay < timeOff_)
    {
        if (!on_)
        {
            PODVector<Component*> components;
            node_->GetComponents(components, Light::GetTypeStatic());
            for (unsigned i = 0; i < components.Size(); i++)
            {
                Light* light = (Light*) components[i];
                light->SetEnabled(true);
            }

        }

        on_ = true;
    }
    else
    {

        if (on_)
        {
            PODVector<Component*> components;
            node_->GetComponents(components, Light::GetTypeStatic());
            for (unsigned i = 0; i < components.Size(); i++)
            {
                Light* light = (Light*) components[i];
                light->SetEnabled(false);
            }

        }

        on_ = false;
    }

}

void TimeOfDay::RegisterObject(Context* context)
{
    context->RegisterFactory<TimeOfDay>();

    ATOMIC_ACCESSOR_ATTRIBUTE("TimeOn", GetTimeOn, SetTimeOn, float, 0.0f, AM_DEFAULT);
    ATOMIC_ACCESSOR_ATTRIBUTE("TimeOff", GetTimeOff, SetTimeOff, float, 0.0f, AM_DEFAULT);
}


}
