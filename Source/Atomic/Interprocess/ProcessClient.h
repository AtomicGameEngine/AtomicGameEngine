
#pragma once

#include "ProcessBase.h"

namespace Atomic
{

class ProcessChannel;

class ProcessClient : public ProcessBase
{
    OBJECT(ProcessClient)

public:

    ProcessClient(Context* context);
    virtual ~ProcessClient();

private:

};

}
