
import ScriptWidget = require("../ScriptWidget");
import UIEvents = require("../UIEvents");

import TextureSelector = require("./TextureSelector");

class MaterialInspector extends ScriptWidget {

    constructor() {

        super();

    }

    createShaderParametersSection(): Atomic.UISection {

        var section = new Atomic.UISection();
        section.text = "Shader Paramaters";
        section.value = 1;

        return section;

    }

    getTextureThumbnail(texture: Atomic.Texture): Atomic.Texture {

        if (!texture) return null;

        var db = ToolCore.getAssetDatabase();
        var asset = db.getAssetByPath(texture.name);

        if (!asset)
            return texture;

        var thumbnail = asset.cachePath + "_thumbnail.png";
        var cache = Atomic.getResourceCache();

        var thumb = <Atomic.Texture2D> cache.getTempResource("Texture2D", thumbnail);


        if (thumb)
          return thumb;

        return texture;

    }

    createTextureSection(): Atomic.UISection {

        var section = new Atomic.UISection();
        section.text = "Textures";
        section.value = 1;

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        section.contentRoot.addChild(attrsVerticalLayout);

        // TODO: Filter on technique
        var textureUnits = [Atomic.TU_DIFFUSE, Atomic.TU_NORMAL, Atomic.TU_EMISSIVE, Atomic.TU_SPECULAR, Atomic.TU_ENVIRONMENT,
            Atomic.TU_CUSTOM1, Atomic.TU_CUSTOM2];

        for (var i in textureUnits) {

            var tunit: Atomic.TextureUnit = textureUnits[i];

            var tunitName = Atomic.Material.getTextureUnitName(tunit);

            var attrLayout = new Atomic.UILayout();
            attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

            var name = new Atomic.UITextField();
            name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
            name.skinBg = "InspectorTextAttrName";

            name.text = tunitName;
            name.fontDescription = fd;

            attrLayout.addChild(name);

            var textureWidget = new Atomic.UITextureWidget();
            textureWidget.texture = this.getTextureThumbnail(this.material.getTexture(tunit));

            var tlp = new Atomic.UILayoutParams();
            tlp.width = 32;
            tlp.height = 32;
            textureWidget.layoutParams = tlp;

            attrLayout.addChild(textureWidget);

            attrsVerticalLayout.addChild(attrLayout);

            var editInfo: { textureUnit: Atomic.TextureUnit } = { textureUnit: tunit };

            var callback = function(ev: Atomic.UIWidgetEvent) {

                if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {
                  var tselect = new TextureSelector(ev.target.view);
              }

            }.bind(editInfo);

            textureWidget.subscribeToEvent(textureWidget, "WidgetEvent", callback);

        }

        return section;

    }

    inspect(material: Atomic.Material) {

        this.material = material;

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var mlp = new Atomic.UILayoutParams();
        mlp.width = 304;

        var materialLayout = new Atomic.UILayout();
        materialLayout.spacing = 4;

        materialLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        materialLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        materialLayout.layoutParams = mlp;
        materialLayout.axis = Atomic.UI_AXIS_Y;

        // node attr layout

        var materialSection = new Atomic.UISection();
        materialSection.text = "Material";
        materialSection.value = 1;
        materialLayout.addChild(materialSection);

        var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        var attrLayout = new Atomic.UILayout();
        attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var name = new Atomic.UITextField();
        name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        name.skinBg = "InspectorTextAttrName";

        name.text = "Name";
        name.fontDescription = fd;

        attrLayout.addChild(name);


        var field = new Atomic.UIEditField();
        field.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        field.skinBg = "TBAttrEditorField";;
        field.fontDescription = fd;
        var lp = new Atomic.UILayoutParams();
        lp.width = 140;
        field.layoutParams = lp;

        field.text = material.getTexture(Atomic.TU_DIFFUSE).name;

        attrLayout.addChild(field);


        attrsVerticalLayout.addChild(attrLayout);


        materialSection.contentRoot.addChild(attrsVerticalLayout);


        /*

        // TODO: support quality level and lod for techniques
        var technique = material.getTechnique(0);

        this.nameTextField.text = technique.name;

        var params = material.getShaderParameters();

        for (var i in params) {

          print(params[i].name, " : ", params[i].value, " : ", params[i].type);

        }
        */

        materialLayout.addChild(this.createTextureSection());
        materialLayout.addChild(this.createShaderParametersSection());

        this.addChild(materialLayout);

    }

    material: Atomic.Material;

    nameTextField: Atomic.UITextField;


}

export = MaterialInspector;
