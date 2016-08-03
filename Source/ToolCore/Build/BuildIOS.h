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

#include "BuildBase.h"

namespace ToolCore
{

class Project;

class BuildIOS : public BuildBase
{
    ATOMIC_OBJECT(BuildIOS, BuildBase);

public:

    BuildIOS(Context* context, Project* project);
    virtual ~BuildIOS();

    String GetBuildSubfolder() { return "iOS-Build"; }

    void Build(const String& buildPath);

protected:

    enum BuildPhase
    {
        ConvertPList,
        CodeSign,
        Deploy
    };

    void RunConvertPList();
    void HandleConvertPListComplete(StringHash eventType, VariantMap& eventData);

    void RunCodeSign();
    void HandleCodeSignComplete(StringHash eventType, VariantMap& eventData);

    void RunDeploy();
    void HandleDeployComplete(StringHash eventType, VariantMap& eventData);

    String GenerateInfoPlist();
    String GenerateEntitlements();

    void Initialize();

    BuildPhase currentBuildPhase_;


};

}
