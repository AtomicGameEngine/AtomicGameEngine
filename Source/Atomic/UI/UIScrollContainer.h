
#pragma once

#include <TurboBadger/tb_scroll_container.h>

#include "UIWidget.h"

namespace Atomic
{

enum UI_SCROLL_MODE {
    ///< X and Y always			scroll-mode: xy
    UI_SCROLL_MODE_X_Y = tb::SCROLL_MODE_X_Y,
    ///< Y always (X never)		scroll-mode: y
    UI_SCROLL_MODE_Y = tb::SCROLL_MODE_Y,
    ///< Y auto (X never)		scroll-mode: y-auto
    UI_SCROLL_MODE_Y_AUTO = tb::SCROLL_MODE_Y_AUTO,
    ///< X auto, Y auto			scroll-mode: auto
    UI_SCROLL_MODE_X_AUTO_Y_AUTO = tb::SCROLL_MODE_X_AUTO_Y_AUTO,
    ///< X any Y never			scroll-mode: off
    UI_SCROLL_MODE_OFF = tb::SCROLL_MODE_OFF
};


class UIScrollContainer : public UIWidget
{
    OBJECT(UIScrollContainer)

public:

    UIScrollContainer(Context* context, bool createWidget = true);
    virtual ~UIScrollContainer();

    void SetScrollMode(UI_SCROLL_MODE mode);
    UI_SCROLL_MODE GetScrollMode();

    /// Set to true if the preferred size of this container should adapt to the preferred size of the content. This is disabled by default.
    void SetAdaptToContentSize(bool adapt);
    bool GetAdaptToContentSize();

    /// Set to true if the content should adapt to the available size of this container when it's larger than the preferred size.
    void SetAdaptContentSize(bool adapt);
    bool GetAdaptContentSize();


protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
