
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

class ATOMIC_API NETHost : public Object
{

    OBJECT(NETHost);

public:
    /// Construct.
    NETHost(Context* context);
    /// Destruct.
    virtual ~NETHost();

    virtual bool Initialize(const String& coreCLRFilesAbsPath) = 0;
    virtual bool CreateDelegate(const String& assemblyName, const String& qualifiedClassName, const String& methodName, void** funcOut) = 0;

};

}
