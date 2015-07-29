// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_select.h>

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceEvents.h>

#include <Atomic/UI/UI.h>

#include "../AEEditor.h"
#include "../Project/AEProject.h"
#include "UIListView.h"
#include "UIProjectFrame.h"
#include "../Project/ProjectUtils.h"

#include "UI/Modal/UIModalOps.h"

using namespace tb;


namespace AtomicEditor
{

ProjectFrame::ProjectFrame(Context* context) :
    AEWidget(context)
{
    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/projectframe.tb.txt");

    delegate_->SetID(TBIDC("projectframe_delegate"));

    delegate_->SetGravity(WIDGET_GRAVITY_TOP_BOTTOM);

    // folders
    TBLayout* foldercontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("foldercontainer"));
    assert(foldercontainer);

    folderList_ = new ListView(context_, "folderList_");
    folderList_->GetRootList()->SetGravity(WIDGET_GRAVITY_ALL);

    TBWidgetDelegate* folderListWD = folderList_->GetWidgetDelegate();

    folderListWD->SetID(TBIDC("folerList_delegate"));

    folderListWD->SetGravity(WIDGET_GRAVITY_ALL);

    foldercontainer->AddChild(folderListWD);

    SubscribeToEvent(E_FILECHANGED, HANDLER(ProjectFrame, HandleFileChanged));

    InitializeMenuSources();

}

ProjectFrame::~ProjectFrame()
{

}

void ProjectFrame::InitializeMenuSources()
{

    MenubarItem* item;
    item = new MenubarItem("Folder", TBIDC("create_folder"));
    item->SetSkinImage(TBIDC("Folder.icon"));
    menuCreateSource.AddItem(item);

    menuCreateSource.AddItem(new MenubarItem("-"));

    item = new MenubarItem("Component", TBIDC("create_component"));
    item->SetSkinImage(TBIDC("JavascriptBitmap"));
    menuCreateSource.AddItem(item);
    item = new MenubarItem("Script", TBIDC("create_script"));
    item->SetSkinImage(TBIDC("JavascriptBitmap"));
    menuCreateSource.AddItem(item);
    item = new MenubarItem("Module", TBIDC("create_module"));
    item->SetSkinImage(TBIDC("JavascriptBitmap"));
    menuCreateSource.AddItem(item);

    menuCreateSource.AddItem(new MenubarItem("-"));

    item = new MenubarItem("2D Level", TBIDC("create_2d_level"));
    item->SetSkinImage(TBIDC("2DLevelBitmap"));
    menuCreateSource.AddItem(item);

}

void ProjectFrame::Refresh()
{
    String cfolder = currentContentFolder_;
    currentContentFolder_ = "";
    RefreshFolders();
    SelectCurrentContentFolder(cfolder);
}

void ProjectFrame::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    // Don't currently do anything here
    /*
    using namespace FileChanged;
    const String& fileName = eventData[P_FILENAME].GetString();
    const String& resourceName = eventData[P_RESOURCENAME].GetString();
    */
}


void ProjectFrame::ScanDirForFolders(Vector<String>& dirs, const String& fullPath)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    Vector<String> _dirs;
    fileSystem->ScanDir(_dirs, fullPath, "", SCAN_DIRS, false);

    for (unsigned i = 0; i < _dirs.Size(); i++)
    {
        const String& dir = _dirs[i];

        if (dir == "." || dir == ".." || dir.EndsWith("/..") || dir.EndsWith("/."))
            continue;

        dirs.Push(dir);
    }

}

void ProjectFrame::RecursiveAddFolder(ListViewItem* parent, const String& fullpath, const String& folderName)
{
    TBID id = TBIDC(fullpath.CString());
    tbidToFolder_[id] = fullpath;

    ListViewItem* child = parent->AddChild(folderName.CString(), "Folder.icon", TBIDC(fullpath.CString()));

    Vector<String> dirs;
    ScanDirForFolders(dirs, fullpath);

    for (unsigned i = 0; i < dirs.Size(); i++)
    {
        const String& dir = dirs[i];
        RecursiveAddFolder(child, AddTrailingSlash(fullpath + dir), dir);
    }

}

void ProjectFrame::Clear()
{
    currentContentFolder_ = "";

    tbidToContent_.Clear();
    tbidToContentFolder_.Clear();
    tbidToFolder_.Clear();

    TBLayout* container = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("contentcontainer"));
    assert(container);

    folderList_->DeleteAllItems();
    container->DeleteAllChildren();

}

