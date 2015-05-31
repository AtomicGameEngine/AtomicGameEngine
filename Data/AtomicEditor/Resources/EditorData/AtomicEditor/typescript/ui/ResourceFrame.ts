import ScriptWidget = require("./ScriptWidget");
import UIEvents = require("./UIEvents");

var UI = Atomic.UI;

class ResourceFrame extends ScriptWidget {

    tabcontainer:Atomic.UITabContainer;
    resourceLayout:Atomic.UILayout;
    resourceViewContainer:Atomic.UILayout;

    editors: { [path: string] : Editor.ResourceEditor; } = {};

    show(value:boolean) {

      if (value) {

      }

    }

    handleEditResource(data) {

      var path = data.path;

      if (this.editors[path]) {

        return;

      }

      var ext = Atomic.getExtension(data.path);

      var editor:Editor.ResourceEditor = null;

      if (ext == ".js")
      {
          editor = new Editor.JSResourceEditor(path, this.tabcontainer);
      }

      if (editor)
      {
          this.editors[path] = editor;
          this.tabcontainer.currentPage = this.tabcontainer.numPages - 1;
          editor.setFocus();
      }


    }

    handleCloseResourceEditor(data) {

      var editor = <Editor.ResourceEditor> data.editor;
      var navigate = <boolean> data.navigateToAvailableResource;

      // remove from lookup
      this.editors[editor.fullPath] = undefined;

      var root = this.tabcontainer.contentRoot;

      var found = false;

      var i = 0;
      for (var child = root.firstChild; child; child = child.next, i++)
      {
            if (child == editor.rootContentWidget)
            {
                found = true;
                root.removeChild(child);
                break;
            }

      }

      this.tabcontainer.currentPage = -1;

      if (navigate) {

        print("NAVIGATE!");

      }


    }

    constructor(parent:Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/resourceframe.tb.txt");

        this.gravity = UI.GRAVITY_ALL;

        this.resourceViewContainer = <Atomic.UILayout> parent.getWidget("resourceviewcontainer");
        this.tabcontainer = <Atomic.UITabContainer> this.getWidget("tabcontainer");
        this.resourceLayout = <Atomic.UILayout> this.getWidget("resourcelayout");

        this.resourceViewContainer.addChild(this);

        this.subscribeToEvent(UIEvents.EditResource, (data) => this.handleEditResource(data));
        this.subscribeToEvent(UIEvents.CloseResourceEditor, (data) => this.handleCloseResourceEditor(data));

    }

}

export = ResourceFrame;
