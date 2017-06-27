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

#pragma once

#include "UIBatch.h"
#include "UIWidget.h"

namespace Atomic
{

class Texture;
class Texture2D;
class VertexBuffer;
class UI;
class UIRenderer;
class UIComponent;

static int const UIVIEW_DEFAULT_TEXTURE_SIZE = 512;
static int const UIVIEW_MIN_TEXTURE_SIZE = 64;
static int const UIVIEW_MAX_TEXTURE_SIZE = 4096;

/// Top level UIView management
class ATOMIC_API UIView : public UIWidget
{
    friend class UI;

    ATOMIC_OBJECT(UIView, UIWidget)

public:

    UIView(Context* context);
    virtual ~UIView();

    /// Set the view size
    bool SetSize(int width, int height);

    /// Remove the UIView from the UI subsystem, readding removed views is not advised
    void Remove();

    /// Focuses the UIView for input events
    virtual void SetFocus();
    /// Gets whether this UIView is focused
    virtual bool GetFocus() const;
    /// Resigned this views focus, the bottom UIView with autofocus on will become focused
    void ResignFocus();

    /// Sets whether the view will autofocus when at the bottom of the view stack
    void SetAutoFocus(bool value) { autoFocus_ = value; }
    /// Gets whether the view will autofocus when at the bottom of the view stack
    bool GetAutoFocus() const { return autoFocus_; }

    /// Gets whether both mouse and keyboard input are enabled
    bool GetInputEnabled() const { return mouseEnabled_ || keyboardEnabled_; }
    /// Sets whether both mouse and keyboard input are enabled
    void SetInputEnabled(bool value) { mouseEnabled_ = keyboardEnabled_ = value; }

    /// Gets whether mouse input is enabled
    bool GetMouseEnabled() const { return mouseEnabled_; }
    /// Sets whether mouse input is enabled
    void SetMouseEnabled(bool value) { mouseEnabled_ = keyboardEnabled_ = value; }

    /// Gets whether keyboard input is enabled
    bool GetKeyboardEnabled() const { return keyboardEnabled_; }
    /// Sets whether keyboard input is enabled
    void SetKeyboardEnabled(bool value) { keyboardEnabled_ = value; }

    /// Set whether the UIView renders to texture, useful for 3D UI's, etc
    bool SetRenderToTexture(bool value, const int width = UIVIEW_DEFAULT_TEXTURE_SIZE, const int height = UIVIEW_DEFAULT_TEXTURE_SIZE);
    /// Gets the UIViews render texture, if any
    Texture2D* GetRenderTexture() { return renderTexture_; }
    /// Returns whether this UIView has a valid render texture
    bool HasRenderTexture() const { return renderTexture_.NotNull(); }

    /// Render the UI
    void Render(bool resetRenderTargets = true);

    /// Low level vertex data submission
    void SubmitBatchVertexData(Texture* texture, const PODVector<float>& vertexData);

protected:

private:

    bool FilterDefaultInput(bool keyEvent = false) const ;

    void BecomeFocused();

    void UpdateUIBatches();
    void GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor);
    void Render(VertexBuffer* buffer, const PODVector<UIBatch>& batches, unsigned batchStart, unsigned batchEnd);
    void SetVertexData(VertexBuffer* dest, const PODVector<float>& vertexData);

    void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData);
    void HandleMouseButtonUp(StringHash eventType, VariantMap& eventData);
    void HandleMouseMove(StringHash eventType, VariantMap& eventData);
    void HandleMouseWheel(StringHash eventType, VariantMap& eventData);
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    void HandleKeyUp(StringHash eventType, VariantMap& eventData);
    void HandleKey(bool keydown, int keycode, int scancode);
    void HandleTextInput(StringHash eventType, VariantMap& eventData);
    //Touch Input
    void HandleTouchBegin(StringHash eventType, VariantMap& eventData);
    void HandleTouchMove(StringHash eventType, VariantMap& eventData);
    void HandleTouchEnd(StringHash eventType, VariantMap& eventData);

    /// UI rendering batches.
    PODVector<UIBatch> batches_;
    /// UI rendering vertex data.
    PODVector<float> vertexData_;
    SharedPtr<VertexBuffer> vertexBuffer_;

    WeakPtr<Graphics> graphics_;
    UIRenderer* renderer_;
    WeakPtr<UI> ui_;
    WeakPtr<UIComponent> uiComponent_;

    SharedPtr<Texture2D> renderTexture_;

    bool autoFocus_;

    bool mouseEnabled_;
    bool keyboardEnabled_;

};

}
