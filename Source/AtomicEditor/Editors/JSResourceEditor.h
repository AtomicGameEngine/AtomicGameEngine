//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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

    JSResourceEditor(Context* context, const String& fullpath, UITabContainer* container);

    virtual ~JSResourceEditor();

    bool OnEvent(const TBWidgetEvent &ev);

    bool FindText(const String& findText, unsigned flags);
    void FindTextClose();

    void OnChange(TBStyleEdit* styleEdit);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    void GotoTokenPos(int tokenPos);
    void GotoLineNumber(int lineNumber);

    void FormatCode();

    void SetFocus();

    bool Save();

private:

    bool ParseJavascriptToJSON(const char* source, String& json, bool loose = true);
    bool BeautifyJavascript(const char* source, String& output);
    void UpdateLineNumbers();

    tb::TBStyleEdit* styleEdit_;
    tb::TBSelectList* lineNumberList_;

    TBEditField* editField_;
    JSAutocomplete* autocomplete_;

    float textDelta_;
    bool textDirty_;

    int currentFindPos_;

};

}
