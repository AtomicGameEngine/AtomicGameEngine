
#pragma once

#include <ThirdParty/TurboBadger/tb_layout.h>

#include "UIWidget.h"

namespace Atomic
{

enum UI_AXIS {
    ///< Horizontal layout
    UI_AXIS_X = tb::AXIS_X,
    ///< Vertical layout
    UI_AXIS_Y = tb::AXIS_Y,
};

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

/// Specifies which width widgets in a AXIS_X layout should have,
///    or which height widgets in a AXIS_Y layout should have. */
///
enum UI_LAYOUT_DISTRIBUTION
{
    ///< Size will be the preferred so each widget may be sized differently.
    UI_LAYOUT_DISTRIBUTION_PREFERRED = tb::LAYOUT_DISTRIBUTION_PREFERRED,
    ///< Size should grow to all available space
    UI_LAYOUT_DISTRIBUTION_AVAILABLE = tb::LAYOUT_DISTRIBUTION_AVAILABLE,
    ///< Sizes depend on the gravity for each widget. (If the widget pulls
    /// ///< towards both directions, it should grow to all available space)
    UI_LAYOUT_DISTRIBUTION_GRAVITY = tb::LAYOUT_DISTRIBUTION_GRAVITY
};

/// Specifies which y position widgets in a AXIS_X layout should have,
///	or which x position widgets in a AXIS_Y layout should have. */
enum UI_LAYOUT_POSITION
{
    ///< Position is centered
    UI_LAYOUT_POSITION_CENTER = tb::LAYOUT_POSITION_CENTER,
    ///< Position is to the left for AXIS_Y layout and top for AXIS_X layout.
    UI_LAYOUT_POSITION_LEFT_TOP = tb::LAYOUT_POSITION_LEFT_TOP,
    ///< Position is to the right for AXIS_Y layout and bottom for AXIS_X layout.
    UI_LAYOUT_POSITION_RIGHT_BOTTOM = tb::LAYOUT_POSITION_RIGHT_BOTTOM,
    ///< Position depend on the gravity for each widget. (If the widget pulls
    /// ///< towards both directions, it will be centered)
    UI_LAYOUT_POSITION_GRAVITY= tb::LAYOUT_POSITION_GRAVITY
};

/** Specifies how widgets should be moved horizontally in a AXIS_X
    layout (or vertically in a AXIS_Y layout) if there is extra space
    available. */
enum UI_LAYOUT_DISTRIBUTION_POSITION
{
    UI_LAYOUT_DISTRIBUTION_POSITION_CENTER = tb::LAYOUT_DISTRIBUTION_POSITION_CENTER,
    UI_LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP = tb::LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP,
    UI_LAYOUT_DISTRIBUTION_POSITION_RIGHT_BOTTOM = tb::LAYOUT_DISTRIBUTION_POSITION_RIGHT_BOTTOM
};



class UILayoutParams : public Object
{
    OBJECT(UILayoutParams)

public:

    UILayoutParams(Context* context);
    virtual ~UILayoutParams();

    void SetWidth(int width) { params_.SetWidth(width); }
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

    void SetAxis(UI_AXIS axis);
    void SetLayoutSize(UI_LAYOUT_SIZE size);

    void SetLayoutPosition(UI_LAYOUT_POSITION position);
    void SetLayoutDistribution(UI_LAYOUT_DISTRIBUTION distribution);
    void SetLayoutDistributionPosition(UI_LAYOUT_DISTRIBUTION_POSITION distribution_pos);


protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
