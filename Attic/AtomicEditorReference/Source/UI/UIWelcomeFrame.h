// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEWidget.h"

using namespace Atomic;

namespace tb
{
    class TBLayout;
}


namespace AtomicEditor
{

class WelcomeFrame : public AEWidget
{
    OBJECT(WelcomeFrame);

public:
    /// Construct.
    WelcomeFrame(Context* context);
    /// Destruct.
    virtual ~WelcomeFrame();

    bool OnEvent(const TBWidgetEvent &ev);

    void UpdateRecentProjects();

private:

    struct ExampleInfo
    {
        String name;
        String folder;
        String module;
        String screenshot;
        TBID id;
    };

    void AddExample(const String& name, const String& desc, const String& screenshot, const String &folder, const String &module);
    void FillExamples();
    bool HandleExampleCopy(const String& name, const String& exampleFolder, String &atomicProjectFile);    

    String exampleInfoDir_;
    String exampleSourceDir_;

    List<ExampleInfo> exampleInfo_;

    tb::TBLayout* currentExampleLayout_;
    int exampleCount_;


};

}
