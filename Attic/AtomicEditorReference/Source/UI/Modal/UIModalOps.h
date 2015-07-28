// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <UI/AEWidget.h>

using namespace Atomic;
using namespace tb;

#include <TurboBadger/tb_widgets_listener.h>

namespace tb
{
    class TBDimmer;
    class TBWindow;
    class TBEditField;
}

namespace AtomicEditor
{


class UIModalOpWindow : public AEWidget
{
    OBJECT(UIModalOpWindow);

public:

    UIModalOpWindow(Context* context);
    virtual ~UIModalOpWindow();
    bool OnEvent(const TBWidgetEvent &ev);

    TBWindow* GetWindow() { return window_; }

protected:

    void Center();

    TBWindow* window_;

};

class UIModalOps: public AEWidget, private TBWidgetListener
{
    OBJECT(UIModalOps)

    void ShowCreateComponent(const String& resourcePath);
    void ShowCreateScript(const String& resourcePath);
    void ShowCreateModule(const String& resourcePath);
    void ShowCreate2DLevel(const String& resourcePath);
    void ShowResourceDelete(const String& resourcePath);
    void ShowNewFolder(const String& resourcePath);
    void ShowBuildSettings();
    void ShowBuild();
    void ShowNewProject();
    void ShowCreateProject(const String& templateFolder, const String& imagePath = String::EMPTY);
    void ShowActivation();
    void ShowActivationSuccess();
    void ShowManageLicense();
    void ShowAbout();
    void ShowInfoModule3D(const String& exampleFolder = String::EMPTY, const String &exampleScreenshot = String::EMPTY);
    void ShowPlatformsInfo();
    void ShowEulaAgreement();

    void ShowPlayer();

    // Whether there is a modal window currently showing
    bool ModalActive() { return opWindow_.NotNull(); }

    void ShowProgramOutput(Object* subprocess);
    void PrintToProgramOutput(const String& text);

    /// Allows you to change the program key being output to the output window
    void SetProgramOutputSubprocess(Object* subprocess);

    void Hide();

public:

    UIModalOps(Context* context);
    virtual ~UIModalOps();
    bool OnEvent(const TBWidgetEvent &ev);

    const String& GetResourcePath() { return resourcePath_; }

private:

    void Show();

    void HandleEditorShutdown(StringHash eventType, VariantMap& eventData);

    // TBWidgetListener
    void OnWidgetDelete(TBWidget *widget);
    bool OnWidgetDying(TBWidget *widget);


    TBDimmer* dimmer_;

    String resourcePath_;

    bool isHiding_;

    SharedPtr<UIModalOpWindow> opWindow_;
};

}
