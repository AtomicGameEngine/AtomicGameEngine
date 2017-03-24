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

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../Project/ProjectFile.h"

#include "NewProjectCmd.h"

#include <Poco/File.h>

namespace ToolCore
{

NewProjectCmd::NewProjectCmd(Context* context) : Command(context)
{

}

NewProjectCmd::~NewProjectCmd()
{

}

bool NewProjectCmd::ParseInternal(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    String value = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

    if (argument != "new")
    {
        errorMsg = "Unable to parse new command";
        return false;
    }

    if (!value.Length())
    {
        errorMsg = "Unable to parse new project path";
        return false;
    }

    projectPath_ = value;

    return true;
}

void NewProjectCmd::Run()
{
    Error("NewProjectCmd is under maintenance, please use the AtomicEditor to create a new project");
}

}
