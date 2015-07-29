// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "ResourceEditor.h"
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/tb_style_edit.h>
#include <TurboBadger/tb_select.h>

using namespace Atomic;
using namespace tb;

namespace AtomicEditor
{

class JSAutocomplete;

class JSResourceEditor: public ResourceEditor, public TBStyleEditTextChangeListener
{
    OBJECT(JSResourceEditor);

public:

    JSResourceEditor(Context* context, const String& fullpath, TBTabContainer* container);

    virtual ~JSResourceEditor();

    bool OnEvent(const TBWidgetEvent &ev);

    bool FindText(const String& findText, unsigned flags);
    void FindTextClose();

    void OnChange(TBStyleEdit* styleEdit);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);    

    void GotoTokenPos(int tokenPos);
    void GotoLineNumber(int lineNumber);

    void SetFocus();

    bool HasUnsavedModifications();

private:

    void UpdateLineNumbers();

    tb::TBStyleEdit* styleEdit_;
    tb::TBSelectList* lineNumberList_;

    TBEditField* editField_;
    JSAutocomplete* autocomplete_;

    float textDelta_;
    bool textDirty_;

    bool modified_;

    int currentFindPos_;

};

}
