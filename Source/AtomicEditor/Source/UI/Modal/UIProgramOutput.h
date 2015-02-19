// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "UIModalOps.h"

namespace AtomicEditor
{

namespace tb
{

class TBEditField;

}

class UIProgramOutput: public UIModalOpWindow
{
    OBJECT(UIProgramOutput);

public:

    UIProgramOutput(Context* context);
    virtual ~UIProgramOutput();
    bool OnEvent(const TBWidgetEvent &ev);

    void SetSubprocess(Object*subprocess);

    void OutputText(const String& text);

private:

    void HandleEvent(StringHash eventType, VariantMap& eventData);

    TBEditField* output_;

};

}
