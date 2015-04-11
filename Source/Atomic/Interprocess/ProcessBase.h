#pragma once

#include "../Core/Object.h"

namespace Atomic
{

class ProcessChannel;

class ProcessBase : public Object
{

    OBJECT(ProcessBase)

public:

    ProcessBase(Context* context);
    virtual ~ProcessBase();

protected:

    SharedPtr<ProcessChannel> inputChannel_;
    SharedPtr<ProcessChannel> outputChannel_;

};


}
