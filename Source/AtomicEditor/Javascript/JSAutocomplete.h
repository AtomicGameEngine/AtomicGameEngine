// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Container/Str.h>
#include <Atomic/Container/HashMap.h>
#include <Atomic/Container/List.h>
#include <Atomic/Math/StringHash.h>

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/tb_style_edit.h>

using namespace Atomic;

using namespace tb;

namespace AtomicEditor
{

// per document autocompletion
class JSAutocomplete
{
public:

    JSAutocomplete(TBEditField* editField);

    ~JSAutocomplete();

    void Show();
    void Hide();
    bool Visible();

    void SetPosition(const TBPoint& pos);
    bool UpdateCompletions(const String& value);

    bool OnEvent(const TBWidgetEvent &ev);

    void UpdateLocals();

private:

    HashMap<StringHash, List<String> > locals_;

    TBEditField* editField_;
    TBStyleEdit* styleEdit_;
    TBSelectList* autoList_;
    TBGenericStringItemSource autoSource_;
    String currentValue_;

};


}
