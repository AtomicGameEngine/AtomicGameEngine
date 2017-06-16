//
// Copyright (c) 2017 the Atomic project.
// Copyright (c) 2008-2015 the Urho3D project.
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

#include "../../Core/Object.h"
#include "../../UI/SystemUI/SystemUI.h"

namespace Atomic
{

/// Message box dialog.
class ATOMIC_API MessageBox : public Object
{
    ATOMIC_OBJECT(MessageBox, Object);

public:
    /// Construct. If layout file is not given, use the default message box layout. If style file is not given, use the default style file from root UI element.
    MessageBox(Context* context, const String& messageString = String::EMPTY, const String& titleString = String::EMPTY);
    /// Destruct.
    virtual ~MessageBox();
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Set title text. No-ops if there is no title text element.
    void SetTitle(const String& text);
    /// Set message text. No-ops if there is no message text element.
    void SetMessage(const String& text);

    /// Return title text. Return empty string if there is no title text element.
    const String& GetTitle() const;
    /// Return message text. Return empty string if there is no message text element.
    const String& GetMessage() const;
    /// Returns true if message box is open.
    bool IsOpen() const { return isOpen_; }

private:
    /// Render message box ui.
    void RenderFrame(StringHash eventType, VariantMap& eventData);

    /// Title text element.
    String titleText_;
    /// Message text element.
    String messageText_;
    /// Is message box window open.
    bool isOpen_;
    /// Initial message box window position.
    ImVec2 windowPosition_;
    /// Initial message box window size.
    ImVec2 windowSize_;
};

}
