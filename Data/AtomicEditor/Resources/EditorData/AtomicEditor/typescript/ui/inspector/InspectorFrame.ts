
import UIEvents = require("../UIEvents");
import ScriptWidget = require("../ScriptWidget");
import DataBinding = require("./DataBinding");

// inspectors

import MaterialInspector = require("./MaterialInspector");


var UI = Atomic.UI;

class InspectorFrame extends ScriptWidget {

  constructor() {

    super();

    this.gravity = UI.GRAVITY_TOP_BOTTOM;

    this.load("AtomicEditor/editor/ui/inspectorframe.tb.txt");

    var container = this.getWidget("inspectorcontainer");

    this.materialInspector = new MaterialInspector();
    container.addChild(this.materialInspector);

    this.subscribeToEvent(UIEvents.EditResource, (data) => this.handleEditResource(data));

  }

  handleEditResource(ev: UIEvents.EditorResourceEvent) {

      var path = ev.path;

      var db = ToolCore.getAssetDatabase();
      var asset = db.getAssetByPath(path);
      if (asset) {

        this.inspect(asset);

      }

  }


  inspect(asset:ToolCore.Asset) {

    if (asset.importerName == "MaterialImporter") {

      this.materialInspector.inspect(asset);

    }
  }

  materialInspector:MaterialInspector;

}

export = InspectorFrame;
