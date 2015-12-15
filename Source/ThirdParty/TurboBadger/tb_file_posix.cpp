// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#include "tb_system.h"

#ifdef TB_FILE_POSIX

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace tb {

TBFileExternalReaderFunction TBFile::reader_function = NULL;

class TBMemoryFile : public TBFile
{
public:
    TBMemoryFile(void* _data, unsigned _size):
        data(_data),
        size(_size),
        currentPos(0)
    {

    }

    virtual ~TBMemoryFile()
    {
        if (data)
            free(data);
    }

    virtual long Size()
    {
        return (long) size;
    }
    virtual size_t Read(void *buf, size_t elemSize, size_t count)
    {
        size_t totalRead = elemSize * count;

        if (currentPos + totalRead > size)
        {
            if (currentPos >= size)
                return 0;

            totalRead = size - currentPos;
        }

        unsigned char* cdata = ((unsigned char*) data) + currentPos;

        memcpy(buf, cdata, totalRead);

        currentPos += totalRead;

        return totalRead;

    }
private:
    void* data;
    size_t size;
    size_t currentPos;
};

// static
TBFile *TBFile::Open(const char *filename, TBFileMode mode)
{
    assert(reader_function);

    void* data = nullptr;
    unsigned size = 0;

    reader_function(filename, &data, &size);

    if (!data || !size)
        return nullptr;

    TBMemoryFile* tbmf = new TBMemoryFile(data, size);

    return tbmf;


}

}; // namespace tb

#endif // TB_FILE_POSIX
