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

namespace Atomic
{

/// %Console window with log history and command line prompt.
class ATOMIC_API Console : public Object
{
    ATOMIC_OBJECT(Console, Object);

public:
    /// Construct.
    Console(Context* context);
    /// Destruct.
    ~Console();

    /// Show or hide.
    void SetVisible(bool enable);
    /// Toggle visibility.
    void Toggle();

    /// Automatically set console to visible when receiving an error log message.
    void SetAutoVisibleOnError(bool enable) { autoVisibleOnError_ = enable; }

    /// Set the command interpreter.
    void SetCommandInterpreter(const String& interpreter);

    /// Set command history maximum size, 0 disables history.
    void SetNumHistoryRows(unsigned rows);

    /// Return whether is visible.
    bool IsVisible() const;

    /// Return true when console is set to automatically visible when receiving an error log message.
    bool IsAutoVisibleOnError() const { return autoVisibleOnError_; }

    /// Return the last used command interpreter.
    const String& GetCommandInterpreter() const { return interpreters_[currentInterpreter_]; }

    /// Return history maximum size.
    unsigned GetNumHistoryRows() const { return historyRows_; }

    /// Remove all rows.
    void Clear();

private:
    /// Populate the command line interpreters that could handle the console command.
    bool PopulateInterpreter();
    ///
    void HandleScreenMode(StringHash eventType, VariantMap& eventData);
    /// Handle a log message.
    void HandleLogMessage(StringHash eventType, VariantMap& eventData);
    /// Render system ui.
    void RenderUi(StringHash eventType, VariantMap& eventData);

    /// Auto visible on error flag.
    bool autoVisibleOnError_;
    /// List of command interpreters.
    Vector<String> interpreters_;
    /// Pointers to c strings in interpreters_ list for efficient UI rendering.
    PODVector<const char*> interpretersPointers_;
    /// Last used command interpreter.
    int currentInterpreter_;
    /// Command history.
    Vector<String> history_;
    /// Command history maximum rows.
    unsigned historyRows_;
    /// Is console window open.
    bool isOpen_;
    /// Input box buffer.
    char inputBuffer_[0x1000];
    IntVector2 windowSize_;
    bool scrollToEnd_ = false;
    bool focusInput_ = false;
};

}
