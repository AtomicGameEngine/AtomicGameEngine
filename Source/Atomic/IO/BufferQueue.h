//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../Core/Object.h"
#include "../Core/Mutex.h"
#include "../Container/ArrayPtr.h"
#include "../Container/List.h"
#include "../Container/Pair.h"
#include "../IO/Deserializer.h"
#include "../IO/Serializer.h"

namespace Atomic
{

/// %Act as a continuous byte buffer which can be appened indefinatly.
class ATOMIC_API BufferQueue : public Object, public Deserializer, public Serializer
{
    ATOMIC_OBJECT(BufferQueue, Object)

public:
    /// Construct.
    BufferQueue(Context* context);
    /// Destruct.
    virtual ~BufferQueue();

    /// Seek operation is not supported for a BufferQueue.
    virtual unsigned Seek(unsigned position) { return 0; }

    /// Produce data into destination. Return number of bytes produced.
    virtual unsigned Read(void* dest, unsigned numBytes);

    /// Buffer data. Makes a copy of it. Returns size passed in.
    virtual unsigned Write(const void* data, unsigned size);
    /// Buffer data by taking ownership of it.
    void Write(SharedArrayPtr<signed char> data, unsigned numBytes);
    /// Buffer data by taking ownership of it.
    void Write(SharedArrayPtr<unsigned char> data, unsigned numBytes);

    /// Remove all buffered data.
    void Clear();

private:
    /// Buffers and their sizes.
    List<Pair<SharedArrayPtr<signed char>, unsigned> > buffers_;
    /// Byte position in the front most buffer.
    unsigned position_;
    /// Mutex for buffer data.
    mutable Mutex bufferMutex_;
};

}
