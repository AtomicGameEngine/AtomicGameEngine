// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Core/Context.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include "../AEEvents.h"
#include "../AEEditor.h"

#include "../UI/UIMainFrame.h"
#include "../UI/UIResourceFrame.h"
#include "../UI/UIProjectFrame.h"

#include "../Project/AEProject.h"
#include "AEResourceOps.h"


namespace AtomicEditor
{

/// Construct.
ResourceOps::ResourceOps(Context* context) :
    Object(context)
{
    context->RegisterSubsystem(this);

    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(ResourceOps, HandleEditorShutdown));
}

/// Destruct.
ResourceOps::~ResourceOps()
{

}

bool ResourceOps::CheckCreateComponent(const String& resourcePath, const String& resourceName, bool reportError)
{

    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    String fullpath = resourcePath + resourceName;
    if (!resourceName.EndsWith(".js"))
        fullpath += ".js";

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (fs->FileExists(fullpath))
    {
        if (reportError)
        {
            String errorMsg;
            errorMsg.AppendWithFormat("The component:\n\n%s\n\nalready exists", fullpath.CString());
            editor->PostModalError("Create Component Error", errorMsg);
        }

        return false;
    }

    if (!project->IsComponentsDirOrFile(resourcePath))
    {
        if (reportError)
        {
            String errorMsg;
            errorMsg.AppendWithFormat("Components must reside in or in a subfolder of the Components folder");
            editor->PostModalError("Create Component Error", errorMsg);
        }

        return false;
    }

    return true;

}

bool ResourceOps::CheckCreateScript(const String& resourcePath, const String& resourceName, bool reportError)
{

    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    String fullpath = resourcePath + resourceName;
    if (!resourceName.EndsWith(".js"))
        fullpath += ".js";

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (fs->FileExists(fullpath))
    {
        if (reportError)
        {
            String errorMsg;
            errorMsg.AppendWithFormat("The script:\n\n%s\n\nalready exists", fullpath.CString());
            editor->PostModalError("Create Script Error", errorMsg);
        }

        return false;
    }

    if (!project->IsScriptsDirOrFile(resourcePath))
    {
        if (reportError)
        {
            String errorMsg;
            errorMsg.AppendWithFormat("Scripts must reside in or in a subfolder of the Scripts folder");
            editor->PostModalError("Create Script Error", errorMsg);
        }

        return false;
    }

    return true;

}


bool ResourceOps::CheckCreateModule(const String& resourcePath, const String& resourceName, bool reportError)
{

    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    String fullpath = resourcePath + resourceName;
    if (!resourceName.EndsWith(".js"))
        fullpath += ".js";

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (fs->FileExists(fullpath))
    {
        if (reportError)
        {
            String errorMsg;
            errorMsg.AppendWithFormat("The module:\n\n%s\n\nalready exists", fullpath.CString());
            editor->PostModalError("Create Module Error", errorMsg);
        }

        return false;
    }

    if (!project->IsModulesDirOrFile(resourcePath))
    {
        if (reportError)
        {
            String errorMsg;
            errorMsg.AppendWithFormat("Modules must reside in or in a subfolder of the Modules folder");
            editor->PostModalError("Create Module Error", errorMsg);
        }

        return false;
    }

    return true;

}

bool ResourceOps::CheckCreate2DLevel(const String& resourcePath, const String& resourceName, bool reportError)
{

    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    String fullpath = resourcePath + resourceName;
    if (!resourceName.EndsWith(".tmx"))
        fullpath += ".tmx";

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (fs->FileExists(fullpath))
    {
        if (reportError)
        {
            String errorMsg;
            errorMsg.AppendWithFormat("The level:\n\n%s\n\nalready exists", fullpath.CString());
            editor->PostModalError("Create 2D Level Error", errorMsg);
        }

        return false;
    }

    return true;

}