void ProjectFrame::RefreshFolders()
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    tbidToFolder_.Clear();

    String resourcePath = project->GetResourcePath();
    currentContentFolder_ = resourcePath;

    TBID id = TBID(resourcePath.CString());
    tbidToFolder_[id] = resourcePath;

    folderList_->DeleteAllItems();
    ListViewItem* resources = folderList_->AddItem("Resources", "Folder.icon", id);

    Vector<String> dirs;
    ScanDirForFolders(dirs, resourcePath);

    for (unsigned i = 0; i < dirs.Size(); i++)
    {
        const String& dir = dirs[i];
        RecursiveAddFolder(resources, AddTrailingSlash(resourcePath + dir), dir);
    }

    resources->SetExpanded(true);
    RefreshContent(resourcePath);

    folderList_->GetRootList()->SetValue(0);

}

void ProjectFrame::ScanContentDirForContent(Vector<String>& folders, Vector<String>& content, const String& fullPath)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    folders.Clear();
    content.Clear();

    Vector<String> _folders;
    fileSystem->ScanDir(_folders, fullPath, "", SCAN_DIRS, false);

    for (unsigned i = 0; i < _folders.Size(); i++)
    {
        const String& dir = _folders[i];

        if (dir == "." || dir == ".." || dir.EndsWith("/..") || dir.EndsWith("/."))
            continue;

        folders.Push(dir);
    }

    fileSystem->ScanDir(content, fullPath, "", SCAN_FILES, false);


}

TBLayout* ProjectFrame::CreateButtonLayout(const String& fullpath, const String& text)
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();
    FileSystem* fs = GetSubsystem<FileSystem>();

    String pathName, fileName, extension;
    SplitPath(fullpath, pathName, fileName, extension);

    TBID bitmapID = TBIDC("Folder.icon");

    if (fs->FileExists(fullpath))
        bitmapID = TBIDC("FileBitmap");

    if (extension.Length())
    {
        if (extension == ".js")
        {
            if (project->IsComponentsDirOrFile(fullpath))
            {
                bitmapID = TBIDC("ComponentBitmap");
            }
            else
            {
                bitmapID = TBIDC("JavascriptBitmap");
            }
        }
    }

    TBLayout* blayout = new TBLayout();

    TBWidget* spacer = new TBWidget();
    spacer->SetRect(TBRect(0, 0, 8, 8));
    blayout->AddChild(spacer);

    TBButton* button = new TBButton();

    LayoutParams lp;
    lp.SetHeight(20);

    TBFontDescription fd;
    fd.SetID(TBIDC("Vera"));
    fd.SetSize(11);

    button->SetGravity(WIDGET_GRAVITY_LEFT);
    TBSkinImage* image = new TBSkinImage(bitmapID);
    image->SetRect(TBRect(0, 0, 12, 12));
    image->SetGravity(WIDGET_GRAVITY_RIGHT);
    blayout->AddChild(image);
    button->SetID(TBIDC(fullpath.CString()));
    button->SetLayoutParams(lp);
    button->SetFontDescription(fd);
    button->SetText(text.CString());
    button->SetSkinBg(TBIDC("TBButton.flat"));
    blayout->AddChild(button);

    return blayout;
}

void ProjectFrame::RefreshContent(const String& fullpath)
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    currentContentFolder_ = fullpath;

    tbidToContent_.Clear();
    tbidToContentFolder_.Clear();

    Vector<String> folders;
    Vector<String> content;
    ScanContentDirForContent(folders, content, fullpath);

    TBLayout* container = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("contentcontainer"));
    assert(container);

    container->DeleteAllChildren();

    if (fullpath != project->GetResourcePath())
    {
        LayoutParams lp;
        lp.SetHeight(20);

        TBFontDescription fd;
        fd.SetID(TBIDC("Vera"));
        fd.SetSize(11);

        TBButton* button = new TBButton();
        button->SetGravity(WIDGET_GRAVITY_LEFT);
        button->SetText("..                     ");
        button->SetID(TBIDC(".."));
        button->SetSkinBg(TBIDC("TBButton.flat"));
        button->SetLayoutParams(lp);
        button->SetFontDescription(fd);

        container->AddChild(button);
    }

    for (unsigned i = 0; i < folders.Size(); i++)
    {
        String contentID = AddTrailingSlash(fullpath + folders[i]);

        container->AddChild(CreateButtonLayout(contentID, folders[i]));
        tbidToContentFolder_[TBIDC(contentID.CString())] = contentID;
    }

    for (unsigned i = 0; i < content.Size(); i++)
    {
        String contentID = fullpath + content[i];
        container->AddChild(CreateButtonLayout(contentID, content[i]));
        tbidToContent_[TBIDC(contentID.CString())] = contentID;
    }

}

