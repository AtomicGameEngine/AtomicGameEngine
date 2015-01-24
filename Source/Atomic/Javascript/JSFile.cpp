#include "Precompiled.h"
#include "../Core/Context.h"
#include "../IO/Deserializer.h"
#include "../IO/FileSystem.h"
#include "../IO/Log.h"
#include "../Javascript/JSFile.h"
#include "../Core/ProcessUtils.h"
#include "../IO/Serializer.h"

#include <Duktape/duktape.h>

#include "DebugNew.h"

namespace Atomic
{

JSFile::JSFile(Context* context) :
    Resource(context),
    size_(0)
{

}

JSFile::~JSFile()
{

}

void JSFile::RegisterObject(Context* context)
{
    context->RegisterFactory<JSFile>();
}

bool JSFile::BeginLoad(Deserializer& source)
{
    size_ = source.GetSize();

    if (size_ == 0)
    {
        data_ = new char[1];
        size_ = 1;
        data_[0] = '\0';
        return true;
    }

    // Read all data ensuring '\0' terminated
    data_ = new char[size_ + 1];
    if (source.Read(data_, size_) != size_)
        return false;

    data_[size_] = '\0';

    SetMemoryUse(size_);

    return true;
}

bool JSFile::Save(Serializer& dest) const
{
    if (size_ == 0)
        return false;

    dest.Write(data_, size_);

    return true;
}

}
