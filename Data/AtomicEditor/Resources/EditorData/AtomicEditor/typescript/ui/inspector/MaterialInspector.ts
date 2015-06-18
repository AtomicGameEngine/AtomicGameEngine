
import ScriptWidget = require("../ScriptWidget");

var UI = Atomic.UI;

class MaterialInspector extends ScriptWidget {

  constructor() {

    super();

    var text = this.nameTextField = new Atomic.UITextField();
    text.gravity = UI.GRAVITY_ALL;

    //this.gravity = UI.GRAVITY_ALL;

    this.addChild(text)

  }

  inspect(asset:ToolCore.Asset) {

    //this.material = mat;

    this.nameTextField.text = asset.name;

  }

  material:Atomic.Material;

  nameTextField:Atomic.UITextField;


}

export = MaterialInspector;
