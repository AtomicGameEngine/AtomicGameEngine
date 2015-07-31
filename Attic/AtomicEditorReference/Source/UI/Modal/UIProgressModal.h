// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <UI/AEWidget.h>

namespace tb
{
    class TBWindow;
    class TBDimmer;
    class TBTextField;
}

using namespace Atomic;
using namespace tb;

namespace AtomicEditor
{

class ProgressModal : public AEWidget
{
    OBJECT(ProgressModal);

public:
    /// Construct.
    ProgressModal(Context* context, const String& title, const String& message);
    /// Destruct.
    virtual ~ProgressModal();

    void Show();
    void Hide();
    void Center();

    void SetMessage(const String& message);

private:

    TBWindow* window_;
    TBDimmer* dimmer_;
    TBTextField* message_;

};


}
