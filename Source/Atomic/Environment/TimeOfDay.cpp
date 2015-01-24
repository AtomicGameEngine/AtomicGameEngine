
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
        SubscribeToEvent(node->GetScene(), E_SCENEUPDATE, HANDLER(TimeOfDay, HandleSceneUpdate));
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

    ACCESSOR_ATTRIBUTE("TimeOn", GetTimeOn, SetTimeOn, float, 0.0f, AM_DEFAULT);
    ACCESSOR_ATTRIBUTE("TimeOff", GetTimeOff, SetTimeOff, float, 0.0f, AM_DEFAULT);
}


}
