#pragma once

#include "../Container/ArrayPtr.h"
#include "../Resource/Resource.h"

namespace Atomic
{

class JSVM;

class ATOMIC_API JSFile : public Resource
{
    OBJECT(JSFile);

public:
    /// Construct.
    JSFile(Context* context);
    /// Destruct.
    virtual ~JSFile();
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Load resource from stream. May be called from a worker thread. Return true if successful.
    virtual bool BeginLoad(Deserializer& source);
    /// Save resource. Return true if successful.
    virtual bool Save(Serializer& dest) const;

    const char* GetSource() {  return data_; }


private:
    /// File size.
    unsigned size_;
    /// File data.
    SharedArrayPtr<char> data_;
};

}
