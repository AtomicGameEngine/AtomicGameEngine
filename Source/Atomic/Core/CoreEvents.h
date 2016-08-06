//
// Copyright (c) 2008-2016 the Urho3D project.
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

#include "../Core/Object.h"

namespace Atomic
{

/// Frame begin event.
ATOMIC_EVENT(E_BEGINFRAME, BeginFrame)
{
    ATOMIC_PARAM(P_FRAMENUMBER, FrameNumber);      // unsigned
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Application-wide logic update event.
ATOMIC_EVENT(E_UPDATE, Update)
{
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Application-wide logic post-update event.
ATOMIC_EVENT(E_POSTUPDATE, PostUpdate)
{
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Render update event.
ATOMIC_EVENT(E_RENDERUPDATE, RenderUpdate)
{
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Post-render update event.
ATOMIC_EVENT(E_POSTRENDERUPDATE, PostRenderUpdate)
{
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Frame end event.
ATOMIC_EVENT(E_ENDFRAME, EndFrame)
{
}

// ATOMIC BEGIN
/// Updating paused or resumed event.
ATOMIC_EVENT(E_UPDATESPAUSEDRESUMED, UpdatesPaused)
{
    ATOMIC_PARAM(P_PAUSED, Paused);            // bool
}
// ATOMIC END

}
