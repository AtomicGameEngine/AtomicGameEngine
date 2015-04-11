#pragma once

#include <Poco/SharedMemory.h>
#include "../Core/Object.h"
#include "../IO/MemoryBuffer.h"

namespace Atomic
{

/// read/write variant map
/// reserve some space at from end for mouse/etc state
class ProcessChannel : public Object
{
    OBJECT(ProcessChannel)

public:

    enum Mode
    {
        READ = 0,
        WRITE
    };

    ProcessChannel(Context* context, Mode mode, const String& name, unsigned size, bool server = true);
    virtual ~ProcessChannel();

private:
    Poco::SharedMemory* shared_;
    MemoryBuffer* buffer_;
    unsigned size_;
    Mode mode_;
};


}
