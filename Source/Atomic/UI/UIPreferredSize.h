
#pragma once

#include <ThirdParty/TurboBadger/tb_widgets.h>
#include <Atomic/Container/RefCounted.h>

namespace tb
{
    class PreferredSize;
}

namespace Atomic
{

/// Defines how the size in one axis depend on the other axis when a widgets size is affected by constraints.
enum UI_SIZE_DEP {
    /// No dependency (Faster layout).
    UI_SIZE_DEP_NONE						= tb::SIZE_DEP_NONE,
    /// The width is dependant on the height. Additional layout pass may be required.
    UI_SIZE_DEP_WIDTH_DEPEND_ON_HEIGHT		= tb::SIZE_DEP_WIDTH_DEPEND_ON_HEIGHT,
    /// The height is dependant on the width. Additional layout pass may be required.
    UI_SIZE_DEP_HEIGHT_DEPEND_ON_WIDTH		= tb::SIZE_DEP_HEIGHT_DEPEND_ON_WIDTH,
    /// Both width and height are dependant on each other. Additional layout pass may be required.
    UI_SIZE_DEP_BOTH						=	tb::SIZE_DEP_BOTH
};

class UIPreferredSize : public RefCounted
{
    friend class UIWidget;

    REFCOUNTED(UIPreferredSize)

public:

    UIPreferredSize(int w = 0, int h = 0);
    virtual ~UIPreferredSize();

    int GetMinWidth() const;
    int GetMinHeight() const;

    int GetMaxWidth() const;
    int GetMaxHeight() const;

    int GetPrefWidth() const;
    int GetPrefHeight() const;

    UI_SIZE_DEP GetSizeDep() const;

    void SetMinWidth(int w);
    void SetMinHeight(int h);

    void SetMaxWidth(int w);
    void SetMaxHeight(int h);

    void SetPrefWidth(int w);
    void SetPrefHeight(int h);

    void SetSizeDep(UI_SIZE_DEP dep);

private:

    /// tb sync
    void SetFromTBPreferredSize(const tb::PreferredSize& sz) { preferredSize_ = sz; }
    const tb::PreferredSize& GetTBPreferredSize() { return preferredSize_; }

    tb::PreferredSize preferredSize_;

};

}
