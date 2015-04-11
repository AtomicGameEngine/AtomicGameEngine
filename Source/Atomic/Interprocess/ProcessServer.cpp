
#include "ProcessChannel.h"
#include "ProcessServer.h"
#include "ProcessClient.h"

namespace Atomic
{

ProcessServer::ProcessServer(Context* context) : ProcessBase(context)
{
    unsigned size = 1024 * 1024 * 4;
    outputChannel_ = new ProcessChannel(context, ProcessChannel::WRITE, "ATOMIC_SERVER_WRITE", size, true);
    inputChannel_ = new ProcessChannel(context, ProcessChannel::WRITE, "ATOMIC_SERVER_READ", size, true);
}

ProcessServer::~ProcessServer()
{

}

}
