
//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Atomic/IO/Log.h>

#include "JSBind.h"
#include "JSBHeader.h"
#include "JSBModule.h"
#include "JSBPackage.h"
#include "JSBEvent.h"

using namespace Atomic;

namespace ToolCore
{

JSBEvent::JSBEvent(Context* context, JSBModule* module, const String& eventID, const String& eventName, const String& eventComment) :
    Object(context),
    module_(module), 
    header_(0),
    eventID_(eventID),
    eventName_(eventName),
    eventComment_(eventComment)
{

}

JSBEvent::~JSBEvent()
{

}

JSBPackage* JSBEvent::GetPackage()
{
    return module_->GetPackage();
}

String JSBEvent::GetScriptEventName(BindingLanguage language) const
{

    if (language == BINDINGLANGUAGE_JAVASCRIPT) {

        if (eventName_ == "WidgetEvent")
        {
            return "UIWidgetEvent";
        }

        if (eventName_ == "WidgetDeleted")
        {
            return "UIWidgetDeletedEvent";
        }
    }

    if (eventName_.EndsWith("Event"))
        return eventName_;

    return eventName_ + "Event";

}

unsigned JSBEvent::GetEventHash() const
{
    return StringHash(eventName_).Value();
}

bool JSBEvent::ScanModuleEvents(JSBModule* module)
{
    const Vector<SharedPtr<JSBHeader>>& headers = module->GetHeaders();

    for (unsigned i = 0; i < headers.Size(); i++)
    {
        JSBHeader* header = headers.At(i);

        String source = header->GetSource();

        if (!source.Contains("ATOMIC_EVENT"))
            continue;

        StringVector lines = source.Split('\n');

        SharedPtr<JSBEvent> curEvent;
        String eventID;
        String eventName;
        String eventComment;

        for (unsigned j = 0; j < lines.Size(); j++)
        {
            String line = lines[j].Trimmed();
            line.Replace("\r", "");

            // Note: processes ATOMIC_EVENT decl in commented blocks
            if (line.StartsWith("ATOMIC_EVENT"))
            {
                // First check to see if there is a line comment above this and try to capture it
                eventComment = NULL;

                if (j > 0) {
                    String prevLine = lines[j-1].Trimmed();
                    if (prevLine.StartsWith(("//")))
                    {
                        prevLine.Replace("//", "");
                        eventComment = prevLine.Trimmed();
                    }
                }

                StringVector parts = line.Split('(');

                if (parts.Size() != 2)
                {
                    ATOMIC_LOGWARNINGF("Unable to parse native event splitting '(' : %s : %s", header->GetFilePath().CString(), line.CString());
                    continue;
                }

                parts = parts[1].Split(',');

                if (parts.Size() != 2)
                {
                    ATOMIC_LOGWARNINGF("Unable to parse native even splitting ',' : %s : %s", header->GetFilePath().CString(), line.CString());
                    continue;
                }

                eventID = parts[0].Trimmed();
                eventName = parts[1].Trimmed();
                eventName.Replace(")", "");

                curEvent = new JSBEvent(module->GetContext(), module, eventID, eventName, eventComment);
            }            

            if (line.Contains("}") && curEvent.NotNull())
            {
                module->RegisterEvent(curEvent);
                curEvent = 0;
            }

            // ATOMIC_PARAM(P_TYPE, Type);                 // enum UI_EVENT_TYPE

            if (line.StartsWith("ATOMIC_PARAM"))
            {
                if (curEvent.Null())
                {
                    ATOMIC_LOGWARNINGF("WARNING: Event param outside of current event: %s", line.CString());
                    continue;
                }

                line.Replace("ATOMIC_PARAM", "");
                line.Replace("(", "");
                line.Replace(")", "");

                StringVector parts = line.Split(';');

                if (parts.Size() != 2)
                {
                    ATOMIC_LOGWARNINGF("Unable to parse native event param splitting ';' : %s : %s", header->GetFilePath().CString(), line.CString());
                    continue;
                }

                String typeInfo;
                String enumTypeName;
                String formalComment;

                // comment contains type
                if (parts[1].Contains("//"))
                {
                    String comment = parts[1];
                    comment.Replace("//", "");
                    comment = comment.Trimmed();
                    StringVector typeVector = comment.Split(' ');
                    int commentStart = 1;
                    if (typeVector.Size() > 0)
                    {
                        typeInfo = typeVector[0];

                        if (typeInfo == "enum" && typeVector.Size() > 1)
                        {
                            if (JSBPackage::GetEnumAllPackages(typeVector[1]))
                            {
                                enumTypeName = typeVector[1];
                            }

                            commentStart = 1;
                        }

                        // Check to see if there are any comments following and try to capture them
                        if (typeVector.Size() > commentStart + 1)
                        {
                            formalComment = "";
                            for (int i = commentStart; i < typeVector.Size(); i++) {
                                formalComment.Append(" ");
                                formalComment.Append(typeVector[i]);
                            }
                        }
                    }
                }

                if (!typeInfo.Length())
                {
                    ATOMIC_LOGWARNINGF("Could not get type info for event param : %s : (%s) file: %s", 
                        curEvent->GetEventID().CString(), line.CString(), header->GetFilePath().CString());
                    continue;
                }

                // ID and Name parse
                parts = parts[0].Split(',');

                if (parts.Size() != 2)
                {
                    ATOMIC_LOGWARNINGF("Unable to parse native event param splitting ',' : %s : %s", header->GetFilePath().CString(), line.CString());
                    continue;
                }

                typeInfo.Replace("*", "");

                EventParam param;
                param.paramID_ = parts[0].Trimmed();
                param.paramName_ = parts[1].Trimmed();
                param.typeInfo_ = typeInfo;
                param.enumTypeName_ = enumTypeName;
                param.comment_ = formalComment;

                curEvent->parameters_.Push(param);

            }

        }
        
    }

    return true;
}

}
