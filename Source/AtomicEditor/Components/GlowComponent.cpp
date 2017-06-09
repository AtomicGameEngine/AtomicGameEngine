//
// Copyright (c) 2014-2017 THUNDERBEAST GAMES LLC
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
#include <Atomic/Core/Context.h>

#include <ToolCore/ToolSystem.h>
#include <ToolCore/Project/Project.h>

#include <AtomicGlow/GlowService/GlowService.h>
#include "GlowComponent.h"

using namespace ToolCore;
using namespace AtomicGlow;

namespace AtomicEditor
{

GlowComponent::GlowComponent(Context *context) : EditorComponent(context)
{
    GlowSettings glowSettings;
    glowSettings.SetDefaults();
    SetFromGlowSettings(glowSettings);
}

GlowComponent::~GlowComponent()
{

}

void GlowComponent::SetFromGlowSettings(const GlowSettings& settings)
{

    lexelDensity_ = settings.lexelDensity_;

    giEnabled_ = settings.giEnabled_;
    giGranularity_ = settings.giGranularity_;
    giMaxBounces_ = settings.giMaxBounces_;

    aoEnabled_ = settings.aoEnabled_;
    aoDepth_ = settings.aoDepth_;
    nsamples_ = settings.nsamples_;
    aoMin_ = settings.aoMin_;
    aoMultiply_ = settings.aoMultiply_;

}

void GlowComponent::CopyToGlowSettings(GlowSettings& settings) const
{
    settings.lexelDensity_ = lexelDensity_ ;
    settings.sceneLexelDensityScale_ = 1.0f;

    settings.giEnabled_= giEnabled_;
    settings.giGranularity_ = giGranularity_;
    settings.giMaxBounces_ = giMaxBounces_;

    settings.aoEnabled_ = aoEnabled_;
    settings.aoDepth_ = aoDepth_;
    settings.nsamples_ = nsamples_;
    settings.aoMin_ = aoMin_;
    settings.aoMultiply_ = aoMultiply_;
}

bool GlowComponent::Bake()
{

    GlowService* glowService = GetSubsystem<GlowService>();

    if (!glowService)
    {
        ATOMIC_LOGERROR("GlowComponent::Bake() - Unable ot get glow service");
        return false;
    }

    SubscribeToEvent(E_ATOMICGLOWSERVICEBAKERESULT, ATOMIC_HANDLER(GlowComponent, HandleAtomicGlowServiceBakeResult));
    SubscribeToEvent(E_ATOMICGLOWSERVICELOGEVENT, ATOMIC_HANDLER(GlowComponent, HandleAtomicGlowServiceLogEvent));
    SubscribeToEvent(E_ATOMICGLOWBAKECANCEL, ATOMIC_HANDLER(GlowComponent, HandleAtomicGlowBakeCancel));

    GlowSettings settings;
    CopyToGlowSettings(settings);
    settings.Validate();
    SetFromGlowSettings(settings);

    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    if (!toolSystem)
        return false;

    Project* project = toolSystem->GetProject();
    if (!project)
        return false;

    return glowService->Bake(project->GetProjectPath(), GetScene(), settings);
}

void GlowComponent::HandleAtomicGlowBakeCancel(StringHash eventType, VariantMap& eventData)
{
    GetSubsystem<GlowService>()->CancelBake();
}

void GlowComponent::HandleAtomicGlowServiceBakeResult(StringHash eventType, VariantMap& eventData)
{
    using namespace AtomicGlowServiceBakeResult;

    // convert to a glow component event, which contains the same fields
    SendEvent(E_ATOMICGLOWBAKERESULT, eventData);

}

void GlowComponent::HandleAtomicGlowServiceLogEvent(StringHash eventType, VariantMap& eventData)
{
    using namespace AtomicGlowServiceLogEvent;

    // convert to a glow component event, which contains the same fields
    SendEvent(E_ATOMICGLOWLOGEVENT, eventData);

}

void GlowComponent::RegisterObject(Context* context)
{
    context->RegisterFactory<GlowComponent>();

    ATOMIC_ATTRIBUTE("Lexel Density", float, lexelDensity_, 0.1f, AM_FILE);

    ATOMIC_ATTRIBUTE("GI Enabled", bool, giEnabled_, false, AM_FILE);
    ATOMIC_ATTRIBUTE("GI Granularity", int, giGranularity_, 16, AM_FILE);
    ATOMIC_ATTRIBUTE("GI Max Cycles", int, giMaxBounces_, 3, AM_FILE);

    ATOMIC_ATTRIBUTE("AO Enabled", bool, aoEnabled_, false, AM_FILE);
    ATOMIC_ATTRIBUTE("AO Depth", float, aoDepth_, 0.25f, AM_FILE);
    ATOMIC_ATTRIBUTE("AO Samples", int, nsamples_, 64, AM_FILE);
    ATOMIC_ATTRIBUTE("AO Min", float, aoMin_, 0.45f, AM_FILE);
    ATOMIC_ATTRIBUTE("AO Multiply", float, aoMultiply_, 1.0f, AM_FILE);

}


}
