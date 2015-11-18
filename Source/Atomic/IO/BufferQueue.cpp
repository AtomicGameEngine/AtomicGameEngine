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

#include "../Precompiled.h"

#include "../IO/BufferQueue.h"

#include "../DebugNew.h"

// This BufferQueue implementation is based on the BufferedSoundStream.
//
// It is a Deserializer and a Serializer. However, it should only be written
// to in large chunks because some writes cause allocation. Reads should
// perform well reguardless of the read chunk sizes.

namespace Atomic
{

BufferQueue::BufferQueue(Context* context) :
    Object(context),
    position_(0)
{
}

BufferQueue::~BufferQueue()
{
}

unsigned BufferQueue::Read(void* dest_, unsigned numBytes)
{
    char* dest((char*)dest_);
    MutexLock lock(bufferMutex_);

    unsigned outBytes = 0;

    while (numBytes && buffers_.Size())
    {
        // Copy as much from the front buffer as possible, then discard it and move to the next
        List<Pair<SharedArrayPtr<signed char>, unsigned> >::Iterator front = buffers_.Begin();

        unsigned copySize = front->second_ - position_;
        if (copySize > numBytes)
            copySize = numBytes;

        memcpy(dest, front->first_.Get() + position_, copySize);
        position_ += copySize;
        if (position_ >= front->second_)
        {
            buffers_.PopFront();
            position_ = 0;
        }

        dest += copySize;
        outBytes += copySize;
        numBytes -= copySize;
    }

    size_ -= outBytes;
    return outBytes;
}

unsigned BufferQueue::Write(const void* data, unsigned numBytes)
{
    if (data && numBytes)
    {
        MutexLock lock(bufferMutex_);

        SharedArrayPtr<signed char> newBuffer(new signed char[numBytes]);
        memcpy(newBuffer.Get(), data, numBytes);
        buffers_.Push(MakePair(newBuffer, numBytes));
        size_ += numBytes;
        return numBytes;
    }
    return 0;
}

void BufferQueue::Write(SharedArrayPtr<signed char> data, unsigned numBytes)
{
    if (data && numBytes)
    {
        MutexLock lock(bufferMutex_);

        buffers_.Push(MakePair(data, numBytes));
        size_ += numBytes;
    }
}

void BufferQueue::Write(SharedArrayPtr<unsigned char> data, unsigned numBytes)
{
    if (data && numBytes)
    {
        MutexLock lock(bufferMutex_);

        buffers_.Push(MakePair(ReinterpretCast<signed char>(data), numBytes));
        size_ += numBytes;
    }
}

void BufferQueue::Clear()
{
    MutexLock lock(bufferMutex_);

    buffers_.Clear();
    position_ = 0;
    size_ = 0;
}

}
