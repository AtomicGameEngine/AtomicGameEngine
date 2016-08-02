//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include "../Scene/Component.h"

namespace Atomic
{

class TimeOfDay : public Component
{
    ATOMIC_OBJECT(TimeOfDay, Component);

    /// Construct.
    TimeOfDay(Context* context);

    /// Destruct.
    virtual ~TimeOfDay();

    /// Register object factory.
    static void RegisterObject(Context* context);

    float GetTimeOn() const { return timeOn_; }
    float GetTimeOff() const { return timeOff_; }

    void SetTimeOn(float timeOn) { timeOn_ = timeOn; }
    void SetTimeOff(float timeOff) { timeOff_ = timeOff; }

protected:

    void OnNodeSet(Node* node);
    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);

private:

    float timeOn_;
    float timeOff_;
    bool on_;

};


}
