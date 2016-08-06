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

/// New screen mode set.
ATOMIC_EVENT(E_SCREENMODE, ScreenMode)
{
    ATOMIC_PARAM(P_WIDTH, Width);                  // int
    ATOMIC_PARAM(P_HEIGHT, Height);                // int
    ATOMIC_PARAM(P_FULLSCREEN, Fullscreen);        // bool
    ATOMIC_PARAM(P_BORDERLESS, Borderless);        // bool
    ATOMIC_PARAM(P_RESIZABLE, Resizable);          // bool
    ATOMIC_PARAM(P_HIGHDPI, HighDPI);              // bool
}

/// Window position changed.
ATOMIC_EVENT(E_WINDOWPOS, WindowPos)
{
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
}

/// Request for queuing rendersurfaces either in manual or always-update mode.
ATOMIC_EVENT(E_RENDERSURFACEUPDATE, RenderSurfaceUpdate)
{
}

/// Frame rendering started.
ATOMIC_EVENT(E_BEGINRENDERING, BeginRendering)
{
}

/// Frame rendering ended.
ATOMIC_EVENT(E_ENDRENDERING, EndRendering)
{
}

/// Update of a view started.
ATOMIC_EVENT(E_BEGINVIEWUPDATE, BeginViewUpdate)
{
    ATOMIC_PARAM(P_VIEW, View);                    // View pointer
    ATOMIC_PARAM(P_TEXTURE, Texture);              // Texture pointer
    ATOMIC_PARAM(P_SURFACE, Surface);              // RenderSurface pointer
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_CAMERA, Camera);                // Camera pointer
}

/// Update of a view ended.
ATOMIC_EVENT(E_ENDVIEWUPDATE, EndViewUpdate)
{
    ATOMIC_PARAM(P_VIEW, View);                    // View pointer
    ATOMIC_PARAM(P_TEXTURE, Texture);              // Texture pointer
    ATOMIC_PARAM(P_SURFACE, Surface);              // RenderSurface pointer
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_CAMERA, Camera);                // Camera pointer
}

/// Render of a view started.
ATOMIC_EVENT(E_BEGINVIEWRENDER, BeginViewRender)
{
    ATOMIC_PARAM(P_VIEW, View);                    // View pointer
    ATOMIC_PARAM(P_TEXTURE, Texture);              // Texture pointer
    ATOMIC_PARAM(P_SURFACE, Surface);              // RenderSurface pointer
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_CAMERA, Camera);                // Camera pointer
}

/// Render of a view ended.
ATOMIC_EVENT(E_ENDVIEWRENDER, EndViewRender)
{
    ATOMIC_PARAM(P_VIEW, View);                    // View pointer
    ATOMIC_PARAM(P_TEXTURE, Texture);              // Texture pointer
    ATOMIC_PARAM(P_SURFACE, Surface);              // RenderSurface pointer
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_CAMERA, Camera);                // Camera pointer
}

/// A render path event has occurred.
ATOMIC_EVENT(E_RENDERPATHEVENT, RenderPathEvent)
{
    ATOMIC_PARAM(P_NAME, Name);                    // String
}

/// Graphics context has been lost. Some or all (depending on the API) GPU objects have lost their contents.
ATOMIC_EVENT(E_DEVICELOST, DeviceLost)
{
}

/// Graphics context has been recreated after being lost. GPU objects in the "data lost" state can be restored now.
ATOMIC_EVENT(E_DEVICERESET, DeviceReset)
{
}

}
