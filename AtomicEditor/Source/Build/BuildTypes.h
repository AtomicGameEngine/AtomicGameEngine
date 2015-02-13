// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Container/Str.h>

#pragma once

using namespace Atomic;

namespace AtomicEditor
{

struct BuildResourceEntry
{
    // the resource directory this resource came from
    String resourceDir_;

    // the absolute path to the resource
    String absolutePath_;

    // the path within the package
    String packagePath_;

    // the offset in the package file
    unsigned offset_;

    // the size in the resource
    unsigned size_;

    // the checksum_
    unsigned checksum_;

    BuildResourceEntry()
    {
        offset_ = size_ = checksum_ = 0;
    }
};

}
