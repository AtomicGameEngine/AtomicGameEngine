
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

#include <Atomic/Container/RefCounted.h>

namespace Atomic
{

class ATOMIC_API AtomicBuildInfo : public RefCounted
{
    ATOMIC_REFCOUNTED(AtomicBuildInfo)

    public:

    AtomicBuildInfo() {}

    /// Get the Git SHA of the build
    static String GetGitSHA();

    /// Get the Date of the build
    static String GetBuildDate();

    /// Get the Time of the build
    static String GetBuildTime();

    /// Get the build release name
    static String GetBuildName();

    /// Get the build string in form (BuildName): BuildDate BuildTime Git:SHA
    static String GetBuildString();

    /// Get whether this is binary or source tree development build
    static bool GetDevBuild();

    /// Get whether this is a distibution/installer binary build
    static bool GetDistBuild();

    /// Get vendor string
    static const String& GetBuildVendor();

};


}
