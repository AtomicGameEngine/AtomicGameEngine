// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEWidget.h"

using namespace Atomic;
using namespace tb;

namespace tb
{
class TBLayout;
}

namespace AtomicEditor
{

class ListView;

class IssuesWidget: public AEWidget
{
    OBJECT(IssuesWidget);

public:

    IssuesWidget(Context* context);
    virtual ~IssuesWidget();

    void UpdateIssues();

    bool OnEvent(const TBWidgetEvent &ev);

private:

    void HandleEditorShutdown(StringHash eventType, VariantMap& eventData);

    SharedPtr<ListView> issueList_;
    TBLayout* issuelayout_;

};

}
