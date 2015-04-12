
#include "../IO/Log.h"

#include "IPCWorker.h"
#include "IPCMessage.h"
#include "IPCUnix.h"

namespace Atomic
{

IPCWorker::IPCWorker(int fd, Context* context) : Object(context),
    fd_(fd)
{

}

IPCWorker::~IPCWorker()
{

}

void IPCWorker::ThreadFunction()
{

    PipeTransport transport;

    if (!transport.OpenClient(fd_))
    {
        LOGERRORF("Unable to open IPC transport fd = %i", fd_);
        shouldRun_ = false;
        return;
    }

    IPCMessageEvent msgEvent;

    StringHash eventType(42);
    VariantMap eventData;
    eventData[eventType] = "MyMy";

    for (unsigned i = 0; i < 380; i++)
    {
        msgEvent.DoSend(transport, eventType, eventData);
    }


}

}
