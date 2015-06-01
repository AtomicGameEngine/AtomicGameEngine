
#pragma once

#include <TurboBadger/tb_widgets.h>
#include "../Core/Object.h"

namespace tb
{
class TBWidget;
}


namespace Atomic
{

class UILayoutParams;
class UIFontDescription;

/// Wraps a TurboBadger widget in our Object model
class UIWidget : public Object, public tb::TBWidgetDelegate
{
    friend class UI;

    OBJECT(UIWidget)

public:

    UIWidget(Context* context, bool createWidget = true);
    virtual ~UIWidget();

    bool Load(const String& filename);

    IntRect GetRect();
    void SetRect(IntRect r);

    void SetSize(int width, int height);
    void SetPosition(int x, int y);
    void SetText(const String& text);

    void SetSkinBg(const String& id);
    void SetLayoutParams(UILayoutParams* params);
    void SetFontDescription(UIFontDescription* fd);

    UIWidget* GetParent();
    UIWidget* GetContentRoot();

    void RemoveChild(UIWidget* child, bool cleanup = true);

    void DeleteAllChildren();

    // String ID
    const String& GetId();
    virtual void SetId(const String& id);

    void Center();
    void SetGravity(/*WIDGET_GRAVITY*/ unsigned gravity);

    void SetValue(double value);
    double GetValue();

    void SetFocus();

    void SetState(/*WIDGET_STATE*/ unsigned state, bool on);
    bool GetState(/*WIDGET_STATE*/ unsigned state);

    void SetVisibility(/*WIDGET_VISIBILITY*/ unsigned visibility);

    void SetStateRaw(/*WIDGET_STATE*/ unsigned state);
    /*WIDGET_STATE*/ unsigned GetStateRaw();

    void Invalidate();
    void Die();

    UIWidget* GetFirstChild();
    UIWidget* GetNext();

    void SetIsFocusable(bool value);


    // get this or child widget with id
    UIWidget* GetWidget(const String& id);

    virtual void AddChild(UIWidget* child);

    tb::TBWidget* GetInternalWidget() { return widget_; }

protected:

    void ConvertEvent(UIWidget* handler, UIWidget* target, const tb::TBWidgetEvent &ev, VariantMap& data);

    void SetWidget(tb::TBWidget* widget);

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);
    virtual void OnDelete();

    String id_;

    tb::TBWidget* widget_;

};

}
