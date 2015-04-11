
#pragma once

#include "ProcessBase.h"

namespace Atomic
{

class ProcessChannel;
class ProcessClient;

class ProcessServer : public ProcessBase
{
    OBJECT(ProcessServer)

public:

    struct ClientData
    {

    };

    ProcessServer(Context* context);
    virtual ~ProcessServer();

private:

    SharedPtr<ProcessClient> client_;

};

}
