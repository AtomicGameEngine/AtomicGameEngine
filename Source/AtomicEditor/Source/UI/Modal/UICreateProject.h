
#pragma once

#include "UIModalOps.h"

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_select_item.h>

namespace AtomicEditor
{

class UICreateProject: public UIModalOpWindow
{
    OBJECT(UICreateProject);

public:

    UICreateProject(Context* context, const String& templateFolder, const String& imagePath = String::EMPTY);
    virtual ~UICreateProject();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    bool DoCreate(const String& folder, const String& projectName);

    TBEditField* projectNameField_;
    TBEditField* projectPathField_;

    String templateFolder_;

};

}
