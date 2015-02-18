// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <UI/AEWidget.h>

#include <TurboBadger/tb_widgets_listener.h>

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

class UIBuildComplete : public AEWidget, private TBWidgetListener
{
    OBJECT(UIBuildComplete);

public:
    /// Construct.
    UIBuildComplete(Context* context, const String& title, const String& message, const String& buildFolder, bool success);
    /// Destruct.
    virtual ~UIBuildComplete();

    void Show();
    void Hide();
    void Center();

    bool OnEvent(const TBWidgetEvent &ev);

    void SetMessage(const String& message);    

    // TBWidgetListener
    void OnWidgetDelete(TBWidget *widget);
    bool OnWidgetDying(TBWidget *widget);

private:

    String buildFolder_;
    bool success_;

    TBWindow* window_;
    TBDimmer* dimmer_;
    TBTextField* message_;

};


}
