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

#pragma once

#include "../../Core/Object.h"
#include "../../Core/Timer.h"
#include "../../Metrics/Metrics.h"

#include "../UIEnums.h"

namespace Atomic
{

static const unsigned DEBUGHUD_SHOW_NONE = 0x0;
static const unsigned DEBUGHUD_SHOW_STATS = 0x1;
static const unsigned DEBUGHUD_SHOW_MODE = 0x2;
static const unsigned DEBUGHUD_SHOW_PROFILER = 0x4;
static const unsigned DEBUGHUD_SHOW_ALL = 0x7;

/// Displays rendering stats and profiling information.
class ATOMIC_API DebugHud : public Object
{
    ATOMIC_OBJECT(DebugHud, Object)

public:
    /// Construct.
    DebugHud(Context* context);
    /// Destruct.
    ~DebugHud();

    /// Set elements to show.
    void SetMode(unsigned mode);
    /// Cycle through elements
    void CycleMode();
    /// Set maximum profiler block depth, default unlimited.
    void SetProfilerMaxDepth(unsigned depth);
    /// Set profiler accumulation interval in seconds.
    void SetProfilerInterval(float interval);
    /// Set the profiler mode to either performance or metrics
    void SetProfilerMode(DebugHudProfileMode mode);
    /// Set whether to show 3D geometry primitive/batch count only. Default false.
    void SetUseRendererStats(bool enable);
    /// Toggle elements.
    void Toggle(unsigned mode);
    /// Toggle all elements.
    void ToggleAll();

    /// Return currently shown elements.
    unsigned GetMode() const { return mode_; }

    /// Return the profiler mode (performance, metrics, etc)
    DebugHudProfileMode GetProfilerMode() const { return profilerMode_; }

    /// Return maximum profiler block depth.
    unsigned GetProfilerMaxDepth() const { return profilerMaxDepth_; }

    /// Return profiler accumulation interval in seconds
    float GetProfilerInterval() const;

    /// Return whether showing 3D geometry primitive/batch count only.
    bool GetUseRendererStats() const { return useRendererStats_; }

    /// Set application-specific stats.
    void SetAppStats(const String& label, const Variant& stats);
    /// Set application-specific stats.
    void SetAppStats(const String& label, const String& stats);
    /// Reset application-specific stats. Return true if it was erased successfully.
    bool ResetAppStats(const String& label);
    /// Clear all application-specific stats.
    void ClearAppStats();

    void SetExtents(bool useRootExtents = true, const IntVector2& position = IntVector2::ZERO, const IntVector2& size = IntVector2::ZERO);
    void ResetExtents();

private:
    /// Render system ui.
    void RenderUi(StringHash eventType, VariantMap& eventData);
    void RecalculateWindowPositions();
    IntVector2 WithinExtents(IntVector2 pos);

    /// Hashmap containing application specific stats.
    HashMap<String, String> appStats_;
    /// Profiler timer.
    Timer profilerTimer_;
    /// Profiler max block depth.
    unsigned profilerMaxDepth_;
    /// Profiler mode 
    DebugHudProfileMode profilerMode_;
    /// Profiler accumulation interval.
    unsigned profilerInterval_;
    /// Show 3D geometry primitive/batch count flag.
    bool useRendererStats_;
    /// Current shown-element mode.
    unsigned mode_;
    /// Time since last fps display update
    float fpsTimeSinceUpdate_;
    /// Frames since last fps display update
    float fpsFramesSinceUpdate_;
    /// Calculated fps
    unsigned fps_;
    /// Cached profiler output.
    String profilerOutput_;
    /// Metrics data snapshot.
    MetricsSnapshot metricsSnapshot_;
    /// DebugHud extents that data will be rendered in.
    IntRect extents_;
    IntVector2 posMode_;
    IntVector2 posStats_;
    IntVector2 posProfiler_;
    IntVector2 sizeProfiler_;
};

}