bool ProjectFrame::HandlePopMenuEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("create popup"))
        {
            UIModalOps* ops = GetSubsystem<UIModalOps>();

            String resourcePath = GetCurrentContentFolder();

            if (resourcePath.Length())
            {
                if (ev.ref_id == TBIDC("create_component"))
                {
                    if (CheckResourceCreatePath(resourcePath, "Component"))
                        ops->ShowCreateComponent(resourcePath);
                }
                else if (ev.ref_id == TBIDC("create_script"))
                {
                    if (CheckResourceCreatePath(resourcePath, "Script"))
                        ops->ShowCreateScript(resourcePath);
                }
                else if (ev.ref_id == TBIDC("create_module"))
                {
                    if (CheckResourceCreatePath(resourcePath, "Module"))
                        ops->ShowCreateModule(resourcePath);
                }
                else if (ev.ref_id == TBIDC("create_2d_level"))
                {
                    ops->ShowCreate2DLevel(resourcePath);
                }
                else if (ev.ref_id == TBIDC("create_folder"))
                {
                    ops->ShowNewFolder(resourcePath);
                }
            }

            return true;
        }
    }

    return false;

}

bool ProjectFrame::OnEvent(const TBWidgetEvent &ev)
{
    if (!ev.target)
        return false;

    if (HandlePopMenuEvent(ev))
        return true;

    if (ev.type == EVENT_TYPE_CLICK)
    {

        if (ev.target->GetID() == TBIDC("menu create"))
        {
            UI* tbui = GetSubsystem<UI>();
            if (!tbui->GetRootWidget()->GetWidgetByID(TBIDC("create popup")))
                if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("create popup")))
                    menu->Show(&menuCreateSource, TBPopupAlignment());

            return true;
        }

        // context menu
        if (ev.target->GetID() == TBIDC("contextmenu"))
        {
            return OnContextMenuEvent(ev);
        }

        // we clicked the folder list
        if (ev.target->GetID() == TBID("folderList_"))
        {
            TBSelectList* list = (TBSelectList*) ev.target;
            TBID tbid = list->GetSelectedItemID();
            if (tbidToFolder_.Contains(tbid))
            {
                SelectCurrentContentFolder(tbidToFolder_[tbid]);
            }

        }
        else
        {
            if (ev.target->GetID() == TBIDC(".."))
            {
                String parentPath = GetParentPath(currentContentFolder_);
                SelectCurrentContentFolder(parentPath);
                return true;
            }
            // see if we clicked a folder in the content area
            else if (tbidToContentFolder_.Contains(ev.target->GetID()))
            {
                String path = tbidToContentFolder_[ev.target->GetID()];
                SelectCurrentContentFolder(path);
            }
            else if (tbidToContent_.Contains(ev.target->GetID()))
            {
                // we clicked some content
                String contentPath = tbidToContent_[ev.target->GetID()];
                Editor* editor = GetSubsystem<Editor>();
                editor->EditResource(contentPath);

            }
        }

        return false;
    }

    if (ev.type == EVENT_TYPE_RIGHT_POINTER_UP)
    {
        TBPoint pos_in_root(ev.target_x, ev.target_y);

        if (tbidToFolder_.Contains(ev.target->GetID()))
        {
            // right click folder
            String folder = tbidToFolder_[ev.target->GetID()];
            CreateFolderContextMenu(AddTrailingSlash(folder), pos_in_root.x, pos_in_root.y);
        }

        else if (tbidToContent_.Contains(ev.target->GetID()))
        {
            // right click content
            String content = tbidToContent_[ev.target->GetID()];
            CreateContentContextMenu(content, pos_in_root.x, pos_in_root.y);
        }
        else
        {
            if (folderList_->GetRootList()->IsAncestorOf(ev.target))
            {
                // right click in folder area
                CreateFolderContainerContextMenu(pos_in_root.x, pos_in_root.y);
            }
            else
            {
                TBScrollContainer* container = delegate_->GetWidgetByIDAndType<TBScrollContainer>(TBIDC("contentcontainerscroll"));
                if (container->IsAncestorOf(ev.target))
                {
                    CreateContentContainerContextMenu(pos_in_root.x, pos_in_root.y);
                }
            }

        }

        return true;
    }

    return false;
}

