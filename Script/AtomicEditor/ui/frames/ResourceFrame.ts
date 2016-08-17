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

import ScriptWidget = require("ui/ScriptWidget");
import EditorEvents = require("editor/EditorEvents");
import UIEvents = require("ui/UIEvents");
import ResourceEditorProvider from "../ResourceEditorProvider";

// the root content of editor widgets (rootContentWidget property) are extended with an editor field
// so we can access the editor they belong to from the widget itself
interface EditorRootContentWidget extends Atomic.UIWidget {
    editor: Editor.ResourceEditor;
}

class ResourceFrame extends ScriptWidget {

    tabcontainer: Atomic.UITabContainer;
    resourceLayout: Atomic.UILayout;
    resourceViewContainer: Atomic.UILayout;
    currentResourceEditor: Editor.ResourceEditor;
    wasClosed: boolean;
    resourceEditorProvider: ResourceEditorProvider;

    // editors have a rootCotentWidget which is what is a child of the tab container

    // editors can be looked up by the full path of what they are editing
    editors: { [path: string]: Editor.ResourceEditor; } = {};

    show(value: boolean) {

        if (value) {

        }

    }

    handleSaveResource(ev: EditorEvents.SaveResourceEvent) {

        if (this.currentResourceEditor) {
            this.currentResourceEditor.save();
            // Grab the path to this file and pass it to the save resource
            this.sendEvent(EditorEvents.SaveResourceNotification, {
                path: ev.path || this.currentResourceEditor.fullPath
            });
        }

    }

    handleDeleteResource(ev: EditorEvents.DeleteResourceEvent) {
        var editor = this.editors[ev.path];
        if (editor) {
            editor.close(true);
            delete this.editors[ev.path];
        }
    }

    handleSaveAllResources(data) {

        for (var i in this.editors) {
            this.editors[i].save();
            this.sendEvent(EditorEvents.SaveResourceNotification, {
                path: this.editors[i].fullPath
            });
        }

    }

    handleEditResource(ev: EditorEvents.EditResourceEvent) {

        var path = ev.path;

        if (this.editors[path]) {

            this.navigateToResource(path);

            return;

        }

        var editor = this.resourceEditorProvider.getEditor(ev.path, this.tabcontainer);
        if (editor) {

            // cast and add editor lookup on widget itself
            (<EditorRootContentWidget> editor.rootContentWidget).editor = editor;

            this.editors[path] = editor;
            this.tabcontainer.currentPage = this.tabcontainer.numPages - 1;
            editor.setFocus();
        }


    }

    navigateToResource(fullpath: string, lineNumber = -1, tokenPos: number = -1) {
        if (this.wasClosed) return;

        if (!this.editors[fullpath]) {
            return;
        }

        var editor = this.editors[fullpath];

        if (this.currentResourceEditor == editor) return;

        var root = this.tabcontainer.contentRoot;

        var i = 0;

        for (var child = root.firstChild; child; child = child.next, i++) {
            if (editor.rootContentWidget == child) {
                break;
            }
        }

        if (i < this.tabcontainer.numPages) {

            this.tabcontainer.currentPage = i;

            editor.setFocus();

            // this cast could be better
            var ext = Atomic.getExtension(fullpath);

            if (ext == ".js" && lineNumber != -1) {
                (<Editor.JSResourceEditor>editor).gotoLineNumber(lineNumber);
            }
            else if (ext == ".js" && tokenPos != -1) {
                (<Editor.JSResourceEditor>editor).gotoTokenPos(tokenPos);
            }

        }

    }

