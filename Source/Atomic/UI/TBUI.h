#pragma once

#ifdef ATOMIC_TBUI

#include "UIElement.h"

namespace tb
{

class TBWidget;

}

namespace Atomic
{

class VertexBuffer;

/// %UI element which renders a 3D scene.
class ATOMIC_API TBUI : public UIElement
{
    OBJECT(TBUI);

    tb::TBWidget* rootWidget_;
    bool initialized_;    

    void HandleScreenMode(StringHash eventType, VariantMap& eventData);
    void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData);
    void HandleMouseButtonUp(StringHash eventType, VariantMap& eventData);
    void HandleMouseMove(StringHash eventType, VariantMap& eventData);
    void HandleMouseWheel(StringHash eventType, VariantMap& eventData);
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    void HandleKeyUp(StringHash eventType, VariantMap& eventData);
    void HandleKey(bool keydown, int keycode, int scancode);
    void HandleTextInput(StringHash eventType, VariantMap& eventData);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleRenderUpdate(StringHash eventType, VariantMap& eventData);

public:
    /// Construct.
    TBUI(Context* context);
    /// Destruct.
    ~TBUI();

    tb::TBWidget* GetRootWidget() { return rootWidget_; }

    bool LoadResourceFile(tb::TBWidget* widget, const String& filename);

    void Render();

    void GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor);

    void SubmitBatchVertexData(Texture* texture, const PODVector<float>& vertexData);

    void SetInputDisabled(bool disabled) { inputDisabled_ = disabled; }

    void FadeOut(float time);
    void FadeIn(float time);

    float GetFadeAlpha() { return fadeAlpha_; }
    void SetFadeAlpha(float fadeAlpha) { fadeAlpha_ = fadeAlpha; }

    void Initialize();

    void Shutdown();

private:

    static WeakPtr<Context> readerContext_;
    static void TBFileReader(const char* filename, void** data, unsigned* length);

    void Render(VertexBuffer* buffer, const PODVector<UIBatch>& batches, unsigned batchStart, unsigned batchEnd);
    void SetVertexData(VertexBuffer* dest, const PODVector<float>& vertexData);

    bool inputDisabled_;

    float fadeAlpha_;
    float fadeTarget_;
    float currentFadeTime_;
    float fadeTime_;

    /// UI rendering batches.
    PODVector<UIBatch> batches_;
    /// UI rendering vertex data.
    PODVector<float> vertexData_;
    SharedPtr<VertexBuffer> vertexBuffer_;

    WeakPtr<Graphics> graphics_;

    bool shuttingDown_;



};

}

#endif
