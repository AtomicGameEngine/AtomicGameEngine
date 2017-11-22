//
// Copyright (c) 2014-2017 THUNDERBEAST GAMES LLC
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

#include "Command.h"

using namespace Atomic;

namespace ToolCore
{


/// Command for cleaning and regenerating a Project resource cache
class CacheCmd: public Command
{

    /// Example usage:
    /// AtomicTool cache --clean --project C:\Path\To\MyProject (cleans cache folder)
    /// AtomicTool cache generate --project C:\Path\To\MyProject (regenerates the project cache)
    /// AtomicTool cache generate --clean --project C:\Path\To\MyProject (cleans and then regenerates the project cache)

    ATOMIC_OBJECT(CacheCmd, Command)

public:

    CacheCmd(Context* context);
    virtual ~CacheCmd();

    void Run();

    bool RequiresNETService() { return true; }

protected:

    bool ParseInternal(const Vector<String>& arguments, unsigned startIndex, String& errorMsg);

private:

    bool CreateCacheDirectory() const;
    bool RemoveCacheDirectory() const;

    bool CleanCache() const;

    bool GenerateCache() const;

    bool cleanCache_;
    bool generateCache_;

};

}
