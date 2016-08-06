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

    ATOMIC_REFCOUNTED(UIPreferredSize)

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
