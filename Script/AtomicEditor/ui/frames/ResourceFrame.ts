//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import ScriptWidget = require("ui/ScriptWidget");
import EditorEvents = require("editor/EditorEvents");
import UIEvents = require("ui/UIEvents");

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

    // editors have a rootCotentWidget which is what is a child of the tab container

    // editors can be looked up by the full path of what they are editing
    editors: { [path: string]: Editor.ResourceEditor; } = {};

    show(value: boolean) {

        if (value) {

        }

    }

    handleSaveResource(ev: EditorEvents.SaveResourceEvent) {

        if (this.currentResourceEditor)
            this.currentResourceEditor.save();

    }

    handleSaveAllResources(data) {

        for (var i in this.editors) {
            this.editors[i].save();
        }

    }

    handleEditResource(ev: EditorEvents.EditResourceEvent) {

        var path = ev.path;

        if (this.editors[path]) {

            this.navigateToResource(path);

            return;

        }

        var ext = Atomic.getExtension(path);

        var editor: Editor.ResourceEditor = null;

        if (ext == ".js" || ext == ".txt") {

            editor = new Editor.JSResourceEditor(path, this.tabcontainer);

        } else if (ext == ".scene") {

            var sceneEditor3D = new Editor.SceneEditor3D(path, this.tabcontainer);
            editor = sceneEditor3D;

            this.sendEvent(EditorEvents.ActiveSceneChange, { scene: sceneEditor3D.scene });

        }

        if (editor) {

            // cast and add editor lookup on widget itself
            (<EditorRootContentWidget> editor.rootContentWidget).editor = editor;

            this.editors[path] = editor;
            this.tabcontainer.currentPage = this.tabcontainer.numPages - 1;
            editor.setFocus();
        }


    }

    navigateToResource(fullpath: string, lineNumber = -1, tokenPos: number = -1) {

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

    handleCloseResource(ev: EditorEvents.CloseResourceEvent) {

        var editor = ev.editor;
        var navigate = ev.navigateToAvailableResource;

        if (!editor)
            return;

        if (this.currentResourceEditor == editor)
            this.currentResourceEditor = null;

        editor.unsubscribeFromAllEvents();

        // remove from lookup
        delete this.editors[editor.fullPath];

        var root = this.tabcontainer.contentRoot;

        root.removeChild(editor.rootContentWidget);

        this.tabcontainer.currentPage = -1;

        if (navigate) {

            var keys = Object.keys(this.editors);

            if (keys.length) {

                this.navigateToResource(keys[keys.length - 1]);

            }

        }

    }

    handleResourceEditorChanged(data) {

        var editor = <Editor.ResourceEditor> data.editor;
        this.currentResourceEditor = editor;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_TAB_CHANGED && ev.target == this.tabcontainer) {
            var w = <EditorRootContentWidget> this.tabcontainer.currentPageWidget;

            if (w && w.editor) {

                if (this.currentResourceEditor != w.editor) {

                    if (w.editor.typeName == "SceneEditor3D") {

                        this.sendEvent(EditorEvents.ActiveSceneChange, { scene: (<Editor.SceneEditor3D> w.editor).scene });

                    }

                    this.sendEvent(UIEvents.ResourceEditorChanged, { editor: w.editor });

                }

            }

        }

        // bubble
        return false;

    }

    shutdown() {

        // on exit close all open editors
        for (var path in this.editors) {

            this.sendEvent(EditorEvents.CloseResource, { editor: this.editors[path], navigateToAvailableResource: false });

        }

    }

    handleProjectUnloaded(data) {

      for (var i in this.editors) {
          this.editors[i].close();
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

        this.subscribeToEvent("ProjectUnloaded", (data) => this.handleProjectUnloaded(data));
        this.subscribeToEvent(EditorEvents.EditResource, (data) => this.handleEditResource(data));
        this.subscribeToEvent(EditorEvents.SaveResource, (data) => this.handleSaveResource(data));
        this.subscribeToEvent(EditorEvents.SaveAllResources, (data) => this.handleSaveAllResources(data));
        this.subscribeToEvent(EditorEvents.CloseResource, (ev: EditorEvents.CloseResourceEvent) => this.handleCloseResource(ev));

        this.subscribeToEvent(UIEvents.ResourceEditorChanged, (data) => this.handleResourceEditorChanged(data));


        this.subscribeToEvent("WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

}

export = ResourceFrame;
