// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#pragma once

#include <Atomic/Core/Object.h>

#include "../AETypes.h"

using namespace Atomic;

namespace Atomic
{

class JSVM;

}

namespace AtomicEditor
{

struct AEPlayerError
{
    String name_;
    String message_;
    String filename_;
    int lineNumber_;
    String stack_;
};

class AEPlayer : public Object
{
    OBJECT(AEPlayer);

public:
    /// Construct.
    AEPlayer(Context* context);
    /// Destruct.
    ~AEPlayer();

    bool Play(AEPlayerMode mode, const IntRect& rect = IntRect());

    AEPlayerMode GetMode() const { return mode_; }

    void Invalidate();

    bool HasErrors() { return errors_.Size() != 0; }
    const Vector<AEPlayerError>& GetErrors() { return errors_; }

private:

    AEPlayerMode mode_;

    Vector<AEPlayerError> errors_;

    void HandleJSError(StringHash eventType, VariantMap& eventData);

    SharedPtr<JSVM> vm_;

};


}
