//
// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
// Copyright (c) 2011-2017, The Game Engine Company LLC (Apache License 2.0)
// Copyright (c) 2013-2014 Chukong Technologies Inc.
// Copyright (c) 2010-2012 cocos2d-x.org
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

#include <jni.h>

namespace Atomic
{

struct AtomicJNIMethodInfo
{
    JNIEnv *GetEnv();
    jclass    classID;
    jmethodID methodID;
};

class AtomicJNI
{

public:

    static jclass GetClassID(const String& className, JNIEnv *env = 0);
    static bool GetStaticMethodInfo(AtomicJNIMethodInfo& methodinfo, const String& className, const String& methodName, const String& paramCode);
    static bool GetMethodInfo(AtomicJNIMethodInfo& methodinfo, const String& className, const String& methodName, const String& paramCode);
    static String JavaStringToString(jstring jstr);

private:
};

}

