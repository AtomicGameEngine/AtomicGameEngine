// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/Container/ArrayPtr.h>
#include <Atomic/UI/UI.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Core/CoreEvents.h>

#include "../AEEvents.h"
#include "../UI/UIFindTextWidget.h"

#include "TextResourceEditor.h"

#include <TurboBadger/tb_message_window.h>
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/tb_style_edit.h>
#include <TurboBadger/tb_style_edit_content.h>

using namespace tb;

namespace AtomicEditor
{

TextResourceEditor ::TextResourceEditor(Context* context, const String &fullpath, TBTabContainer *container) :
    ResourceEditor(context, fullpath, container),
    styleEdit_(0),
    editField_(0),
    modified_(false),
    currentFindPos_(-1)
{

    TBLayout* layout =  new TBLayout();
    layout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
    layout->SetGravity(WIDGET_GRAVITY_ALL);

    rootContentWidget_->AddChild(layout);

    TBContainer* c = new TBContainer();
    c->SetGravity(WIDGET_GRAVITY_ALL);

    TBEditField* text = editField_ = new TBEditField();
    text->SetMultiline(true);
    text->SetWrapping(true);
    text->SetGravity(WIDGET_GRAVITY_ALL);
    text->SetStyling(true);
    text->SetSkinBg(TBIDC("TextCode"));

    TBFontDescription fd;
    fd.SetID(TBIDC("Monaco"));
    fd.SetSize(12);
    text->SetFontDescription(fd);

    SharedPtr<File> jsFile(GetSubsystem<ResourceCache>()->GetFile(fullpath));
    assert(jsFile);

    String source;
    jsFile->ReadText(source);

    text->SetText(source.CString());

    c->AddChild(text);

    layout->AddChild(c);
    layout->SetSpacing(0);

    TBStyleEdit* sedit = text->GetStyleEdit();
    sedit->text_change_listener = this;

    TBTextTheme* theme = new TBTextTheme();
    for (unsigned i = 0; i < TB_MAX_TEXT_THEME_COLORS; i++)
        theme->themeColors[i] = TBColor(255, 255, 255);

    sedit->SetTextTheme(theme);

    styleEdit_ = sedit;

    SubscribeToEvent(E_UPDATE, HANDLER(TextResourceEditor, HandleUpdate));

    // FIXME: Set the size at the end of setup, so all children are updated accordingly
    // future size changes will be handled automatically
    TBRect rect = container_->GetContentRoot()->GetRect();
    rootContentWidget_->SetSize(rect.w, rect.h);


}

TextResourceEditor::~TextResourceEditor()
{
}



bool TextResourceEditor::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_KEY_DOWN)
    {
        if (ev.special_key == TB_KEY_ESC)
        {
            SendEvent(E_FINDTEXTCLOSE);
        }

    }

    if (ev.type == EVENT_TYPE_SHORTCUT)
    {
        if (ev.ref_id == TBIDC("close"))
        {
            if (modified_)
            {
                TBMessageWindow *msg_win = new TBMessageWindow(container_, TBIDC("unsaved_jsmodifications_dialog"));
                TBMessageWindowSettings settings(TB_MSG_OK_CANCEL, TBID(uint32(0)));
                settings.dimmer = true;
                settings.styling = true;
                msg_win->Show("Unsaved Modifications", "There are unsaved modications.\nDo you wish to discard them and close?", &settings, 640, 360);
            }
            else
            {
                Close();
            }

        }

        if (ev.ref_id == TBIDC("save") && modified_)
        {
            TBStr text;
            styleEdit_->GetText(text);
            File file(context_, fullpath_, FILE_WRITE);
            file.Write((void*) text.CStr(), text.Length());
            file.Close();

            ResourceCache* cache = GetSubsystem<ResourceCache>();

            //SharedPtr<File> jsFile (GetSubsystem<ResourceCache>()->GetFile<File>(fullpath_));
            //cache->ReloadResource(jsFile);

            String filename = GetFileNameAndExtension(fullpath_);
            button_->SetText(filename.CString());

            modified_ = false;

            SendEvent(E_JAVASCRIPTSAVED);

            return true;
        }
        else if (ev.ref_id == TBIDC("find"))
        {
            using namespace FindTextOpen;
            SendEvent(E_FINDTEXTOPEN);
        }
        else if (ev.ref_id == TBIDC("findnext") || ev.ref_id == TBIDC("findprev"))
        {
            String text;

            FindTextWidget* finder = GetSubsystem<FindTextWidget>();
            finder->GetFindText(text);

            // TODO: get flags from finder
            unsigned flags = FINDTEXT_FLAG_NONE;

            if (ev.ref_id == TBIDC("findnext"))
                flags |= FINDTEXT_FLAG_NEXT;
            else if (ev.ref_id == TBIDC("findprev"))
                flags |= FINDTEXT_FLAG_PREV;

            flags |= FINDTEXT_FLAG_WRAP;

            finder->Find(text, flags);
        }
        else if (ev.ref_id == TBIDC("cut") || ev.ref_id == TBIDC("copy") || ev.ref_id == TBIDC("paste")
                 || ev.ref_id == TBIDC("selectall") || ev.ref_id == TBIDC("undo") || ev.ref_id == TBIDC("redo") )
        {
            editField_->OnEvent(ev);
        }

    }

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("unsaved_jsmodifications_dialog"))
        {
            if (ev.ref_id == TBIDC("TBMessageWindow.ok"))
            {
                Close();
            }
            else
            {
                SetFocus();
            }

            return true;
        }

    }

    return false;
}

