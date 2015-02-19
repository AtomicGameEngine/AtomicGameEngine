// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "UIModalOps.h"

namespace AtomicEditor
{

class UICreateComponent: public UIModalOpWindow
{
    OBJECT(UICreateComponent);

public:

    UICreateComponent(Context* context);
    virtual ~UICreateComponent();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    TBEditField* nameField_;
};

class UICreateScript: public UIModalOpWindow
{
    OBJECT(UICreateScript);

public:

    UICreateScript(Context* context);
    virtual ~UICreateScript();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    TBEditField* nameField_;
};

class UICreateModule: public UIModalOpWindow
{
    OBJECT(UICreateModule);

public:

    UICreateModule(Context* context);
    virtual ~UICreateModule();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    TBEditField* nameField_;
};

class UICreate2DLevel: public UIModalOpWindow
{
    OBJECT(UICreate2DLevel);

public:

    UICreate2DLevel(Context* context);
    virtual ~UICreate2DLevel();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    TBEditField* nameField_;
};


class UIResourceDelete: public UIModalOpWindow
{
    OBJECT(UIResourceDelete);

public:

    UIResourceDelete(Context* context);
    virtual ~UIResourceDelete();
    bool OnEvent(const TBWidgetEvent &ev);

private:

};

class UINewFolder: public UIModalOpWindow
{
    OBJECT(UINewFolder);

public:

    UINewFolder(Context* context);
    virtual ~UINewFolder();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    TBEditField* nameField_;

};

}
