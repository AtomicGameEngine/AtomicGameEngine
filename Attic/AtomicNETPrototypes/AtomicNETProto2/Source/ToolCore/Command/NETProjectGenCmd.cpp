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

#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../NETTools/NETProjectGen.h"

#include "NETProjectGenCmd.h"

namespace ToolCore
{

NETProjectGenCmd::NETProjectGenCmd(Context* context) : Command(context)
{

}

NETProjectGenCmd::~NETProjectGenCmd()
{

}

bool NETProjectGenCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();

    projectFile_ = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;
    scriptPlatform_ = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;

    if (argument != "net-projectgen")
    {
        errorMsg = "Unable to parse bind command";
        return false;
    }

    if (!projectFile_.Length())
    {
        errorMsg = "Unable to parse net-projectgen command";
        return false;
    }


    if (!scriptPlatform_.Length())
    {
        errorMsg = "Unable to parse net-projectgen command";
        return false;
    }

    return true;
}

void NETProjectGenCmd::Run()
{
    SharedPtr<NETProjectGen> gen(new NETProjectGen(context_));
    gen->SetScriptPlatform(scriptPlatform_);

    gen->LoadProject(projectFile_);

    gen->Generate();

    Finished();
}

}
