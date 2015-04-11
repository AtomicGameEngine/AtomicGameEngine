
#include "ProcessChannel.h"

namespace Atomic
{

ProcessChannel::ProcessChannel(Context *context, Mode mode, const String& name, unsigned size, bool server) :
    Object(context),
    size_(size),
    mode_(mode)
{
    shared_ = new Poco::SharedMemory(name.CString(), size, mode == WRITE ? Poco::SharedMemory::AM_WRITE : Poco::SharedMemory::AM_READ, 0, server);
    buffer_ = new MemoryBuffer(shared_->begin(), size);
}

ProcessChannel::~ProcessChannel()
{
    delete buffer_;
    delete shared_;
}

}
