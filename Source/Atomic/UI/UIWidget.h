
#pragma once

#include <ThirdParty/TurboBadger/tb_widgets.h>

#include "../Core/Object.h"

#include "UIPreferredSize.h"
#include "UIDragObject.h"

namespace Atomic
{

/// Defines widget visibility, used with UIWidget::SetVisibility.
enum UI_WIDGET_VISIBILITY
{
    /// Visible (default)
    UI_WIDGET_VISIBILITY_VISIBLE = tb:: WIDGET_VISIBILITY_VISIBLE,
    /// Invisible, but layouted. Interaction disabled.
    UI_WIDGET_VISIBILITY_INVISIBLE = tb::WIDGET_VISIBILITY_INVISIBLE,
    /// Invisible and no layout. Interaction disabled.
    UI_WIDGET_VISIBILITY_GONE = tb::WIDGET_VISIBILITY_GONE
};

/// TBWidget gravity (may be combined).
/// Gravity gives hints about positioning and sizing preferences.
enum UI_GRAVITY {

    UI_GRAVITY_NONE = tb::WIDGET_GRAVITY_NONE,
    UI_GRAVITY_LEFT = tb::WIDGET_GRAVITY_LEFT,
    UI_GRAVITY_RIGHT = tb::WIDGET_GRAVITY_RIGHT,
    UI_GRAVITY_TOP = tb::WIDGET_GRAVITY_TOP,
    UI_GRAVITY_BOTTOM = tb::WIDGET_GRAVITY_BOTTOM,

    UI_GRAVITY_LEFT_RIGHT	= tb::WIDGET_GRAVITY_LEFT_RIGHT,
    UI_GRAVITY_TOP_BOTTOM	= tb::WIDGET_GRAVITY_TOP_BOTTOM,
    UI_GRAVITY_ALL			= tb::WIDGET_GRAVITY_ALL,
    UI_GRAVITY_DEFAULT		= tb::WIDGET_GRAVITY_DEFAULT
};


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

    const String& GetId();

    UIWidget* GetParent();
    UIWidget* GetContentRoot();
    IntRect GetRect();

    UIPreferredSize* GetPreferredSize();

    void SetRect(IntRect r);    
    void SetSize(int width, int height);
    void SetPosition(int x, int y);
    void SetText(const String& text);
    void SetSkinBg(const String& id);
    void SetLayoutParams(UILayoutParams* params);
    void SetFontDescription(UIFontDescription* fd);

    void RemoveChild(UIWidget* child, bool cleanup = true);

    void DeleteAllChildren();

    // String ID    
    virtual void SetId(const String& id);

    void Center();
    void SetGravity(UI_GRAVITY gravity);

    void SetValue(double value);
    double GetValue();

    void SetFocus();

    void SetState(/*WIDGET_STATE*/ unsigned state, bool on);
    bool GetState(/*WIDGET_STATE*/ unsigned state);

    void SetVisibility(UI_WIDGET_VISIBILITY visibility);
    UI_WIDGET_VISIBILITY GetVisibility();

    void SetStateRaw(/*WIDGET_STATE*/ unsigned state);
    /*WIDGET_STATE*/ unsigned GetStateRaw();

    void Invalidate();
    void Die();

    void SetDragObject(UIDragObject* object) { dragObject_ = object; }
    UIDragObject* GetDragObject() { return dragObject_; }

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

    SharedPtr<UIPreferredSize> preferredSize_;

    SharedPtr<UIDragObject> dragObject_;

};

}
