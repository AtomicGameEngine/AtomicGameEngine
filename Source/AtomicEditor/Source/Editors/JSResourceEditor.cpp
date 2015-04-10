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



#include "JSResourceEditor.h"

#include "../AEJavascript.h"

#include "../Javascript/JSAutocomplete.h"
#include "../Javascript/JSTheme.h"
#include "../Javascript/JSASTSyntaxColorVisitor.h"
#ifdef USE_SPIDERMONKEY
#include "../Javascript/JSSpiderMonkeyVM.h"
#endif

#include <TurboBadger/tb_message_window.h>
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/tb_style_edit.h>
#include <TurboBadger/tb_style_edit_content.h>

using namespace tb;

namespace AtomicEditor
{

JSResourceEditor ::JSResourceEditor(Context* context, const String &fullpath, TBTabContainer *container) :
    ResourceEditor(context, fullpath, container),
    styleEdit_(0),
    lineNumberList_(0),
    editField_(0),
    autocomplete_(0),
    textDirty_(true),
    textDelta_(0.0f),
    modified_(false),
    currentFindPos_(-1)
{

    TBLayout* layout = new TBLayout();
    layout->SetLayoutSize(LAYOUT_SIZE_GRAVITY);
    layout->SetGravity(WIDGET_GRAVITY_ALL);
    layout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);

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

    String json;

    JSASTProgram* program = NULL;
    //JSSpiderMonkeyVM* smjs = GetSubsystem<JSSpiderMonkeyVM>();
    AEJavascript* smjs = GetSubsystem<AEJavascript>();
    if (smjs->ParseJavascriptToJSON(source.CString(), json))
    {
        program = JSASTProgram::ParseFromJSON(fullpath, json);
    }

    text->SetText(source.CString());

    lineNumberList_ = new TBSelectList();
    lineNumberList_->SetFontDescription(fd);
    lineNumberList_->SetSkinBg(TBIDC("LineNumberSelectList"));
    lineNumberList_->GetScrollContainer()->SetScrollMode(SCROLL_MODE_OFF);
    //lineNumberList_->GetScrollContainer()->SetIgnoreScrollEvents(true);
    lineNumberList_->SetGravity(WIDGET_GRAVITY_ALL);
    LayoutParams lp;
    lp.max_w = 48;
    lineNumberList_->SetLayoutParams(lp);

    c->AddChild(text);

    layout->AddChild(lineNumberList_);
    layout->AddChild(c);
    layout->SetSpacing(0);

    TBStyleEdit* sedit = text->GetStyleEdit();
    TBTextTheme* theme = new TBTextTheme();
    for (unsigned i = 0; i < TB_MAX_TEXT_THEME_COLORS; i++)
        theme->themeColors[i] = TBColor(255, 255, 255);

    theme->themeColors[JSTHEME_LITERAL_STRING].SetFromString("#E6DB74", 7);

    theme->themeColors[JSTHEME_LITERAL_NUMBER].SetFromString("#AE81FF", 7);
    theme->themeColors[JSTHEME_LITERAL_REGEX].SetFromString("#AE81FF", 7);
    theme->themeColors[JSTHEME_LITERAL_BOOLEAN].SetFromString("#AE81FF", 7);
    theme->themeColors[JSTHEME_LITERAL_NULL].SetFromString("#AE81FF", 7);

    theme->themeColors[JSTHEME_FUNCTION].SetFromString("#66D9EF", 7);
    theme->themeColors[JSTHEME_VAR].SetFromString("#66D9EF", 7);


    theme->themeColors[JSTHEME_KEYWORD].SetFromString("#f92672", 7);
    theme->themeColors[JSTHEME_OPERATOR].SetFromString("#f92672", 7);

    theme->themeColors[JSTHEME_CODE].SetFromString("#a6e22e", 7);
    theme->themeColors[JSTHEME_COMMENT].SetFromString("#75715e", 7);

    theme->themeColors[JSTHEME_FUNCTIONDECLARG].SetFromString("#FF9800", 7);

    sedit->SetTextTheme(theme);

    sedit->text_change_listener = this;

    styleEdit_ = sedit;
    UpdateLineNumbers();

    if (program)
    {
        JSASTSyntaxColorVisitor syntaxColor(sedit);
        syntaxColor.visit(program);
    }

    autocomplete_ = new JSAutocomplete(text);
    autocomplete_->UpdateLocals();

    SubscribeToEvent(E_UPDATE, HANDLER(JSResourceEditor, HandleUpdate));

    // FIXME: Set the size at the end of setup, so all children are updated accordingly
    // future size changes will be handled automatically
    TBRect rect = container_->GetContentRoot()->GetRect();
    rootContentWidget_->SetSize(rect.w, rect.h);
}

JSResourceEditor::~JSResourceEditor()
{
}

void JSResourceEditor::UpdateLineNumbers()
{
    if (!styleEdit_)
        return;

    TBGenericStringItemSource* lineSource = lineNumberList_->GetDefaultSource();

    int lines = lineSource->GetNumItems();

    int lineCount = styleEdit_->blocks.CountLinks();

    if (lines == lineCount)
        return;

    while (lines > lineCount)
    {
        lineSource->DeleteItem(lineSource->GetNumItems() - 1);
        lines --;
    }

    for (int i = lines; i < lineCount; i++)
    {
        String sline;
        sline.AppendWithFormat("%i  ", i + 1);
        TBGenericStringItem* item = new TBGenericStringItem(sline.CString());
        lineSource->AddItem(item);

    }

    // item widgets don't exist until ValidateList
    lineNumberList_->ValidateList();

    for (int i = 0; i < lineCount; i++)
    {
        TBTextField* textField = (TBTextField* )lineNumberList_->GetItemWidget(i);

        if (textField)
        {
            textField->SetTextAlign(TB_TEXT_ALIGN_RIGHT);
            textField->SetSkinBg(TBIDC("TBSelectItemLineNumber"));
        }

    }

}

