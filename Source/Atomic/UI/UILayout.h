
#pragma once

#include <ThirdParty/TurboBadger/tb_layout.h>

#include "UIWidget.h"

namespace Atomic
{

/// Specifies which height widgets in a AXIS_X layout should have,
///	or which width widgets in a AXIS_Y layout should have.
///	No matter what, it will still prioritize minimum and maximum for each widget.
enum UI_LAYOUT_SIZE
{
    /// Sizes depend on the gravity for each widget. (If the widget pulls
    /// towards both directions, it should grow to all available space)
    UI_LAYOUT_SIZE_GRAVITY = tb::LAYOUT_SIZE_GRAVITY,

    /// Size will be the preferred so each widget may be sized differently.
    UI_LAYOUT_SIZE_PREFERRED = tb::LAYOUT_SIZE_PREFERRED,

    /// Size should grow to all available space
    UI_LAYOUT_SIZE_AVAILABLE = tb::LAYOUT_SIZE_AVAILABLE
};


class UILayoutParams : public Object
{
    OBJECT(UILayoutParams)

public:

    UILayoutParams(Context* context);
    virtual ~UILayoutParams();

    void SetHeight(int height) { params_.SetHeight(height); }

    tb::LayoutParams* GetTBLayoutParams() { return &params_; }

private:

    tb::LayoutParams params_;

};


class UILayout : public UIWidget
{
    OBJECT(UILayout)

public:

    UILayout(Context* context, bool createWidget = true);
    virtual ~UILayout();

    void SetSpacing(int spacing);

    void SetAxis(/* AXIS */ unsigned axis);
    void SetLayoutSize(UI_LAYOUT_SIZE size);

    void SetLayoutPosition(/*LAYOUT_POSITION*/ unsigned position);
    void SetLayoutDistribution(/* LAYOUT_DISTRIBUTION */ unsigned distribution);
    void SetLayoutDistributionPosition(/*LAYOUT_DISTRIBUTION_POSITION*/ unsigned distribution_pos);


protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
