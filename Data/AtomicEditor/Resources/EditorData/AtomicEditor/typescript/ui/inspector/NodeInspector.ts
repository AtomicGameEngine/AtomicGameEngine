
import ScriptWidget = require("../ScriptWidget");

var UI = Atomic.UI;

class NodeInspector extends ScriptWidget {

  constructor() {

    super();

  }

  inspect(node:Atomic.Node) {

    var attrs = node.getAttributes();

    for (var i in attrs) {
        var attr = attrs[i];
        print(attr);
    }

    var fd = new Atomic.UIFontDescription();
    fd.id = "Vera";
    fd.size = 11;

    var nlp = new Atomic.UILayoutParams();
    nlp.width = 304;

    var nodeLayout = new Atomic.UILayout();
    nodeLayout.spacing = 4;

    nodeLayout.layoutDistribution  = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
    nodeLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
    nodeLayout.layoutParams = nlp;

    var nodeContainer = new Atomic.UIContainer();
    nodeContainer.gravity = Atomic.UI_GRAVITY_ALL;
    nodeContainer.skinBg = "InspectorTopLayout";

    nodeLayout.addChild(nodeContainer);

    this.addChild(nodeLayout);

  }


}

export = NodeInspector;