void JSResourceEditor::OnChange(TBStyleEdit* styleEdit)
{
    textDelta_ = 0.25f;
    textDirty_ = true;
    modified_ = true;

    String filename = GetFileNameAndExtension(fullpath_);
    filename += "*";
    button_->SetText(filename.CString());

    autocomplete_->Hide();

    TBTextFragment* fragment = 0;
    int ofs = styleEdit_->caret.pos.ofs;
    fragment = styleEdit_->caret.pos.block->FindFragment(ofs, true);

    if (fragment && fragment->len && (styleEdit_->caret.pos.ofs == (fragment->ofs + fragment->len)))
    {
        String value(fragment->Str(), fragment->len);
        bool hasCompletions = autocomplete_->UpdateCompletions(value);

        if (hasCompletions)
        {
            autocomplete_->SetPosition(TBPoint(fragment->xpos, (styleEdit_->caret.y - styleEdit_->scroll_y) + fragment->line_height));
            autocomplete_->Show();
        }
    }

    UpdateLineNumbers();
}

bool JSResourceEditor::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_KEY_DOWN)
    {
        if (autocomplete_ && autocomplete_->Visible())
        {
            return autocomplete_->OnEvent(ev);
        }

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
        else if (ev.ref_id == TBIDC("beautify"))
        {
            TBStr text;
            styleEdit_->GetText(text);

            if (text.Length())
            {
                AEJavascript* smjs = GetSubsystem<AEJavascript>();
                String output;
                if (smjs->BeautifyJavascript(text.CStr(), output))
                {
                    if (output.Length())
                    {
                        styleEdit_->selection.SelectAll();
                        styleEdit_->InsertText(output.CString(), output.Length());
                    }
                }
            }
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

void JSResourceEditor::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    if (!styleEdit_)
        return;

    // sync line number
    lineNumberList_->GetScrollContainer()->ScrollTo(0, styleEdit_->scroll_y);
    lineNumberList_->SetValue(styleEdit_->GetCaretLine());

    if (autocomplete_->Visible())
    {
        TBTextFragment* fragment = 0;
        int ofs = styleEdit_->caret.pos.ofs;
        fragment = styleEdit_->caret.pos.block->FindFragment(ofs, true);

        if (fragment && (styleEdit_->caret.pos.ofs == (fragment->ofs + fragment->len)))
        {
            String value(fragment->Str(), fragment->len);
            bool hasCompletions = autocomplete_->UpdateCompletions(value);

            if (!hasCompletions)
            {
                autocomplete_->Hide();
            }
        }

    }

    // Timestep parameter is same no matter what event is being listened to
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();

    if (!textDirty_)
        return;

    if (textDelta_ > 0.0f)
    {
        textDelta_ -= timeStep;
        if (textDelta_ < 0.0f)
        {
            textDelta_ = 0.0f;
        }
        else
        {
            return;
        }
    }

    TBStr text;
    styleEdit_->GetText(text);

    JSASTProgram* program = NULL;
    /*
    JSSpiderMonkeyVM* smjs = GetSubsystem<JSSpiderMonkeyVM>();
    */
    AEJavascript* smjs = GetSubsystem<AEJavascript>();
    String json;
    if (smjs->ParseJavascriptToJSON(text.CStr(), json))
    {
        program = JSASTProgram::ParseFromJSON("fullpath", json);

        if (program)
        {
            JSASTSyntaxColorVisitor syntaxColor(styleEdit_);
            syntaxColor.visit(program);
            delete program;
        }

    }

    textDirty_ = false;

    editField_->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);

}

void JSResourceEditor::FindTextClose()
{
    editField_->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);
    styleEdit_->selection.SelectNothing();
}

bool JSResourceEditor::FindText(const String& findText, unsigned flags)
{
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

void JSResourceEditor::SetFocus()
{
    editField_->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);
}

void JSResourceEditor::GotoTokenPos(int tokenPos)
{
    styleEdit_->caret.SetGlobalOfs(tokenPos);

    int height = styleEdit_->layout_height;

    int newy = styleEdit_->caret.y - height/2;

    styleEdit_->SetScrollPos(styleEdit_->scroll_x, newy);

}

void JSResourceEditor::GotoLineNumber(int lineNumber)
{
    int line = 0;
    TBBlock *block = NULL;
    for (block = styleEdit_->blocks.GetFirst(); block; block = block->GetNext())
    {
        if (lineNumber == line)
            break;
        line++;
    }
    if (!block)
        return;

    styleEdit_->caret.Place(block, 0);

    int height = styleEdit_->layout_height;
    int newy = styleEdit_->caret.y - height/2;

    styleEdit_->SetScrollPos(styleEdit_->scroll_x, newy);

}


bool JSResourceEditor::HasUnsavedModifications()
{
    return modified_;
}

}
