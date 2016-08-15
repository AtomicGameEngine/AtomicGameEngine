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

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

    class Project;
    class Subprocess;

    // AtomicProject.dll state (this shouldn't be in resources too)

    enum NETProjectState
    {
        NETPROJECT_CLEAN,
        NETPROJECT_DIRTY,
        NETPROJECT_ERROR
    };

    class NETProjectSystem : public Object
    {
        ATOMIC_OBJECT(NETProjectSystem, Object)

    public:

        NETProjectSystem(Context* context);
        virtual ~NETProjectSystem();

        bool GetVisualStudioAvailable() const { return visualStudioPath_.Length() != 0; }

        const String& GetSolutionPath() const { return solutionPath_; }

        void BuildAtomicProject();        

        /// Open the solution, if opening a source file, better to call OpenSourceFile as will launch VS instance with source file loaded
        /// otherwise, no guarantee where source file will load when multiple VS instances running
        void OpenSolution();       

        void OpenSourceFile(const String& sourceFilePath);

        bool GenerateSolution();

    private:

        void HandleUpdate(StringHash eventType, VariantMap& eventData);

        void HandleNETBuildResult(StringHash eventType, VariantMap& eventData);

        void HandleFileChanged(StringHash eventType, VariantMap& eventData);
        void HandleResourceAdded(StringHash eventType, VariantMap& eventData);
        void HandleResourceRemoved(StringHash eventType, VariantMap& eventData);
        void HandleAssetRenamed(StringHash eventType, VariantMap& eventData);
        void HandleAssetMoved(StringHash eventType, VariantMap& eventData);

        void HandleAssetNew(StringHash eventType, VariantMap& eventData);
        void HandleAssetScanBegin(StringHash eventType, VariantMap& eventData);
        void HandleAssetScanEnd(StringHash eventType, VariantMap& eventData);

        void HandleProjectLoaded(StringHash eventType, VariantMap& eventData);
        void HandleProjectUnloaded(StringHash eventType, VariantMap& eventData);

        void Clear();
        void Initialize();

        String visualStudioPath_;

        String solutionPath_;
        String projectAssemblyPath_;

        float quietPeriod_;

        bool solutionDirty_;
        bool  projectAssemblyDirty_;

        // Visual Studio subprocess
        WeakPtr<Subprocess> vsSubprocess_;

    };

}