    handleCloseResource(ev: EditorEvents.EditorCloseResourceEvent) {
        this.wasClosed = false;
        var editor = ev.editor;
        var navigate = ev.navigateToAvailableResource;

        if (!editor)
            return;

        editor.unsubscribeFromAllEvents();

        (<EditorRootContentWidget> editor.rootContentWidget).editor = null;

        var editors = Object.keys(this.editors);

        var closedIndex = editors.indexOf(editor.fullPath);

        if (editor.typeName == "SceneEditor3D") {

            this.sendEvent(EditorEvents.ActiveSceneEditorChange, { sceneEditor: (<Editor.SceneEditor3D> null) });

        }


        // remove from lookup
        delete this.editors[editor.fullPath];

        var root = this.tabcontainer.contentRoot;

        root.removeChild(editor.rootContentWidget);

        if (editor != this.currentResourceEditor) {
            this.wasClosed = true;
            return;
        } else {
            this.currentResourceEditor = null;
            this.tabcontainer.currentPage = -1;
        }

        if (navigate) {
            var nextEditor = editors[closedIndex + 1];
            if (nextEditor) {
                this.navigateToResource(nextEditor);
            } else {
                this.navigateToResource(editors[closedIndex - 1]);
            }
        }

    }

    handleResourceEditorChanged(data) {

        var editor = <Editor.ResourceEditor> data.editor;
        this.currentResourceEditor = editor;

    }

    handleRenameResource(ev:EditorEvents.RenameResourceEvent) {
        var editor = this.editors[ev.path];
        if (editor) {
            this.editors[ev.newPath] = editor;
            delete this.editors[ev.path];
        }
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_TAB_CHANGED && ev.target == this.tabcontainer) {

            var w = <EditorRootContentWidget> this.tabcontainer.currentPageWidget;

            if (w && w.editor) {

                if (this.currentResourceEditor != w.editor) {

                    if (w.editor.typeName == "SceneEditor3D") {

                        this.sendEvent(EditorEvents.ActiveSceneEditorChange, { sceneEditor: (<Editor.SceneEditor3D> w.editor) });

                    }

                    this.sendEvent(UIEvents.ResourceEditorChanged, { editor: w.editor });

                }

            }

        }

        if (ev.type == Atomic.UI_EVENT_TYPE_POINTER_UP) {
            this.wasClosed = false;
        }

        // bubble
        return false;

    }

    shutdown() {

        // on exit close all open editors
        for (var path in this.editors) {

            this.sendEvent(EditorEvents.EditorResourceClose, { editor: this.editors[path], navigateToAvailableResource: false });

        }

    }

    handleProjectUnloaded(data) {

      for (var i in this.editors) {
          var editor = this.editors[i];
           this.sendEvent(EditorEvents.EditorResourceClose, { editor: editor, navigateToAvailableResource: false });
           editor.close();
      }

    }

    constructor(parent: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/resourceframe.tb.txt");

        this.gravity = Atomic.UI_GRAVITY_ALL;

        this.resourceViewContainer = <Atomic.UILayout> parent.getWidget("resourceviewcontainer");
        this.tabcontainer = <Atomic.UITabContainer> this.getWidget("tabcontainer");
        this.resourceLayout = <Atomic.UILayout> this.getWidget("resourcelayout");

        this.resourceViewContainer.addChild(this);
        this.resourceEditorProvider = new ResourceEditorProvider(this);
        this.resourceEditorProvider.loadStandardEditors();

        this.subscribeToEvent(EditorEvents.ProjectUnloadedNotification, (data) => this.handleProjectUnloaded(data));
        this.subscribeToEvent(EditorEvents.EditResource, (data) => this.handleEditResource(data));
        this.subscribeToEvent(EditorEvents.SaveResource, (data) => this.handleSaveResource(data));
        this.subscribeToEvent(EditorEvents.SaveAllResources, (data) => this.handleSaveAllResources(data));
        this.subscribeToEvent(EditorEvents.EditorResourceClose, (ev: EditorEvents.EditorCloseResourceEvent) => this.handleCloseResource(ev));
        this.subscribeToEvent(EditorEvents.RenameResourceNotification, (ev: EditorEvents.RenameResourceEvent) => this.handleRenameResource(ev));
        this.subscribeToEvent(EditorEvents.DeleteResourceNotification, (data) => this.handleDeleteResource(data));

        this.subscribeToEvent(UIEvents.ResourceEditorChanged, (data) => this.handleResourceEditorChanged(data));

        this.subscribeToEvent("WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

}

export = ResourceFrame;
