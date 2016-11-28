//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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
#include "Atomic\IO\File.h"
#include "Atomic\IO\FileSystem.h"
#include "Atomic\Resource\ResourceCache.h"

#include "../Assets/Asset.h"
#include "../Assets/AssetDatabase.h"

#include "Command.h"

using namespace Atomic;

namespace ToolCore
{

    class CompressCmd : public Command
    {
        ATOMIC_OBJECT(CompressCmd, Command);

    public:

        CompressCmd(Context* context);
        virtual ~CompressCmd();

        bool Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg);

        void Run();

        bool RequiresProjectLoad() { return false; }

    private:
        void CompressFile(SharedPtr<File> file);

        SharedPtr<AssetDatabase> db_;
        SharedPtr<FileSystem> fileSystem_;
        SharedPtr<ResourceCache> cache_;
        SharedPtr<Asset> asset_;

        String compressDirectory_;
        String cachePath_;
    };

}
