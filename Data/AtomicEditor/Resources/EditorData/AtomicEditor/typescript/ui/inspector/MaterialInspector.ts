
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

    var cache = Atomic.getResourceCache();

    var material = <Atomic.Material> cache.getResource("Material", asset.path);

    if (!material)
      return;

    // TODO: support quality level and lod for techniques  
    var technique = material.getTechnique(0);

    this.nameTextField.text = technique.name;

    var params = material.getShaderParameters();

    for (var i in params) {

      print(params[i].name, " : ", params[i].value, " : ", params[i].type);

    }

  }

  material:Atomic.Material;

  nameTextField:Atomic.UITextField;


}

export = MaterialInspector;
