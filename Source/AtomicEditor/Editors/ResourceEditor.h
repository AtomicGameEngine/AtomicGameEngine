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

#include <Atomic/UI/UIButton.h>
#include <Atomic/UI/UITabContainer.h>

using namespace Atomic;
using namespace tb;

namespace AtomicEditor
{

class EditorTabLayout;

class ResourceEditor: public Object
{
    ATOMIC_OBJECT(ResourceEditor, Object);

public:

    ResourceEditor(Context* context, const String& fullpath, UITabContainer* container);

    virtual ~ResourceEditor();

    UIButton* GetButton() { return button_; }

    virtual bool HasUnsavedModifications() { return modified_; }

    virtual void SetFocus() {}
    virtual void Close(bool navigateToAvailableResource = true);

    virtual bool OnEvent(const TBWidgetEvent &ev) { return false; }

    virtual bool FindText(const String& text, unsigned flags) { return false; }
    virtual void FindTextClose() { }

    virtual bool RequiresInspector() { return false; }

    const String& GetFullPath() { return fullpath_; }

    virtual void Undo() {}
    virtual void Redo() {}

    virtual bool Save() { return true; }

    virtual void Delete();

    UIWidget* GetRootContentWidget() { return rootContentWidget_; }

    virtual void InvokeShortcut(const String& shortcut);

    void RequestClose();

    void SetModified(bool modified);

protected:

    String fullpath_;

    bool modified_;

    EditorTabLayout* editorTabLayout_;

    SharedPtr<UITabContainer> container_;
    SharedPtr<UIWidget> rootContentWidget_;
    SharedPtr<UIButton> button_;

private:

    void HandleFileChanged(StringHash eventType, VariantMap& eventData);
    void HandleRenameResourceNotification(StringHash eventType, VariantMap& eventData);
};

}
