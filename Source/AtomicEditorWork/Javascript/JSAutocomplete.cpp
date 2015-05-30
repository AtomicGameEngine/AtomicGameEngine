// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/IO/Log.h>
#include "JSAutocomplete.h"


namespace AtomicEditor
{

JSAutocomplete::JSAutocomplete(TBEditField* editField) :
    editField_(editField),
    styleEdit_(editField->GetStyleEdit()),
    autoList_(0)
{
    autoList_ = new TBSelectList();
    autoList_->SetSource(&autoSource_);

    autoList_->SetSize(150, 100);
    autoList_->SetIgnoreInput(true);

    editField->AddChild(autoList_);

    Hide();
}

bool JSAutocomplete::UpdateCompletions(const String& value)
{
    if (value == currentValue_)
        return autoSource_.GetNumItems() > 0;

    currentValue_ = value;

    String lstring(value[0]);
    if (!locals_.Contains(lstring))
    {
        return false;
    }

    autoSource_.DeleteAllItems();

    const List<String>& tokens = locals_[lstring];

    for (List<String>::ConstIterator i = tokens.Begin(); i != tokens.End(); ++i)
    {
        if (*i != value && i->StartsWith(value))
        {
            autoSource_.AddItem(new TBGenericStringItem(i->CString()));
        }

    }

    autoList_->SetValue(0);

    return autoSource_.GetNumItems() > 0;

}

void JSAutocomplete::UpdateLocals()
{
    TBBlock *block = styleEdit_->blocks.GetFirst();
    while (block)
    {
        for (TBTextFragment* frag = block->fragments.GetFirst(); frag; frag = frag->GetNext())
        {
            if (frag->len > 3)
            {
                String lstring(frag->Str()[0]);

                String token(frag->Str(), frag->len);

                if (!locals_[lstring].Contains(token))
                {
                    //LOGINFOF("Adding Token %s to locals", token.CString());
                    locals_[lstring].Push(token);
                }

            }

        }

        block = block->GetNext();
    }
}

void JSAutocomplete::SetPosition(const TBPoint &pos)
{
    autoList_->SetPosition(pos);
}

bool JSAutocomplete::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_KEY_DOWN)
    {
        if (ev.special_key == TB_KEY_UP)
        {
            int v = autoList_->GetValue() - 1;
            if (v < 0)
            {
                styleEdit_->autocomplete_visible = false;
                autoList_->SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);
                autoList_->SetValue(0);
                return styleEdit_->KeyDown(ev.key, ev.special_key, ev.modifierkeys);

            }
            else
            {
                autoList_->SetValue(v);
            }
        }
        else if (ev.special_key == TB_KEY_DOWN)
        {
            int v = autoList_->GetValue() + 1;
            if (v >= autoList_->GetSource()->GetNumItems())
            {
                styleEdit_->autocomplete_visible = false;
                autoList_->SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);
                autoList_->SetValue(0);
                return styleEdit_->KeyDown(ev.key, ev.special_key, ev.modifierkeys);
            }
            else
            {
                autoList_->SetValue(v);
            }
        }
        else if (ev.special_key == TB_KEY_ENTER || ev.special_key == TB_KEY_TAB)
        {
            TBStr str = autoList_->GetSource()->GetItemString(autoList_->GetValue());

            TBTextFragment* fragment = 0;
            int ofs = styleEdit_->caret.pos.ofs;
            if (ofs >= 0)
            {
                fragment = styleEdit_->caret.pos.block->FindFragment(ofs, true);
                if (fragment)
                {
                    int gofs = fragment->GetGlobalOfs();
                    styleEdit_->selection.Select(gofs, gofs + fragment->len);
                    styleEdit_->InsertText(str.CStr(), str.Length());
                }
            }

            styleEdit_->autocomplete_visible = false;
            autoList_->SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);
            autoList_->SetValue(0);

            return true;
        }
        else if (ev.special_key == TB_KEY_ESC)
        {
            styleEdit_->autocomplete_visible = false;
            autoList_->SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);
            autoList_->SetValue(0);
            return true;
        }

    }

    return false;
}

JSAutocomplete::~JSAutocomplete()
{
    autoList_->SetSource(NULL);
    autoSource_.DeleteAllItems();
}

void JSAutocomplete::Show()
{
    styleEdit_->autocomplete_visible = true;
    autoList_->SetVisibilility(WIDGET_VISIBILITY_VISIBLE);
    autoList_->SetValue(0);
}

void JSAutocomplete::Hide()
{
    styleEdit_->autocomplete_visible = false;
    autoList_->SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);
    autoList_->SetValue(0);
}

bool JSAutocomplete::Visible()
{
    return autoList_->GetVisibility() == WIDGET_VISIBILITY_VISIBLE;
}


}
