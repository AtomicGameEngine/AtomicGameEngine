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

#include <Atomic/Engine/Application.h>

namespace Atomic
{

    class JSVM;

    class AppBase : public Application
    {
        ATOMIC_OBJECT(AppBase, Application)

    public:
        /// Construct.
        AppBase(Context* context);
        virtual ~AppBase();

        /// Setup before engine initialization. 
        virtual void Setup();
        /// Setup after engine initialization. L
        virtual void Start();
        /// Cleanup after the main loop. 
        virtual void Stop();

        void AddArgument(const String& argument) { arguments_.Push(argument); }

        virtual void ProcessArguments();

        static void AddEngineConfigSearchPath(const String& path) { engineConfigSearchPaths_.Push(path); }

    protected:

        void ReadEngineConfig();

        Vector<String> arguments_;
        static Vector<String> engineConfigSearchPaths_;

        SharedPtr<JSVM> vm_;

    private:

        void HandleLogMessage(StringHash eventType, VariantMap& eventData);
        void HandleJSError(StringHash eventType, VariantMap& eventData);

    };

}
