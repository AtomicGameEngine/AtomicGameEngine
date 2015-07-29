// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

//
// Portions Copyright (c) 2008-2014 the Urho3D project.
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

#include "AtomicEditor.h"

#include "Atomic/Core/StringUtils.h"
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Container/ArrayPtr.h>

#include <LZ4/lz4.h>
#include <LZ4/lz4hc.h>

#include "BuildBase.h"
#include "ResourcePackager.h"

namespace AtomicEditor
{

ResourcePackager::ResourcePackager(Context* context, BuildBase* buildBase) : Object(context)
  , buildBase_(buildBase)
  , checksum_(0)
{


}

ResourcePackager::~ResourcePackager()
{

}

bool ResourcePackager::WritePackageFile(const String& destFilePath)
{
    buildBase_->BuildLog("Writing package");

    SharedPtr<File> dest(new File(context_, destFilePath, FILE_WRITE));
    if (!dest->IsOpen())
    {
        buildBase_->BuildError("Could not open output file " + destFilePath);
        return false;
    }

    // Write ID, number of files & placeholder for checksum
    WriteHeader(dest);

    for (unsigned i = 0; i < resourceEntries_.Size(); i++)
    {
        BuildResourceEntry* entry = resourceEntries_[i];

        // Write entry (correct offset is still unknown, will be filled in later)
        dest->WriteString(entry->packagePath_);
        dest->WriteUInt(entry->offset_);
        dest->WriteUInt(entry->size_);
        dest->WriteUInt(entry->checksum_);
    }

    unsigned totalDataSize = 0;

    // Write file data, calculate checksums & correct offsets
    for (unsigned i = 0; i < resourceEntries_.Size(); i++)
    {
        BuildResourceEntry* entry = resourceEntries_[i];

        entry->offset_ = dest->GetSize();

        File srcFile(context_, entry->absolutePath_);
        if (!srcFile.IsOpen())
        {
            buildBase_->BuildError("Could not open input file " + entry->absolutePath_);
            return false;
        }

        unsigned dataSize = entry->size_;
        totalDataSize += dataSize;
        SharedArrayPtr<unsigned char> buffer(new unsigned char[dataSize]);

        if (srcFile.Read(&buffer[0], dataSize) != dataSize)
        {
            buildBase_->BuildError("Could not read input file " + entry->absolutePath_);
            return false;
        }

        srcFile.Close();

        for (unsigned j = 0; j < dataSize; ++j)
        {
            checksum_ = SDBMHash(checksum_, buffer[j]);
            entry->checksum_ = SDBMHash(entry->checksum_, buffer[j]);
        }

        // might not want to compress for WebGL
        //if (!compress_)
        //{
        //    PrintLine(entries_[i].name_ + " size " + String(dataSize));
        //    dest.Write(&buffer[0], entries_[i].size_);
        //}
        //else
        //{

        unsigned compressedBlockSize_ = 32768;

        SharedArrayPtr<unsigned char> compressBuffer(new unsigned char[LZ4_compressBound(compressedBlockSize_)]);

        unsigned pos = 0;
        unsigned totalPackedBytes = 0;

        while (pos < dataSize)
        {
            unsigned unpackedSize = compressedBlockSize_;
            if (pos + unpackedSize > dataSize)
                unpackedSize = dataSize - pos;

            unsigned packedSize = LZ4_compressHC((const char*)&buffer[pos], (char*)compressBuffer.Get(), unpackedSize);
            if (!packedSize)
            {
                buildBase_->BuildError("LZ4 compression failed for file " + entry->absolutePath_ + " at offset " + pos);
                return false;
            }

            dest->WriteUShort(unpackedSize);
            dest->WriteUShort(packedSize);
            dest->Write(compressBuffer.Get(), packedSize);
            totalPackedBytes += 6 + packedSize;

            pos += unpackedSize;
        }

        buildBase_->BuildLog(entry->absolutePath_ + " in " + String(dataSize) + " out " + String(totalPackedBytes));
        }
    //}

    // Write package size to the end of file to allow finding it linked to an executable file
    unsigned currentSize = dest->GetSize();
    dest->WriteUInt(currentSize + sizeof(unsigned));

    // Write header again with correct offsets & checksums
    dest->Seek(0);
    WriteHeader(dest);

    for (unsigned i = 0; i < resourceEntries_.Size(); i++)
    {
        BuildResourceEntry* entry = resourceEntries_[i];

        dest->WriteString(entry->packagePath_);
        dest->WriteUInt(entry->offset_);
        dest->WriteUInt(entry->size_);
        dest->WriteUInt(entry->checksum_);
    }

    buildBase_->BuildLog("Number of files " + String(resourceEntries_.Size()));
    buildBase_->BuildLog("File data size " + String(totalDataSize));
    buildBase_->BuildLog("Package size " + String(dest->GetSize()));

    return true;
}

void ResourcePackager::WriteHeader(File* dest)
{
    dest->WriteFileID("ULZ4");
    dest->WriteUInt(resourceEntries_.Size());
    dest->WriteUInt(checksum_);
}


void ResourcePackager::GeneratePackage(const String& destFilePath)
{
    for (unsigned i = 0; i < resourceEntries_.Size(); i++)
    {
        BuildResourceEntry* entry = resourceEntries_[i];

        File file(context_);

        if (!file.Open(entry->absolutePath_))
        {
            buildBase_->BuildError(Atomic::ToString("Could not open resource file %s", entry->absolutePath_.CString()));
            return;
        }

        if (!file.GetSize())
        {
            return;
        }

        entry->size_ = file.GetSize();
    }

    WritePackageFile(destFilePath);

}

void ResourcePackager::AddResourceEntry(BuildResourceEntry* entry)
{
    resourceEntries_.Push(entry);
}

}
