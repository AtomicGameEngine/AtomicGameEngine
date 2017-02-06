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

#pragma once

#include <Atomic/Core/Object.h>

#include "JSBindTypes.h"

using namespace Atomic;

namespace ToolCore
{

    class JSBPackage;
    class JSBModule;
    class JSBHeader;


    class JSBEvent : public Object
    {

        ATOMIC_OBJECT(JSBEvent, Object)

    public:

        struct EventParam
        {
            String paramID_;
            String paramName_;
            String typeInfo_;
            String enumTypeName_;
            String comment_;
        };

        JSBEvent(Context* context, JSBModule* module, const String& eventID, const String& eventName, const String& eventComment);
        virtual ~JSBEvent();

        const String& GetEventID() const { return eventID_; }
        const String& GetEventName() const { return eventName_; }
        const String& GetEventComment() const { return eventComment_; }
        unsigned GetEventHash() const;
        /// Generally this is the EventName + "Event"
        String GetScriptEventName(BindingLanguage language = BINDINGLANGUAGE_ANY) const;
        const Vector<EventParam>& GetParameters() const { return parameters_;  }

        void SetHeader(JSBHeader* header) { header_ = header; }
        JSBHeader* GetHeader() { return header_; }

        JSBPackage* GetPackage();       

        static bool ScanModuleEvents(JSBModule* module);


    private:

        SharedPtr<JSBModule> module_;
        JSBHeader* header_;

        String eventName_;
        String eventID_;
        String eventComment_;

        Vector<EventParam> parameters_;

    };


}
