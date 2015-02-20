// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEWidget.h"
#include "UIMenubar.h"

using namespace Atomic;

namespace tb
{
    class TBLayout;
    class TBMenuWindow;
}

namespace AtomicEditor
{

class ListView;
class ListViewItem;

class ProjectFrame : public AEWidget
{

    OBJECT(ProjectFrame);

    void ScanDirForFolders(Vector<String>& dirs, const String& fullPath);
    void ScanContentDirForContent(Vector<String>& folders, Vector<String>& content, const String& fullPath);

    void RecursiveAddFolder(ListViewItem* parent, const String& fullpath, const String& folderName);

public:
    /// Construct.
    ProjectFrame(Context* context);
    /// Destruct.
    virtual ~ProjectFrame();

    void RefreshFolders();
    void RefreshContent(const String& fullpath);

    bool OnEvent(const TBWidgetEvent &ev);


    void Clear();
    void Refresh();

    void SelectCurrentContentFolder(const String& folder);

    String GetCurrentContentFolder();

private:

    void InitializeMenuSources();
    bool HandlePopMenuEvent(const TBWidgetEvent &ev);
    bool CheckResourceCreatePath(const String& path, const String& resourceType);

    bool OnContextMenuEvent(const TBWidgetEvent &ev);
    void CreateFolderContextMenu(const String& folder, int x, int y);
    void CreateContentContextMenu(const String& content, int x, int y);
    void CreateFolderContainerContextMenu(int x, int y);
    void CreateContentContainerContextMenu(int x, int y);

    void HandleFileChanged(StringHash eventType, VariantMap& eventData);

    TBLayout* CreateButtonLayout(const String& fullpath, const String &text);

    SharedPtr<ListView> folderList_;
    HashMap<uint32, String> tbidToFolder_;
    HashMap<uint32, String> tbidToContent_;
    HashMap<uint32, String> tbidToContentFolder_;

    MenubarItemSource menuCreateSource;

    String currentContentFolder_;

    // When operating with context menu
    String contextMenuPath_;

};

}
