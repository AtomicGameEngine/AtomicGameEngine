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

#include <SDL.h>

#include "../../IO/Log.h"

#include "AndroidJNI.h"

namespace Atomic
{

static bool GetJSEnv(JNIEnv **env)
{
    if (!env)
    {
        return false;

    }

    *env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    return *env != NULL;
}


JNIEnv* AtomicJNIMethodInfo::GetEnv()
{
    JNIEnv *env = NULL;
    GetJSEnv(&env);
    return env;
}

jclass AtomicJNI::GetClassID(const String &className, JNIEnv *env)
{
    JNIEnv *pEnv = env;
    jclass ret   = 0;

    if (!pEnv)
    {
        if (!GetJSEnv(&pEnv))
        {
            return 0;
        }
    }

    ret = pEnv->FindClass(className.CString());

    if (!ret)
    {
        ATOMIC_LOGWARNINGF("AtomicJNI::GetClassID() - Failed to find class of %s", className.CString());
        return 0;
    }

    // ATOMIC_LOGINFOF("AtomicJNI::GetClassID() - Found class of %s", className.CString());

    return ret;
}


bool AtomicJNI::GetStaticMethodInfo(AtomicJNIMethodInfo& methodinfo, const String& className, const String& methodName, const String& paramCode)
{
    jmethodID methodID = 0;
    JNIEnv    *pEnv    = 0;

    if (!GetJSEnv(&pEnv))
    {
        return false;
    }

    jclass classID = GetClassID(className, pEnv);

    if (!classID)
    {
        return false;
    }

    methodID = pEnv->GetStaticMethodID(classID, methodName.CString(), paramCode.CString());

    if (!methodID)
    {
        ATOMIC_LOGWARNINGF("AtomicJNI::GetStaticMethodInfo() - Failed to find method id of %s - %s", methodName.CString(), paramCode.CString());
        return false;
    }

    methodinfo.classID  = classID;
    methodinfo.methodID = methodID;

    return true;

}


bool AtomicJNI::GetMethodInfo(AtomicJNIMethodInfo& methodinfo, const String& className, const String& methodName, const String& paramCode)
{
    jmethodID methodID = 0;
    JNIEnv    *pEnv    = 0;

    if (!GetJSEnv(&pEnv))
    {
        return false;
    }

    jclass classID = GetClassID(className, pEnv);

    if (!classID)
    {
        return false;
    }

    methodID = pEnv->GetMethodID(classID, methodName.CString(), paramCode.CString());
    if (!methodID)
    {
        ATOMIC_LOGWARNINGF("AtomicJNI::GetMethodInfo() - Failed to find method id of %s", methodName.CString());
        return false;
    }

    methodinfo.classID  = classID;
    methodinfo.methodID = methodID;

    return true;
}


String AtomicJNI::JavaStringToString(jstring jstr)
{
    if (jstr == NULL)
    {
        return String::EMPTY;
    }

    JNIEnv *env = 0;

    if (!GetJSEnv(&env))
    {
        return String::EMPTY;
    }

    const char *chars = env->GetStringUTFChars(jstr, NULL);
    String ret(chars);
    env->ReleaseStringUTFChars(jstr, chars);
    return ret;
}

}


