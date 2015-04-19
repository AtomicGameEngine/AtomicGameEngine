
#pragma once

#include <TurboBadger/tb_widgets.h>
#include "../Core/Object.h"

namespace tb
{
class TBWidget;
}


namespace Atomic
{

/// Wraps a TurboBadger widget
class UIWidget : public Object, public tb::TBWidgetDelegate
{
    friend class UI;

    OBJECT(UIWidget)

public:

    UIWidget(Context* context, bool createWidget = true);
    virtual ~UIWidget();

    bool Load(const String& filename);

    void SetSize(int width, int height);
    void SetPosition(int x, int y);
    bool SetText(const String& text);

    void Center();

    UIWidget* GetWidgetByID(const String& id);

    void AddChild(UIWidget* child);

    tb::TBWidget* GetInternalWidget() { return widget_; }

protected:

    void ConvertEvent(UIWidget* target, const tb::TBWidgetEvent &ev, VariantMap& data);

    void SetWidget(tb::TBWidget* widget);

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);
    virtual void OnDelete();

    tb::TBWidget* widget_;

};

}
