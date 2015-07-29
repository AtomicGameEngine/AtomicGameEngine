
#include "UIPreferredSize.h"

namespace Atomic
{

UIPreferredSize::UIPreferredSize(int w, int h) :
    preferredSize_(w, h)
{
    preferredSize_.max_w = w == 0 ? 10000 : w;
    preferredSize_.max_h = h == 0 ? 10000 : h;
}

UIPreferredSize::~UIPreferredSize()
{

}

int UIPreferredSize::GetMinWidth() const
{
    return preferredSize_.min_w;

}

int UIPreferredSize::GetMinHeight() const
{
    return preferredSize_.min_h;
}

int UIPreferredSize::GetMaxWidth() const
{
    return preferredSize_.max_w;
}

int UIPreferredSize::GetMaxHeight() const
{
    return preferredSize_.max_h;
}

int UIPreferredSize::GetPrefWidth() const
{
    return preferredSize_.pref_w;
}

int UIPreferredSize::GetPrefHeight() const
{
    return preferredSize_.pref_h;
}

UI_SIZE_DEP UIPreferredSize::GetSizeDep() const
{
    return (UI_SIZE_DEP) preferredSize_.size_dependency;
}

void UIPreferredSize::SetMinWidth(int w)
{
    preferredSize_.min_w = w;

}

void UIPreferredSize::SetMinHeight(int h)
{
    preferredSize_.min_h = h;
}

void UIPreferredSize::SetMaxWidth(int w)
{
    preferredSize_.max_w = w;
}

void UIPreferredSize::SetMaxHeight(int h)
{
    preferredSize_.max_h = h;
}

void UIPreferredSize::SetPrefWidth(int w)
{
    preferredSize_.pref_w = w;
}

void UIPreferredSize::SetPrefHeight(int h)
{
    preferredSize_.pref_h = h;
}

void UIPreferredSize::SetSizeDep(UI_SIZE_DEP dep)
{
    preferredSize_.size_dependency = (tb::SIZE_DEP) dep;
}

}
