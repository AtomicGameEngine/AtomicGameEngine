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
#include <Atomic/IO/Log.h>

#include <AtomicNET/NETScript/NETScript.h>

#include <ToolCore/NETTools/NETProjectSystem.h>
#include <ToolCore/NETTools/AtomicNETService.h>
#include "AEEditorNETService.h"

namespace AtomicEditor
{
    
    EditorNETService::EditorNETService(Context* context) : Object(context)
    {

        Initialize();

    }


    EditorNETService::~EditorNETService()
    {

    }

    bool EditorNETService::Initialize()
    {
        RegisterNETScriptLibrary(context_);

        context_->RegisterSubsystem(new NETProjectSystem(context_));

        netService_ = new AtomicNETService(context_);

        if (!netService_->Start())
        {
            netService_ = nullptr;
            ATOMIC_LOGERRORF("Unable to start AtomicNETService");
            return false;
        }

        context_->RegisterSubsystem(netService_);

        return true;
    }

}
