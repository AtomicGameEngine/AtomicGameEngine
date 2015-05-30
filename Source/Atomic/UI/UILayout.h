
#pragma once

#include "UIWidget.h"

namespace Atomic
{

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
    void SetLayoutSize(/* LAYOUT_SIZE */ unsigned size);

    void SetLayoutPosition(/*LAYOUT_POSITION*/ unsigned position);
    void SetLayoutDistribution(/* LAYOUT_DISTRIBUTION */ unsigned distribution);
    void SetLayoutDistributionPosition(/*LAYOUT_DISTRIBUTION_POSITION*/ unsigned distribution_pos);


protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
