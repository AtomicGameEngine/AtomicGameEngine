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

#pragma once

#include "../Graphics/Camera.h"
#include "../Graphics/Material.h"
#include "../Graphics/Technique.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/Octree.h"
#include "../Graphics/VertexBuffer.h"
#include "../Resource/ResourceCache.h"

#include "UIWidget.h"

namespace Atomic
{

class UI;

/// a root widget that renders offscreen
class UIOffscreenView : public UIWidget
{
    friend UI;

    ATOMIC_OBJECT(UIOffscreenView, UIWidget)

public:

    /// Construct
    UIOffscreenView(Context* context);

    /// Destruct
    virtual ~UIOffscreenView();

    /// Get the current width of the texture.
    int GetWidth() const { return GetTexture2D()->GetWidth(); }
    /// Get the current height of the texture.
    int GetHeight() const { return GetTexture2D()->GetHeight(); }
    /// Set the dimensions of the texture.
    void SetSize(int width, int height);
    /// Get the Texture2D to which this view is rendering.
    Texture2D* GetTexture2D() const { return texture_; }
    /// Return render surface.
    RenderSurface* GetRenderSurface() const { return GetTexture2D()->GetRenderSurface(); }
    /// Get a material for this view (for convenience only/doesn't have to be used).
    Material* GetMaterial();
    /// Register instances nessisary to process input events automatically.
    void RegisterInput(Camera* camera, Octree* octree, Drawable* drawable, const IntRect& rect = IntRect::ZERO);
    /// Unregister instances to return to manual input.
    void UnregisterInput();
    /// Get wether to clear the render target each frame.
    bool GetClearRenderTargetEachFrame() const { return clearRenderTargetEachFrame_; }
    /// Set wether to clear the render target each frame.
    void SetClearRenderTargetEachFrame(bool clear) { clearRenderTargetEachFrame_ = clear; }
    /// Save PNG to a serializer.
    bool SavePNG(Serializer& dest) const;
    /// Save PNG to a file.
    bool SavePNG(const String& fileName) const;

    void InvokeRightPointerDown(int x, int y, int click_count, int qualifiers, bool superKeyDown = false);
    void InvokeRightPointerUp(int x, int y, int qualifiers, bool superKeyDown = false);
    void InvokePointerDown(int x, int y, int click_count, int qualifiers, bool touch, int touchId, bool superKeyDown = false);
    void InvokePointerUp(int x, int y, int qualifiers, bool touch, int touchId, bool superKeyDown = false);
    void InvokePointerMove(int x, int y, int qualifiers, bool touch, int touchId, bool superKeyDown = false);
    void InvokeWheel(int x, int y, int delta_x, int delta_y, int qualifiers, bool superKeyDown = false);
    bool InvokeKey(int key, int keycode, bool down, int qualifiers, bool superKeyDown = false);

protected:

    SharedPtr<Texture2D> texture_;
    SharedPtr<Material> material_;
    PODVector<UIBatch> batches_;
    PODVector<float> vertexData_;
    SharedPtr<VertexBuffer> vertexBuffer_;
    SharedPtr<Camera> inputCamera_;
    SharedPtr<Octree> inputOctree_;
    SharedPtr<Drawable> inputDrawable_;
    IntRect inputRect_;
    bool clearRenderTargetEachFrame_;

private:

};

}
