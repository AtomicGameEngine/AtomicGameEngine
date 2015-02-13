// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace AtomicEditor
{

class MessageModal : public Object
{
    OBJECT(MessageModal);

    struct ErrorWindowInfo
    {
        String title_;
        String message_;
        String id_;
    };

public:
    /// Construct.
    MessageModal(Context* context);
    /// Destruct.
    virtual ~MessageModal();

    void ShowErrorWindow(const String& title, const String& message);
    void ShowInfoWindow(const String& title, const String& message);

private:

    void HandleEditorModal(StringHash eventType, VariantMap& eventData);
};


}
