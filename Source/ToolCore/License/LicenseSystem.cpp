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

// BEGIN LICENSE MANAGEMENT

#ifdef ATOMIC_PLATFORM_WINDOWS
#ifndef _MSC_VER
#define _WIN32_IE 0x501
#endif
#include <windows.h>
#include <shellapi.h>
#include <direct.h>
#include <shlobj.h>
#include <sys/types.h>
#include <sys/utime.h>
#endif

#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Core/Context.h>
#include <Atomic/Core/Timer.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/Log.h>

#include "LicenseEvents.h"
#include "LicenseSystem.h"

#include <Poco/MD5Engine.h>
#include <Poco/File.h>

namespace ToolCore
{

LicenseSystem::LicenseSystem(Context* context) :
    Object(context)
    , eulaAgreementConfirmed_(false)

{
    FileSystem* filesystem = GetSubsystem<FileSystem>();

    eulaAgreementPath_ = filesystem->GetAppPreferencesDir("AtomicEditor", "License");
    eulaAgreementPath_ = AddTrailingSlash(eulaAgreementPath_);
    eulaAgreementPath_ += "EulaConfirmed";

}

LicenseSystem::~LicenseSystem()
{

}

void LicenseSystem::Initialize()
{

    FileSystem* filesystem = GetSubsystem<FileSystem>();

    eulaAgreementConfirmed_ = filesystem->FileExists(eulaAgreementPath_);

    if (!eulaAgreementConfirmed_)
    {
        SendEvent(E_LICENSE_EULAREQUIRED);
        return;
    }
    else
    {
        SendEvent(E_LICENSE_EULAACCEPTED);
    }

}

void LicenseSystem::LicenseAgreementConfirmed()
{
    eulaAgreementConfirmed_ = true;

    SharedPtr<File> file(new File(context_, eulaAgreementPath_, FILE_WRITE));
    file->WriteInt(1);
    file->Close();
}


}

// END LICENSE MANAGEMENT