void ProjectFrame::SelectCurrentContentFolder(const String& folder)
{
    folderList_->SelectItemByID(TBIDC(folder.CString()));
    if (currentContentFolder_ != folder)
        RefreshContent(folder);
}

bool ProjectFrame::CheckResourceCreatePath(const String& path, const String& resourceType)
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    if (resourceType == "Script")
    {
        if (!project->IsScriptsDirOrFile(path))
        {
            editor->PostModalError("Create Script Error", "Scripts must reside in or in a subfolder of the Scripts folder");
            return false;
        }


    }
    if (resourceType == "Module")
    {
        if (!project->IsModulesDirOrFile(path))
        {
            editor->PostModalError("Create Module Error", "Modules must reside in or in a subfolder of the Modules folder");
            return false;
        }


    }
    if (resourceType == "Component")
    {
        if (!project->IsComponentsDirOrFile(path))
        {
            editor->PostModalError("Create Component Error", "Components must reside in or in a subfolder of the Components folder");
            return false;
        }

    }

    return true;

}

// Context Menus

bool ProjectFrame::OnContextMenuEvent(const TBWidgetEvent &ev)
{
    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (ev.ref_id == TBIDC("reveal_in_finder"))
    {
        ProjectUtils* utils = GetSubsystem<ProjectUtils>();
        utils->RevealInFinder(contextMenuPath_);
    }
    else if (ev.ref_id == TBIDC("create_component"))
    {
        ops->ShowCreateComponent(contextMenuPath_);
    }
    else if (ev.ref_id == TBIDC("create_script"))
    {
        ops->ShowCreateScript(contextMenuPath_);
    }
    else if (ev.ref_id == TBIDC("create_module"))
    {
        ops->ShowCreateModule(contextMenuPath_);
    }
    else if (ev.ref_id == TBIDC("create_2d_level"))
    {
        ops->ShowCreate2DLevel(contextMenuPath_);
    }
    else if (ev.ref_id == TBIDC("new_folder"))
    {
        ops->ShowNewFolder(contextMenuPath_);
    }
    else if (ev.ref_id == TBIDC("delete"))
    {
        ops->ShowResourceDelete(contextMenuPath_);
    }

    return true;
}

String ProjectFrame::GetCurrentContentFolder()
{
    if (currentContentFolder_.Length())
        return currentContentFolder_;

    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    if (project)
        return project->GetResourcePath();

    return "";

}

void ProjectFrame::CreateFolderContextMenu(const String& folder, int x, int y)
{
    contextMenuPath_ = "";

    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    TBMenuWindow *menu = new TBMenuWindow(delegate_, TBIDC("contextmenu"));
    TBGenericStringItemSource *source = menu->GetList()->GetDefaultSource();

    TBGenericStringItem* item = new TBGenericStringItem("Create Folder", TBIDC("new_folder"));
    item->SetSkinImage(TBIDC("FolderCreateBitmap"));
    source->AddItem(item);

    bool isJS = false;
    if (project->IsComponentsDirOrFile(folder))
    {
        isJS = true;
        source->AddItem(new TBGenericStringItem("-"));
        item = new TBGenericStringItem("Create Component", TBIDC("create_component"));
        item->SetSkinImage(TBIDC("ComponentBitmap"));
        source->AddItem(item);
    }
    else if (project->IsScriptsDirOrFile(folder))
    {
        isJS = true;
        source->AddItem(new TBGenericStringItem("-"));
        item = new TBGenericStringItem("Create Script", TBIDC("create_script"));
        item->SetSkinImage(TBIDC("JavascriptBitmap"));
        source->AddItem(item);
    }
    else if (project->IsModulesDirOrFile(folder))
    {
        isJS = true;
        source->AddItem(new TBGenericStringItem("-"));
        item = new TBGenericStringItem("Create Module", TBIDC("create_module"));
        item->SetSkinImage(TBIDC("JavascriptBitmap"));
        source->AddItem(item);
    }

    if (!isJS)
    {
        item = new TBGenericStringItem("Create 2D Level", TBIDC("create_2d_level"));
        item->SetSkinImage(TBIDC("2DLevelBitmap"));
        source->AddItem(item);
    }

    source->AddItem(new TBGenericStringItem("-"));
    item = new TBGenericStringItem("Reveal in Finder", TBIDC("reveal_in_finder"));
    item->SetSkinImage(TBIDC("MagnifierBitmap"));
    source->AddItem(item);

    source->AddItem(new TBGenericStringItem("-"));

    item = new TBGenericStringItem("Delete Folder", TBIDC("delete"));
    item->SetSkinImage(TBIDC("FolderDeleteBitmap"));
    source->AddItem(item);

    menu->Show(source, TBPopupAlignment(TBPoint(x, y)), -1);

    contextMenuPath_ = folder;

}

