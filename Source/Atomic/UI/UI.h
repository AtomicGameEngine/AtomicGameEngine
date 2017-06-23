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

#include <ThirdParty/TurboBadger/tb_widgets_listener.h>

#include "../Core/Object.h"
#include "../UI/UIEnums.h"
#include "../UI/UIBatch.h"

namespace Atomic
{

class VertexBuffer;
class UIRenderer;
class UIWidget;
class UIView;
class UIPopupWindow;
class MessageBox;

class ATOMIC_API UI : public Object, private tb::TBWidgetListener
{
    friend class UIView;

    ATOMIC_OBJECT(UI, Object)

public:

    /// Construct.
    UI(Context* context);
    /// Destruct.
    virtual ~UI();

    tb::TBWidget* GetRootWidget() { return rootWidget_; }
    bool LoadResourceFile(tb::TBWidget* widget, const String& filename);

    void SetKeyboardDisabled(bool disabled) {keyboardDisabled_ = disabled; }
    void SetInputDisabled(bool disabled) { inputDisabled_ = disabled; }

    void Render(bool resetRenderTargets = true);

    void Initialize(const String& languageFile);

    void Shutdown();

    void LoadSkin(const String& skin, const String& overrideSkin = String::EMPTY);
    bool GetSkinLoaded() { return skinLoaded_; }

    /// Load the default skin, will also look in resoures for UI/Skin/skin.ui.txt and
    /// UI/Skin/Override/skin.ui.txt for base skin and possible override (TODO: baked in UI setting for load from project)
    void LoadDefaultPlayerSkin();


    void AddFont(const String& fontFile, const String &name);
    void SetDefaultFont(const String& name, int size);

    bool IsWidgetWrapped(tb::TBWidget* widget);

    // wrap an existing widget we new'd from script
    void WrapWidget(UIWidget* widget, tb::TBWidget* tbwidget);

    // given a TB widget, creating a UIWidget
    UIWidget* WrapWidget(tb::TBWidget* widget);

    bool UnwrapWidget(tb::TBWidget* widget);

    unsigned DebugGetWrappedWidgetCount() { return widgetWrap_.Size(); }

    void PruneUnreachableWidgets();

    void GetTBIDString(unsigned id, String& value);

    MessageBox *ShowSystemMessageBox(const String& title, const String& message);

    // Debug HUD

    void ShowDebugHud(bool value);
    void ToggleDebugHud();

    /// Cycle debug HUD between showing primitive stats, current mode, profiler data, all three or none
    void CycleDebugHudMode();

    void SetDebugHudProfileMode(DebugHudProfileMode mode);

    void SetDebugHudExtents(bool useRootExtents = true, const IntVector2& position = IntVector2::ZERO, const IntVector2& size = IntVector2::ZERO);

    /// Set the DebugHud refresh interval for performance and metrics in seconds
    void SetDebugHudRefreshInterval(float seconds);

    void ShowConsole(bool value);
    void ToggleConsole();

    /// Get whether the console is currently visible
    bool GetConsoleIsVisible() const { return consoleVisible_; }

    bool GetFocusedWidget();

    /// request exit on next frame
    void RequestExit() { exitRequested_ = true; inputDisabled_ = true; }

    UIRenderer* GetRenderer() { return renderer_; }

    UIWidget* GetWidgetAt(int x, int y, bool include_children);

    bool GetBlockChangedEvents() const { return changedEventsBlocked_ > 0; }

    void SetBlockChangedEvents(bool blocked = true);

    UIWidget* GetHoveredWidget();

    // Debugging
    static void DebugShowSettingsWindow(UIWidget* parent);

    /// Get the currently focused view
    UIView* GetFocusedView() const { return focusedView_; }

private:

    static WeakPtr<Context> uiContext_;
    static void TBFileReader(const char* filename, void** data, unsigned* length);
    static void TBIDRegisterStringCallback(unsigned id, const char* value);

    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
    void HandleRenderUpdate(StringHash eventType, VariantMap& eventData);
    void HandleExitRequested(StringHash eventType, VariantMap& eventData);

    // TBWidgetListener
    void OnWidgetDelete(tb::TBWidget *widget);
    bool OnWidgetDying(tb::TBWidget *widget);    
    void OnWidgetFocusChanged(tb::TBWidget *widget, bool focused);
    bool OnWidgetInvokeEvent(tb::TBWidget *widget, const tb::TBWidgetEvent &ev);
    void OnWindowClose(tb::TBWindow *window);

    /// Add a UIView to UI subsystem, happens immediately at UIView creation
    void AddUIView(UIView* uiView);
    /// Set the currently focused view
    void SetFocusedView(UIView* uiView);
    /// Removes a UIView from the UI subsystem, readding a view is not encouraged
    void RemoveUIView(UIView* uiView);

    tb::TBWidget* rootWidget_;
    UIRenderer* renderer_;

    WeakPtr<Graphics> graphics_;

    HashMap<tb::TBWidget*, SharedPtr<UIWidget> > widgetWrap_;
    HashMap<unsigned, String> tbidToString_;

    WeakPtr<UIPopupWindow> tooltip_;

    int changedEventsBlocked_;

    bool inputDisabled_;
    bool keyboardDisabled_;
    bool initialized_;
    bool skinLoaded_;
    bool consoleVisible_;
    bool exitRequested_;
    
    float tooltipHoverTime_;

    Vector<SharedPtr<UIView>> uiViews_;

    WeakPtr<UIView> focusedView_;

    // Events
    void HandleScreenMode(StringHash eventType, VariantMap& eventData);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleConsoleClosed(StringHash eventType, VariantMap& eventData);
};

}
