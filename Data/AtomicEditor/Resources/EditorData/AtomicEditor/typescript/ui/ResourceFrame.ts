import ScriptWidget = require("./ScriptWidget");
import UIEvents = require("./UIEvents");

var UI = Atomic.UI;

class ResourceFrame extends ScriptWidget {

    tabcontainer:Atomic.UITabContainer;
    resourceLayout:Atomic.UILayout;
    resourceViewContainer:Atomic.UILayout;

    editors: { [path: string] : Editor.ResourceEditor; } = {};

    show(value:boolean) {

      if (show) {

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

          //tabcontainer_->SetCurrentPage(tabcontainer_->GetNumPages()-1);
          //editor->SetFocus();
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

    }

}

export = ResourceFrame;