void ProjectFrame::CreateContentContextMenu(const String& content, int x, int y)
{
    contextMenuPath_ = "";

    TBMenuWindow *menu = new TBMenuWindow(delegate_, TBIDC("contextmenu"));
    TBGenericStringItemSource *source = menu->GetList()->GetDefaultSource();
    source->AddItem(new TBGenericStringItem("Delete", TBIDC("delete")));

    source->AddItem(new TBGenericStringItem("-"));
    source->AddItem(new TBGenericStringItem("Reveal in Finder", TBIDC("reveal_in_finder")));

    menu->Show(source, TBPopupAlignment(TBPoint(x, y)), -1);

    contextMenuPath_ = content;
}

void ProjectFrame::CreateFolderContainerContextMenu(int x, int y)
{
    contextMenuPath_ = "";

    TBMenuWindow *menu = new TBMenuWindow(delegate_, TBIDC("contextmenu"));
    TBGenericStringItemSource *source = menu->GetList()->GetDefaultSource();
    source->AddItem(new TBGenericStringItem("Create Folder", TBIDC("new_folder")));

    source->AddItem(new TBGenericStringItem("-"));
    source->AddItem(new TBGenericStringItem("Reveal in Finder", TBIDC("reveal_in_finder")));

    menu->Show(source, TBPopupAlignment(TBPoint(x, y)), -1);

    contextMenuPath_ = currentContentFolder_;

}

void ProjectFrame::CreateContentContainerContextMenu(int x, int y)
{
    contextMenuPath_ = "";

    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    TBMenuWindow *menu = new TBMenuWindow(delegate_, TBIDC("contextmenu"));
    TBGenericStringItemSource *source = menu->GetList()->GetDefaultSource();

    TBGenericStringItem* item = new TBGenericStringItem("Create Folder", TBIDC("new_folder"));
    item->SetSkinImage(TBIDC("Folder.icon"));

    source->AddItem(item);

    bool isJS = false;
    if (project->IsComponentsDirOrFile(currentContentFolder_))
    {
        isJS = true;
        source->AddItem(new TBGenericStringItem("-"));
        item = new TBGenericStringItem("Create Component", TBIDC("create_component"));
        item->SetSkinImage(TBIDC("ComponentBitmap"));
        source->AddItem(item);
    }
    else if (project->IsScriptsDirOrFile(currentContentFolder_))
    {
        isJS = true;
        source->AddItem(new TBGenericStringItem("-"));
        item = new TBGenericStringItem("Create Script", TBIDC("create_script"));
        item->SetSkinImage(TBIDC("JavascriptBitmap"));
        source->AddItem(item);
    }
    else if (project->IsModulesDirOrFile(currentContentFolder_))
    {
        isJS = true;
        source->AddItem(new TBGenericStringItem("-"));
        item = new TBGenericStringItem("Create Module", TBIDC("create_module"));
        item->SetSkinImage(TBIDC("JavascriptBitmap"));
        source->AddItem(item);
    }

    if (!isJS)
    {
        item = new TBGenericStringItem("Create 2D Level", TBIDC("create_2d_level"));
        item->SetSkinImage(TBIDC("2DLevelBitmap"));
        source->AddItem(item);
    }

    source->AddItem(new TBGenericStringItem("-"));
    source->AddItem(new TBGenericStringItem("Reveal in Finder", TBIDC("reveal_in_finder")));

    menu->Show(source, TBPopupAlignment(TBPoint(x, y)), -1);

    contextMenuPath_ = currentContentFolder_;

}


}
