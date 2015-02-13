// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#pragma once

#include "../Scene/Component.h"

namespace Atomic
{

class TimeOfDay : public Component
{
    OBJECT(TimeOfDay);

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
