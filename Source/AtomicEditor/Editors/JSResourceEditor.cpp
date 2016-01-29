//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Container/ArrayPtr.h>
#include <Atomic/UI/UI.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/JSONFile.h>

#include <Atomic/Core/CoreEvents.h>
#include <AtomicJS/Javascript/JSVM.h>

#include <ToolCore/ToolEnvironment.h>

#include <AtomicWebView/WebViewEvents.h>
#include <AtomicWebView/UIWebView.h>
#include <AtomicWebView/WebClient.h>
#include <AtomicWebView/WebMessageHandler.h>
#include <AtomicWebView/WebTexture2D.h>

#include "JSResourceEditor.h"


using namespace tb;
using namespace ToolCore;

namespace AtomicEditor
{

JSResourceEditor ::JSResourceEditor(Context* context, const String &fullpath, UITabContainer *container) :
    ResourceEditor(context, fullpath, container)
{

    TBLayout* layout = new TBLayout();
    layout->SetLayoutSize(LAYOUT_SIZE_GRAVITY);
    layout->SetGravity(WIDGET_GRAVITY_ALL);
    layout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);

    rootContentWidget_->GetInternalWidget()->AddChild(layout);

    TBContainer* c = new TBContainer();
    c->SetGravity(WIDGET_GRAVITY_ALL);

    layout->AddChild(c);

    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    String codeEditorDir = tenv->GetToolDataDir();
    codeEditorDir += "CodeEditor/Editor.html";

#ifdef ATOMIC_PLATFORM_OSX
    String url = "file://" + codeEditorDir;
#else
    String url = "file:///" + codeEditorDir;
#endif

    webView_ = new UIWebView(context_, url);
    webClient_ = webView_->GetWebClient();
    messageHandler_ = new WebMessageHandler(context_);
    webClient_->AddMessageHandler(messageHandler_);

    webView_->GetWebTexture2D()->SetClearColor(Color(.23f, .23f, .23f, 1));

    SubscribeToEvent(webClient_, E_WEBVIEWLOADEND, HANDLER(JSResourceEditor, HandleWebViewLoadEnd));
    SubscribeToEvent(messageHandler_, E_WEBMESSAGE, HANDLER(JSResourceEditor, HandleWebMessage));


    c->AddChild(webView_->GetInternalWidget());

}

JSResourceEditor::~JSResourceEditor()
{

}

void JSResourceEditor::HandleWebViewLoadEnd(StringHash eventType, VariantMap& eventData)
{    
    webClient_->ExecuteJavaScript(ToString("loadCode(\"atomic://resources/%s\");", fullpath_.CString()));
}

void JSResourceEditor::HandleWebMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace WebMessage;

    const String& request = eventData[P_REQUEST].GetString();
    WebMessageHandler* handler = static_cast<WebMessageHandler*>(eventData[P_HANDLER].GetPtr());

    if (request == "change")
    {
        SetModified(true);
    }
    else
    {
        JSONValue jvalue;
        if (JSONFile::ParseJSON(request, jvalue, false))
        {
            String message = jvalue["message"].GetString();
            if (message == "saveCode")
            {
                String code = jvalue["payload"].GetString();
                File file(context_, fullpath_, FILE_WRITE);
                file.Write((void*) code.CString(), code.Length());
                file.Close();
            }
        }
    }

    handler->Success();

}

void JSResourceEditor::FormatCode()
{
    //webClient_->ExecuteJavaScript("beautifyCode();");
}

bool JSResourceEditor::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_SHORTCUT)
    {
        if (ev.ref_id == TBIDC("close"))
        {
            RequestClose();
        }
    }

    return false;
}

void JSResourceEditor::FindTextClose()
{
}

bool JSResourceEditor::FindText(const String& findText, unsigned flags)
{

    return true;
}

void JSResourceEditor::SetFocus()
{
    //editField_->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);
}

void JSResourceEditor::GotoTokenPos(int tokenPos)
{

}

void JSResourceEditor::GotoLineNumber(int lineNumber)
{

}

bool JSResourceEditor::Save()
{
    if (!modified_)
        return true;

    webClient_->ExecuteJavaScript("saveCode();");

    SetModified(false);

    return true;

}


}
