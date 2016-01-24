//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include "ResourceEditor.h"

using namespace Atomic;
using namespace tb;

namespace Atomic
{
    class UIWebView;
    class WebClient;
    class WebMessageHandler;
}

namespace AtomicEditor
{

class JSResourceEditor: public ResourceEditor
{
    OBJECT(JSResourceEditor);

public:

    JSResourceEditor(Context* context, const String& fullpath, UITabContainer* container);

    virtual ~JSResourceEditor();

    bool OnEvent(const TBWidgetEvent &ev);

    bool FindText(const String& findText, unsigned flags);
    void FindTextClose();

    void GotoTokenPos(int tokenPos);
    void GotoLineNumber(int lineNumber);

    void FormatCode();

    void SetFocus();

    bool Save();

private:

    void HandleWebViewLoadEnd(StringHash eventType, VariantMap& eventData);
    void HandleWebMessage(StringHash eventType, VariantMap& eventData);

    bool BeautifyJavascript(const char* source, String& output);

    SharedPtr<UIWebView> webView_;
    WeakPtr<WebClient> webClient_;
    WeakPtr<WebMessageHandler> messageHandler_;
};

}
