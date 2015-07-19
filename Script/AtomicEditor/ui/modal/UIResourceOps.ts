import EditorEvents = require("../../editor/EditorEvents");
import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");
import ResourceOps = require("../../resources/ResourceOps");

export class ResourceDelete extends ModalWindow {

  constructor(asset: ToolCore.Asset) {

      super();

      this.asset = asset;
      this.init("Delete Resource", "AtomicEditor/editor/ui/resourcedelete.tb.txt");
      var message = <Atomic.UIEditField> this.getWidget("message");

      var text = "Are you sure you want to delete resource:\n\n";
      text += asset.path;
      text += "\n\nThis operation cannot be undone";

      message.text = text;

      this.resizeToFitContent();
      this.center();

  }

  handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

      if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

          var id = ev.target.id;

          if (id == "delete") {

              this.hide();

              var db = ToolCore.getAssetDatabase();
              db.deleteAsset(this.asset);

              return true;
          }

          if (id == "cancel") {

              this.hide();

              return true;
          }

      }

  }

  asset: ToolCore.Asset;

}

export class CreateFolder extends ModalWindow {

    constructor(resourcePath: string) {

        super();

        this.resourcePath = resourcePath;
        this.init("New Folder", "AtomicEditor/editor/ui/resourcenewfolder.tb.txt");
        this.nameField = <Atomic.UIEditField> this.getWidget("folder_name");
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "create") {

                var resourcePath = Atomic.addTrailingSlash(this.resourcePath) + this.nameField.text;

                if (ResourceOps.CreateNewFolder(resourcePath)) {

                    this.hide();

                }

                return true;

            }

            if (id == "cancel") {

                this.hide();

                return true;
            }

        }

    }

    resourcePath: string;
    nameField: Atomic.UIEditField;

}
