// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceEvents.h>

#include "ResourceEditor.h"

#include "../UI/UIMainFrame.h"
#include "../UI/UIResourceFrame.h"

namespace AtomicEditor
{

class EditorTabLayout: public TBLayout
{
public:

    ResourceEditor* editor_;
    TBButton* button_;
    TBButton* close_;
    TBTabContainer* container_;

    void SetValue(int value)
    {
        button_->SetValue(value);
    }

    bool OnEvent(const TBWidgetEvent &ev)
    {
        if (ev.type == EVENT_TYPE_CLICK || ev.type == EVENT_TYPE_POINTER_DOWN)
        {
            if (ev.target->GetID() == TBIDC("tabclose"))
            {
                container_->OnEvent(ev);
                editor_->Close();
                return true;
            }
            else
            {
                TBWidgetEvent nevent = ev;
                nevent.target = this;
                container_->OnEvent(nevent);
            }
        }

        return false;
    }
};

ResourceEditor::ResourceEditor(Context* context, const String& fullpath, TBTabContainer* container):
    Object(context), fullpath_(fullpath), container_(container),
    editorTabLayout_(0), rootContentWidget_(0), button_(0)
{

    String filename = GetFileNameAndExtension(fullpath_);

    editorTabLayout_ = new EditorTabLayout();
    editorTabLayout_->SetID(TBIDC("tab"));

    button_ = new TBButton();
    button_->SetText(filename.CString());
    button_->SetSqueezable(true);
    button_->SetSkinBg(TBIDC("TBButton.flat"));
    button_->SetValue(1);
    editorTabLayout_->AddChild(button_);

    TBButton* closebutton = new TBButton();
    editorTabLayout_->AddChild(closebutton);
    closebutton->SetSkinBg(TBIDC("TBWindow.close"));
    closebutton->SetIsFocusable(false);
    closebutton->SetID(TBIDC("tabclose"));

    editorTabLayout_->editor_ = this;
    editorTabLayout_->button_ = button_;
    editorTabLayout_->close_ = closebutton;
    editorTabLayout_->container_ = container;

    container_->GetTabLayout()->AddChild(editorTabLayout_);

    rootContentWidget_ = new TBWidget();
    rootContentWidget_->SetGravity(WIDGET_GRAVITY_ALL);
    container_->GetContentRoot()->AddChild(rootContentWidget_);

    SubscribeToEvent(E_FILECHANGED, HANDLER(ResourceEditor, HandleFileChanged));
}

ResourceEditor::~ResourceEditor()
{

}

void ResourceEditor::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    using namespace FileChanged;
    const String& fileName = eventData[P_FILENAME].GetString();
    const String& resourceName = eventData[P_RESOURCENAME].GetString();

    if (fullpath_ == fileName)
    {
        FileSystem* fs = GetSubsystem<FileSystem>();
        if (!fs->FileExists(fullpath_))
            Close();
    }
}

void ResourceEditor::Close(bool navigateToAvailabeResource)
{
    // keep us alive through the close
    SharedPtr<ResourceEditor> keepalive(this);

    container_->GetTabLayout()->RemoveChild(editorTabLayout_);

    MainFrame* frame = GetSubsystem<MainFrame>();
    ResourceFrame* rframe = frame->GetResourceFrame();
    rframe->CloseResourceEditor(this, navigateToAvailabeResource);
}

}