void TextResourceEditor::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    if (!styleEdit_)
        return;
}

void TextResourceEditor::FindTextClose()
{
    editField_->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);
    styleEdit_->selection.SelectNothing();
}


void TextResourceEditor::OnChange(TBStyleEdit* styleEdit)
{
    modified_ = true;
    String filename = GetFileNameAndExtension(fullpath_);
    filename += "*";
    button_->SetText(filename.CString());
}

bool TextResourceEditor::FindText(const String& findText, unsigned flags)
{
    // TODO: this should be shared with the JS resource editor
    unsigned findLength = findText.Length();

    if (!findLength)
        return true;

    TBStr _source;
    styleEdit_->GetText(_source);
    String source = _source.CStr();

    unsigned pos = String::NPOS;
    int startPos = currentFindPos_;

    if (currentFindPos_ == -1)
        startPos = styleEdit_->caret.GetGlobalOfs();
    else
    {
        if (flags & FINDTEXT_FLAG_NEXT)
            startPos += findLength;
    }

    if (flags & FINDTEXT_FLAG_PREV)
    {
        String pretext = source.Substring(0, startPos);
        pos = pretext.FindLast(findText, String::NPOS, flags & FINDTEXT_FLAG_CASESENSITIVE ? true : false);
    }
    else
    {
        pos = source.Find(findText, startPos, flags & FINDTEXT_FLAG_CASESENSITIVE ? true : false);
    }

    if (pos == String::NPOS)
    {
        if (flags & FINDTEXT_FLAG_WRAP)
        {
            if (flags & FINDTEXT_FLAG_PREV)
            {
                pos = source.FindLast(findText, String::NPOS, flags & FINDTEXT_FLAG_CASESENSITIVE ? true : false);
            }
            else
            {
                pos = source.Find(findText, 0, flags & FINDTEXT_FLAG_CASESENSITIVE ? true : false);
            }
        }

        if (pos == String::NPOS)
        {
            styleEdit_->selection.SelectNothing();
            return true;
        }
    }

    currentFindPos_ = pos;

    styleEdit_->caret.SetGlobalOfs((int) pos + findLength);

    int height = styleEdit_->layout_height;

    int newy = styleEdit_->caret.y - height/2;

    styleEdit_->SetScrollPos(styleEdit_->scroll_x, newy);

    styleEdit_->selection.Select(pos, pos + findLength);

    return true;
}

void TextResourceEditor::SetFocus()
{
    editField_->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);
}


bool TextResourceEditor::HasUnsavedModifications()
{
    return modified_;
}

}