bool ResourceOps::CopyFile(File* srcFile, const String& destFileName)
{
    SharedPtr<File> destFile(new File(context_, destFileName, FILE_WRITE));
    if (!destFile->IsOpen())
        return false;

    unsigned fileSize = srcFile->GetSize();
    SharedArrayPtr<unsigned char> buffer(new unsigned char[fileSize]);

    unsigned bytesRead = srcFile->Read(buffer.Get(), fileSize);
    unsigned bytesWritten = destFile->Write(buffer.Get(), fileSize);
    return bytesRead == fileSize && bytesWritten == fileSize;

}

void ResourceOps::HandleResourceDelete(const String& resourcePath, bool reportError)
{
    Editor* editor = GetSubsystem<Editor>();
    FileSystem* fs = GetSubsystem<FileSystem>();

    if (fs->DirExists(resourcePath))
    {
        fs->RemoveDir(resourcePath, true);

        GetSubsystem<MainFrame>()->GetProjectFrame()->Refresh();

        return;
    }
    else if (fs->FileExists(resourcePath))
    {
        if (!fs->Delete(resourcePath))
        {
            if (reportError)
            {
                String errorMsg;
                errorMsg.AppendWithFormat("Unable to delete:\n\n %s", resourcePath.CString());
                editor->PostModalError("Delete Resource Error", errorMsg);
            }

            return;
        }

        GetSubsystem<MainFrame>()->GetProjectFrame()->Refresh();

        return;
    }
    else
    {
        if (reportError)
        {
            String errorMsg;
            errorMsg.AppendWithFormat("Unable to find:\n\n %s", resourcePath.CString());
            editor->PostModalError("Delete Resource Error", errorMsg);
        }

        return;
    }

}

void ResourceOps::HandleNewFolder(const String& resourcePath, bool reportError)
{
    Editor* editor = GetSubsystem<Editor>();
    FileSystem* fs = GetSubsystem<FileSystem>();

    if (fs->DirExists(resourcePath) || fs->FileExists(resourcePath))
    {
        if (reportError)
        {
            String errorMsg;
            errorMsg.AppendWithFormat("Already exists:\n\n %s", resourcePath.CString());
            editor->PostModalError("New Folder Error", errorMsg);
        }

        return;
    }

    if (!fs->CreateDir(resourcePath))
    {
        if (reportError)
        {
            String errorMsg;
            errorMsg.AppendWithFormat("Could not create:\n\n %s", resourcePath.CString());
            editor->PostModalError("New Folder Error", errorMsg);
        }

        return;
    }

    // file watcher doesn't currently handle subdir
    GetSubsystem<MainFrame>()->GetProjectFrame()->Refresh();

}

void ResourceOps::HandleCreateComponent(const String& resourcePath, const String& resourceName,
                                        bool navigateToResource, bool reportError)
{
    Editor* editor = GetSubsystem<Editor>();

    if (!CheckCreateComponent(resourcePath, resourceName, reportError))
        return;

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    SharedPtr<File> srcFile = cache->GetFile("AtomicEditor/templates/template_component.js");
    if (srcFile.Null() || !srcFile->IsOpen())
    {
        editor->PostModalError("Create Component Error", "Could not open component template");
        return;
    }

    String fullpath = resourcePath + resourceName;
    if (!resourceName.EndsWith(".js"))
        fullpath += ".js";

    if (!CopyFile(srcFile, fullpath))
    {
        String errorMsg;
        errorMsg.AppendWithFormat("Error copying template:\n\n%s\n\nto:\n\n%s",
                                  "AtomicEditor/template_component.js", fullpath.CString());
        editor->PostModalError("Create Component Error", errorMsg);
        return;
    }

    if (navigateToResource)
    {
        ResourceFrame* rframe = GetSubsystem<MainFrame>()->GetResourceFrame();
        rframe->EditResource(fullpath);
    }

    GetSubsystem<MainFrame>()->GetProjectFrame()->Refresh();

}

