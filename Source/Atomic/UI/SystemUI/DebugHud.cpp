//
// Copyright (c) 2017 the Atomic project.
// Copyright (c) 2008-2015 the Urho3D project.
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

#include "../../Core/CoreEvents.h"
#include "../../Core/Profiler.h"
#include "../../Engine/Engine.h"
#include "../../Graphics/Graphics.h"
#include "../../Graphics/Renderer.h"
#include "../../IO/Log.h"
#include "../../UI/UI.h"
#include "../../UI/SystemUI/SystemUI.h"
#include "../../UI/SystemUI/DebugHud.h"

#include "../../DebugNew.h"


namespace Atomic
{

static const char* qualityTexts[] =
{
    "Low",
    "Med",
    "High"
};

static const char* shadowQualityTexts[] =
{
    "16bit Low",
    "24bit Low",
    "16bit High",
    "24bit High"
};

static const float FPS_UPDATE_INTERVAL = 0.5f;

DebugHud::DebugHud(Context* context) :
    Object(context),
    profilerMaxDepth_(M_MAX_UNSIGNED),
    profilerMode_(DEBUG_HUD_PROFILE_PERFORMANCE),
    profilerInterval_(1000),
    useRendererStats_(true),
    mode_(DEBUGHUD_SHOW_NONE),
    fpsTimeSinceUpdate_(FPS_UPDATE_INTERVAL),
    fpsFramesSinceUpdate_(0),
    fps_(0)
{
    ResetExtents();
    RecalculateWindowPositions();
    SubscribeToEvent(E_SYSTEMUIFRAME, ATOMIC_HANDLER(DebugHud, RenderUi));
}

DebugHud::~DebugHud()
{
    UnsubscribeFromAllEvents();
}

// TODO: get rid of `useRootExtents` and replace calls to SetExtents(true, ...) with ResetExtents().
void DebugHud::SetExtents(bool useRootExtents, const IntVector2& position, const IntVector2& size)
{
    if (useRootExtents)
        ResetExtents();
    else
    {
        auto bottomRight = position + size;
        extents_ = IntRect(position.x_, position.y_, bottomRight.x_, bottomRight.y_);
    }
    RecalculateWindowPositions();
}

void DebugHud::ResetExtents()
{
//    auto uiRoot = GetSubsystem<UI>()->GetRootWidget();
//    auto topLeft = uiRoot->GetPosition();
//    auto bottomRight = uiRoot->GetPosition() + uiRoot->GetSize();
//    extents_ = IntRect(topLeft.x_, topLeft.y_, bottomRight.x_, bottomRight.y_);
    auto graphics = GetSubsystem<Graphics>();
    extents_ = IntRect(0, 0, graphics->GetWidth(), graphics->GetHeight());
}

void DebugHud::RecalculateWindowPositions()
{
    posMode_ = WithinExtents({0, -30});
    posStats_ = WithinExtents({0, 0});
    posProfiler_ = WithinExtents({-530, 0});
}

void DebugHud::SetProfilerMode(DebugHudProfileMode mode)
{ 
    profilerMode_ = mode;
}

void DebugHud::SetMode(unsigned mode)
{
    mode_ = mode;
}

void DebugHud::CycleMode()
{
    switch (mode_)
    {
    case DEBUGHUD_SHOW_NONE:
        SetMode(DEBUGHUD_SHOW_STATS);
        break;
    case DEBUGHUD_SHOW_STATS:
        SetMode(DEBUGHUD_SHOW_MODE);
        break;
    case DEBUGHUD_SHOW_MODE:
        SetMode(DEBUGHUD_SHOW_PROFILER);
        break;
    case DEBUGHUD_SHOW_PROFILER:
        SetMode(DEBUGHUD_SHOW_ALL);
        break;
    case DEBUGHUD_SHOW_ALL:
    default:
        SetMode(DEBUGHUD_SHOW_NONE);
        break;
    }
}

void DebugHud::SetProfilerMaxDepth(unsigned depth)
{
    profilerMaxDepth_ = depth;
}

void DebugHud::SetProfilerInterval(float interval)
{
    profilerInterval_ = (unsigned)Max((int)(interval * 1000.0f), 0);
}

void DebugHud::SetUseRendererStats(bool enable)
{
    useRendererStats_ = enable;
}

void DebugHud::Toggle(unsigned mode)
{
    SetMode(GetMode() ^ mode);
}

void DebugHud::ToggleAll()
{
    Toggle(DEBUGHUD_SHOW_ALL);
}

float DebugHud::GetProfilerInterval() const
{
    return (float)profilerInterval_ / 1000.0f;
}

void DebugHud::SetAppStats(const String& label, const Variant& stats)
{
    SetAppStats(label, stats.ToString());
}

void DebugHud::SetAppStats(const String& label, const String& stats)
{
    bool newLabel = !appStats_.Contains(label);
    appStats_[label] = stats;
    if (newLabel)
        appStats_.Sort();
}

bool DebugHud::ResetAppStats(const String& label)
{
    return appStats_.Erase(label);
}

void DebugHud::ClearAppStats()
{
    appStats_.Clear();
}

IntVector2 DebugHud::WithinExtents(IntVector2 pos)
{
    if (pos.x_ < 0)
        pos.x_ += extents_.right_;
    else if (pos.x_ > 0)
        pos.x_ += extents_.left_;
    else
        pos.x_ = extents_.left_;

    if (pos.y_ < 0)
        pos.y_ += extents_.bottom_;
    else if (pos.y_ > 0)
        pos.y_ += extents_.top_;
    else
        pos.y_ = extents_.top_;

    return pos;
};

void DebugHud::RenderUi(StringHash eventType, VariantMap& eventData)
{
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    const auto backgroundTextWindowFlags = ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoTitleBar|
            ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoInputs;

    if (mode_ & DEBUGHUD_SHOW_MODE)
    {
        ImGui::SetNextWindowPos(ImVec2(posMode_.x_, posMode_.y_));
        if (ImGui::Begin("DebugHud mode", 0, ImVec2(1024, 30), 0, backgroundTextWindowFlags))
        {
            ImGui::Text("Tex:%s Mat:%s Spec:%s Shadows:%s Size:%i Quality:%s Occlusion:%s Instancing:%s API:%s",
                        qualityTexts[renderer->GetTextureQuality()],
                        qualityTexts[renderer->GetMaterialQuality()],
                        renderer->GetSpecularLighting() ? "On" : "Off",
                        renderer->GetDrawShadows() ? "On" : "Off",
                        renderer->GetShadowMapSize(),
                        shadowQualityTexts[renderer->GetShadowQuality()],
                        renderer->GetMaxOccluderTriangles() > 0 ? "On" : "Off",
                        renderer->GetDynamicInstancing() ? "On" : "Off",
                        graphics->GetApiName().CString());
        }
        ImGui::End();
    }

    if (mode_ & DEBUGHUD_SHOW_STATS)
    {
        fpsTimeSinceUpdate_ += GetSubsystem<Time>()->GetTimeStep();
        ++fpsFramesSinceUpdate_;
        if (fpsTimeSinceUpdate_ > FPS_UPDATE_INTERVAL)
        {
            fps_ = (int)(fpsFramesSinceUpdate_ / fpsTimeSinceUpdate_);
            fpsFramesSinceUpdate_ = 0;
            fpsTimeSinceUpdate_ = 0;
        }

        unsigned primitives, batches;
        if (!useRendererStats_)
        {
            primitives = graphics->GetNumPrimitives();
            batches = graphics->GetNumBatches();
        }
        else
        {
            primitives = renderer->GetNumPrimitives();
            batches = renderer->GetNumBatches();
        }

        String stats;

        unsigned singlePassPrimitives = graphics->GetSinglePassPrimitives();
        unsigned editorPrimitives = graphics->GetNumPrimitives() - renderer->GetNumPrimitives();

        ImGui::SetNextWindowPos(ImVec2(posStats_.x_, posStats_.y_));
        if (ImGui::Begin("DebugHud Stats", 0, ImVec2(0, 0), 0,
                         backgroundTextWindowFlags))
        {
            ImGui::Text("FPS %d", fps_);
            if (singlePassPrimitives)
            {
                ImGui::Text("Triangles (All passes) %u", primitives);
                ImGui::Text("Triangles (Single pass) %u", singlePassPrimitives);
                ImGui::Text("Triangles (Editor) %u", editorPrimitives);
            }
            else
                ImGui::Text("Triangles %u", primitives);
            ImGui::Text("Batches %u", batches);
            ImGui::Text("Views %u", renderer->GetNumViews());
            ImGui::Text("Lights %u", renderer->GetNumLights(true));
            ImGui::Text("Shadowmaps %u", renderer->GetNumShadowMaps(true));
            ImGui::Text("Occluders %u", renderer->GetNumOccluders(true));

            for (HashMap<String, String>::ConstIterator i = appStats_.Begin(); i != appStats_.End(); ++i)
                ImGui::Text("%s %s", i->first_.CString(), i->second_.CString());
        }
        ImGui::End();
    }

    if (mode_ & DEBUGHUD_SHOW_PROFILER)
    {
        ImGui::SetNextWindowPos(ImVec2(posProfiler_.x_, posProfiler_.y_));
        ImGui::SetNextWindowSize(ImVec2(sizeProfiler_.x_, sizeProfiler_.y_));
        if (ImGui::Begin("DebugHud Metrics", 0, ImVec2(0, 0), 0, backgroundTextWindowFlags))
        {
            if (profilerTimer_.GetMSec(false) >= profilerInterval_)
            {
                if (profilerMode_ == DEBUG_HUD_PROFILE_PERFORMANCE)
                {
                    profilerTimer_.Reset();
                    Profiler* profiler = GetSubsystem<Profiler>();
                    if (profiler)
                    {
                        profilerOutput_ = profiler->PrintData(false, false, profilerMaxDepth_);
                        profiler->BeginInterval();
                    }
                }
                else
                {
                    Metrics* metrics = GetSubsystem<Metrics>();
                    if (metrics)
                    {
                        if (metrics->GetEnabled())
                        {
                            metricsSnapshot_.Clear();
                            metrics->Capture(&metricsSnapshot_);
                            profilerOutput_ = metricsSnapshot_.PrintData(2);
                        }
                        else
                            profilerOutput_ = "Metrics system not enabled";
                    }
                    else
                        profilerOutput_ = "Metrics subsystem not found";
                }
                auto size = ImGui::CalcTextSize(profilerOutput_.CString());
                sizeProfiler_ = IntVector2((int)size.x + 20, (int)size.y + 20);
                posProfiler_ = WithinExtents({-((int)size.x + 20), 0});
            }
            ImGui::TextUnformatted(profilerOutput_.CString());
        }
        ImGui::End();
    }
}

}
