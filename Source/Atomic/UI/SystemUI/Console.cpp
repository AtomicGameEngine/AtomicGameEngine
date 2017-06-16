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

#include "../../Core/Context.h"
#include "../../Core/CoreEvents.h"
#include "../../Engine/EngineEvents.h"
#include "../../Graphics/Graphics.h"
#include "../../Graphics/GraphicsEvents.h"
#include "../../Input/Input.h"
#include "../../IO/IOEvents.h"
#include "../../IO/Log.h"
#include "../../Resource/ResourceCache.h"
#include "SystemUI.h"
#include "SystemUIEvents.h"
#include "Console.h"

#include "../../DebugNew.h"

namespace Atomic
{

static const int DEFAULT_HISTORY_SIZE = 512;

Console::Console(Context* context) :
    Object(context),
    autoVisibleOnError_(false),
    historyRows_(DEFAULT_HISTORY_SIZE),
    isOpen_(false),
    windowSize_(M_MAX_INT, 200)     // Width gets clamped by HandleScreenMode()
{
    inputBuffer_[0] = 0;

    SetNumHistoryRows(DEFAULT_HISTORY_SIZE);
    VariantMap dummy;
    HandleScreenMode(0, dummy);
    PopulateInterpreter();

    SubscribeToEvent(E_SCREENMODE, ATOMIC_HANDLER(Console, HandleScreenMode));
    SubscribeToEvent(E_LOGMESSAGE, ATOMIC_HANDLER(Console, HandleLogMessage));
}

Console::~Console()
{
    UnsubscribeFromAllEvents();
}

void Console::SetVisible(bool enable)
{
    isOpen_ = enable;
    if (isOpen_)
    {
        focusInput_ = true;
        SubscribeToEvent(E_SYSTEMUIFRAME, ATOMIC_HANDLER(Console, RenderUi));
    }
    else
    {
        UnsubscribeFromEvent(E_SYSTEMUIFRAME);
        ImGui::SetWindowFocus(0);
    }
}

void Console::Toggle()
{
    SetVisible(!IsVisible());
}

void Console::SetNumHistoryRows(unsigned rows)
{
    historyRows_ = rows;
    if (history_.Size() > rows)
        history_.Resize(rows);
}

bool Console::IsVisible() const
{
    return isOpen_;
}

bool Console::PopulateInterpreter()
{
    EventReceiverGroup* group = context_->GetEventReceivers(E_CONSOLECOMMAND);
    if (!group || group->receivers_.Empty())
        return false;

    String currentInterpreterName;
    if (currentInterpreter_ < interpreters_.Size())
        currentInterpreterName = interpreters_[currentInterpreter_];

    interpreters_.Clear();
    interpretersPointers_.Clear();
    for (unsigned i = 0; i < group->receivers_.Size(); ++i)
    {
        Object* receiver = group->receivers_[i];
        if (receiver)
        {
            interpreters_.Push(receiver->GetTypeName());
            interpretersPointers_.Push(interpreters_.Back().CString());
        }
    }
    Sort(interpreters_.Begin(), interpreters_.End());

    currentInterpreter_ = interpreters_.IndexOf(currentInterpreterName);
    if (currentInterpreter_ == interpreters_.Size())
        currentInterpreter_ = 0;

    return true;
}

void Console::HandleLogMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace LogMessage;

    int level = eventData[P_LEVEL].GetInt();
    String levelText;
    switch (level)
    {
    case LOG_DEBUG:
        levelText = "[Debug] ";
        break;
    case LOG_INFO:
        levelText = "[Info] ";
        break;
    case LOG_WARNING:
        levelText = "[Warning] ";
        break;
    case LOG_ERROR:
        levelText = "[Error] ";
        break;
    case LOG_NONE:
    case LOG_RAW:
    default:
        break;
    }

    // The message may be multi-line, so split to rows in that case
    Vector<String> rows = eventData[P_MESSAGE].GetString().Split('\n');

    for (unsigned i = 0; i < rows.Size(); ++i)
        history_.Push(levelText + rows[i]);
    scrollToEnd_ = true;

    if (autoVisibleOnError_ && level == LOG_ERROR && !IsVisible())
        SetVisible(true);
}

void Console::RenderUi(StringHash eventType, VariantMap& eventData)
{
    Graphics* graphics = GetSubsystem<Graphics>();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    bool wasOpen = isOpen_;
    ImVec2 size(graphics->GetWidth(), windowSize_.y_);
    ImGui::SetNextWindowSize(size);
    if (ImGui::Begin("Debug Console", &isOpen_, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|
                     ImGuiWindowFlags_NoSavedSettings))
    {
        auto region = ImGui::GetContentRegionAvail();
        ImGui::BeginChild("scrolling", ImVec2(region.x, region.y - 30), false, ImGuiWindowFlags_HorizontalScrollbar);

        for (const auto& row : history_)
            ImGui::TextUnformatted(row.CString());

        if (scrollToEnd_)
        {
            ImGui::SetScrollHere();
            scrollToEnd_ = false;
        }

        ImGui::EndChild();

        ImGui::PushItemWidth(100);
        if (ImGui::Combo("", &currentInterpreter_, &interpretersPointers_.Front(), interpretersPointers_.Size()))
        {

        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(region.x - 110);
        if (focusInput_)
        {
            ImGui::SetKeyboardFocusHere();
            focusInput_ = false;
        }
        if (ImGui::InputText("", inputBuffer_, sizeof(inputBuffer_), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            focusInput_ = true;
            String line(inputBuffer_);
            if (line.Length())
            {
                // Store to history, then clear the lineedit
                history_.Push(line);
                if (history_.Size() > historyRows_)
                    history_.Erase(history_.Begin());
                scrollToEnd_ = true;
                inputBuffer_[0] = 0;

                // Send the command as an event for script subsystem
                using namespace ConsoleCommand;

                VariantMap& newEventData = GetEventDataMap();
                newEventData[P_COMMAND] = line;
                newEventData[P_ID] = interpreters_[currentInterpreter_];
                SendEvent(E_CONSOLECOMMAND, newEventData);
            }
        }
        ImGui::PopItemWidth();
    }
    else if (wasOpen)
    {
        SetVisible(false);
        ImGui::SetWindowFocus(0);
        SendEvent(E_CONSOLECLOSED);
    }

    windowSize_.y_ = ImGui::GetWindowHeight();

    ImGui::End();
}

void Console::Clear()
{
    history_.Clear();
}

void Console::SetCommandInterpreter(const String& interpreter)
{
    auto index = interpreters_.IndexOf(interpreter);
    if (index == interpreters_.Size())
        index = 0;
    currentInterpreter_ = index;
}

void Console::HandleScreenMode(StringHash eventType, VariantMap& eventData)
{
    Graphics* graphics = GetSubsystem<Graphics>();
    windowSize_.x_ = Clamp(windowSize_.x_, 0, graphics->GetWidth());
    windowSize_.y_ = Clamp(windowSize_.y_, 0, graphics->GetHeight());
}

}
