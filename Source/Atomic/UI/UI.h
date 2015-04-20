
#pragma once

#include "../Core/Object.h"
#include "../UI/UIBatch.h"

namespace tb
{
class TBWidget;
}

namespace Atomic
{

class VertexBuffer;
class UIRenderer;
class UIWidget;

class UI : public Object
{
    OBJECT(UI)

public:

    /// Construct.
    UI(Context* context);
    /// Destruct.
    virtual ~UI();

    tb::TBWidget* GetRootWidget() { return rootWidget_; }
    bool LoadResourceFile(tb::TBWidget* widget, const String& filename);

    void SetKeyboardDisabled(bool disabled) {keyboardDisabled_ = disabled; }
    void SetInputDisabled(bool disabled) { inputDisabled_ = disabled; }

    void Render();
    void GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor);
    void SubmitBatchVertexData(Texture* texture, const PODVector<float>& vertexData);

    void Initialize(const String& languageFile);

    void Shutdown();

    void LoadSkin(const String& skin, const String& overrideSkin);
    void AddFont(const String& fontFile, const String &name);
    void SetDefaultFont(const String& name, int size);

    bool IsWidgetWrapped(tb::TBWidget* widget);
    UIWidget* WrapWidget(tb::TBWidget* widget);

    void GetTBIDString(unsigned id, String& value);

private:

    static WeakPtr<Context> uiContext_;
    static void TBFileReader(const char* filename, void** data, unsigned* length);
    static void TBIDRegisterStringCallback(unsigned id, const char* value);

    void HandleRenderUpdate(StringHash eventType, VariantMap& eventData);
    void Render(VertexBuffer* buffer, const PODVector<UIBatch>& batches, unsigned batchStart, unsigned batchEnd);
    void SetVertexData(VertexBuffer* dest, const PODVector<float>& vertexData);

    tb::TBWidget* rootWidget_;
    UIRenderer* renderer_;

    /// UI rendering batches.
    PODVector<UIBatch> batches_;
    /// UI rendering vertex data.
    PODVector<float> vertexData_;
    SharedPtr<VertexBuffer> vertexBuffer_;

    WeakPtr<Graphics> graphics_;

    HashMap<tb::TBWidget*, UIWidget*> widgetWrap_;
    HashMap<unsigned, String> tbidToString_;

    bool inputDisabled_;
    bool keyboardDisabled_;
    bool initialized_;

    // Events
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


};

}