void ResourceOps::HandleCreateScript(const String& resourcePath, const String& resourceName,
                                        bool navigateToResource, bool reportError)
{
    Editor* editor = GetSubsystem<Editor>();

    if (!CheckCreateScript(resourcePath, resourceName, reportError))
        return;

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    SharedPtr<File> srcFile = cache->GetFile("AtomicEditor/templates/template_script.js");
    if (srcFile.Null() || !srcFile->IsOpen())
    {
        editor->PostModalError("Create Script Error", "Could not open script template");
        return;
    }

    String fullpath = resourcePath + resourceName;
    if (!resourceName.EndsWith(".js"))
        fullpath += ".js";

    if (!CopyFile(srcFile, fullpath))
    {
        String errorMsg;
        errorMsg.AppendWithFormat("Error copying template:\n\n%s\n\nto:\n\n%s",
                                  "AtomicEditor/template_script.js", fullpath.CString());
        editor->PostModalError("Create Script Error", errorMsg);
        return;
    }

    if (navigateToResource)
    {
        ResourceFrame* rframe = GetSubsystem<MainFrame>()->GetResourceFrame();
        rframe->EditResource(fullpath);
    }

    GetSubsystem<MainFrame>()->GetProjectFrame()->Refresh();

}

void ResourceOps::HandleCreateModule(const String& resourcePath, const String& resourceName,
                                        bool navigateToResource, bool reportError)
{
    Editor* editor = GetSubsystem<Editor>();

    if (!CheckCreateModule(resourcePath, resourceName, reportError))
        return;

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    SharedPtr<File> srcFile = cache->GetFile("AtomicEditor/templates/template_module.js");
    if (srcFile.Null() || !srcFile->IsOpen())
    {
        editor->PostModalError("Create Script Error", "Could not open module template");
        return;
    }

    String fullpath = resourcePath + resourceName;
    if (!resourceName.EndsWith(".js"))
        fullpath += ".js";

    if (!CopyFile(srcFile, fullpath))
    {
        String errorMsg;
        errorMsg.AppendWithFormat("Error copying template:\n\n%s\n\nto:\n\n%s",
                                  "AtomicEditor/template_module.js", fullpath.CString());
        editor->PostModalError("Create Module Error", errorMsg);
        return;
    }

    if (navigateToResource)
    {
        ResourceFrame* rframe = GetSubsystem<MainFrame>()->GetResourceFrame();
        rframe->EditResource(fullpath);
    }

    GetSubsystem<MainFrame>()->GetProjectFrame()->Refresh();

}

void ResourceOps::HandleCreate2DLevel(const String& resourcePath, const String& resourceName,
                                        bool navigateToResource, bool reportError)
{
    Editor* editor = GetSubsystem<Editor>();

    if (!CheckCreate2DLevel(resourcePath, resourceName, reportError))
        return;

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    SharedPtr<File> srcFile = cache->GetFile("AtomicEditor/templates/template_empty.tmx");
    if (srcFile.Null() || !srcFile->IsOpen())
    {
        editor->PostModalError("Create Script Error", "Could not open module template");
        return;
    }

    String fullpath = resourcePath + resourceName;
    if (!resourceName.EndsWith(".tmx"))
        fullpath += ".tmx";

    if (!CopyFile(srcFile, fullpath))
    {
        String errorMsg;
        errorMsg.AppendWithFormat("Error copying template:\n\n%s\n\nto:\n\n%s",
                                  "AtomicEditor/template_empty.tmx", fullpath.CString());
        editor->PostModalError("Create 2D Level Error", errorMsg);
        return;
    }

    if (navigateToResource)
    {
        //ResourceFrame* rframe = GetSubsystem<MainFrame>()->GetResourceFrame();
        //rframe->EditResource(fullpath);
    }

    GetSubsystem<MainFrame>()->GetProjectFrame()->Refresh();

}

void ResourceOps::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}


}
