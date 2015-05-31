import ScriptWidget = require("./ScriptWidget");
import UIEvents = require("./UIEvents");

var UI = Atomic.UI;

// the root content of editor widgets (rootContentWidget property) are extended with an editor field
// so we can access the editor they belong to from the widget itself
interface EditorRootContentWidget extends Atomic.UIWidget{
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

    handleEditResource(data) {

        var path = data.path;

        if (this.editors[path]) {

            return;

        }

        var ext = Atomic.getExtension(data.path);

        var editor: Editor.ResourceEditor = null;

        if (ext == ".js") {

           editor = new Editor.JSResourceEditor(path, this.tabcontainer);

        } else if (ext == ".scene") {

           editor = new Editor.SceneEditor3D(path, this.tabcontainer);

        }

        if (editor) {

            // add __editor which lets us lookup the editor via the rootContentWidget
            // could this be formalized with an interface?
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

    handleCloseResourceEditor(data) {

        var editor = <Editor.ResourceEditor> data.editor;
        var navigate = <boolean> data.navigateToAvailableResource;

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

    handleWidgetEvent(data) {

      // ok, first thing is to fix up this widget <-> editor mess

      if (data.type == Atomic.UI.EVENT_TYPE_TAB_CHANGED && data.target == this.tabcontainer)
      {
        var w = <EditorRootContentWidget> this.tabcontainer.currentPageWidget;

        if (w && w.editor) {

          if (this.currentResourceEditor != w.editor) {

            this.sendEvent(UIEvents.ResourceEditorChanged, { editor: w.editor});

          }

        }


      }

      // bubble
      return false;

    }

    constructor(parent: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/resourceframe.tb.txt");

        this.gravity = UI.GRAVITY_ALL;

        this.resourceViewContainer = <Atomic.UILayout> parent.getWidget("resourceviewcontainer");
        this.tabcontainer = <Atomic.UITabContainer> this.getWidget("tabcontainer");
        this.resourceLayout = <Atomic.UILayout> this.getWidget("resourcelayout");

        this.resourceViewContainer.addChild(this);

        this.subscribeToEvent(UIEvents.EditResource, (data) => this.handleEditResource(data));
        this.subscribeToEvent(UIEvents.CloseResourceEditor, (data) => this.handleCloseResourceEditor(data));
        this.subscribeToEvent(UIEvents.ResourceEditorChanged, (data) => this.handleResourceEditorChanged(data));

        this.subscribeToEvent("WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

}

export = ResourceFrame;
