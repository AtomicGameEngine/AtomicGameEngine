
import ScriptWidget = require("../ScriptWidget");
import UIEvents = require("../UIEvents");

import TextureSelector = require("./TextureSelector");

var techniqueSource = new Atomic.UIMenuItemSource();

var solidSource = new Atomic.UIMenuItemSource();
solidSource.addItem(new Atomic.UIMenuItem("Diffuse", "Diffuse"));
solidSource.addItem(new Atomic.UIMenuItem("Diffuse Emissive", "Diffuse Emissive"));
solidSource.addItem(new Atomic.UIMenuItem("Diffuse Normal", "Diffuse Normal"));
solidSource.addItem(new Atomic.UIMenuItem("Diffuse Normal Specular", "Diffuse Normal Specular"));

var tranSource = new Atomic.UIMenuItemSource();
tranSource.addItem(new Atomic.UIMenuItem("Alpha", "Alpha"));
tranSource.addItem(new Atomic.UIMenuItem("Alpha Mask", "Alpha Mask"));
tranSource.addItem(new Atomic.UIMenuItem("Additive", "Additive"));
tranSource.addItem(new Atomic.UIMenuItem("Additive Alpha", "Additive Alpha"));
tranSource.addItem(new Atomic.UIMenuItem("Emissive Alpha", "Emissive Alpha"));
tranSource.addItem(new Atomic.UIMenuItem("Alpha AO", "Alpha AO"));
tranSource.addItem(new Atomic.UIMenuItem("Alpha Mask AO", "Alpha Mask AO"));

var lightmapSource = new Atomic.UIMenuItemSource();
lightmapSource.addItem(new Atomic.UIMenuItem("Lightmap", "Lightmap"));
lightmapSource.addItem(new Atomic.UIMenuItem("Lightmap Alpha", "Lightmap Alpha"));

var _ = new Atomic.UIMenuItem("Solid");
_.subSource = solidSource;
techniqueSource.addItem(_);

_ = new Atomic.UIMenuItem("Transparency");
_.subSource = tranSource;
techniqueSource.addItem(_);

_ = new Atomic.UIMenuItem("Lightmap");
_.subSource = lightmapSource;
techniqueSource.addItem(_);

var techniqueLookup = {
    "Techniques/Diff.xml": "Diffuse",
    "Techniques/DiffEmissive.xml": "Diffuse Emissive",
    "Techniques/DiffNormal.xml": "Diffuse Normal",
    "Techniques/DiffNormalSpec.xml": "Diffuse Normal Specular"
}

var techniqueReverseLookup = {};

for (var key in techniqueLookup) {

  techniqueReverseLookup[techniqueLookup[key]] = key;

}


class MaterialInspector extends ScriptWidget {

    constructor() {

        super();

        this.fd.id = "Vera";
        this.fd.size = 11;


    }

    createShaderParametersSection(): Atomic.UISection {

        var section = new Atomic.UISection();
        section.text = "Shader Paramaters";
        section.value = 1;
        section.fontDescription = this.fd;

        var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        section.contentRoot.addChild(attrsVerticalLayout);

        var params = this.material.getShaderParameters();

        for (var i in params) {


          var attrLayout = new Atomic.UILayout();
          attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

          var name = new Atomic.UITextField();
          name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
          name.skinBg = "InspectorTextAttrName";

          name.text = params[i].name;
          name.fontDescription = this.fd;

          attrLayout.addChild(name);

          var field = new Atomic.UIEditField();
          field.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
          field.skinBg = "TBAttrEditorField";;
          field.fontDescription = this.fd;
          var lp = new Atomic.UILayoutParams();
          lp.width = 140;
          field.layoutParams = lp;

          field.text = params[i].value;

          attrLayout.addChild(field);

          attrsVerticalLayout.addChild(attrLayout);

          // print(params[i].name, " : ", params[i].value, " : ", params[i].type);

        }


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

    onTechniqueSet(techniqueName:string) {

      this.techniqueButton.text = techniqueName;

      var cache = Atomic.getResourceCache();
      var technique = <Atomic.Technique> cache.getResource("Technique", techniqueReverseLookup[techniqueName]);
      this.material.setTechnique(0, technique);

    }

    createTechniquePopup(): Atomic.UIWidget {

        var button = this.techniqueButton = new Atomic.UIButton();
        var technique = this.material.getTechnique(0);

        button.text = techniqueLookup[technique.name];

        button.fontDescription = this.fd;

        var lp = new Atomic.UILayoutParams();
        lp.width = 180;
        button.layoutParams = lp;

        button.onClick = function() {

            var menu = new Atomic.UIMenuWindow(button, "technique popup");

            menu.fontDescription = this.fd;
            menu.show(techniqueSource);

            button.subscribeToEvent(button, "WidgetEvent", function(ev: Atomic.UIWidgetEvent) {

                if (ev.type != Atomic.UI_EVENT_TYPE_CLICK)
                    return;

                if (ev.target && ev.target.id == "technique popup") {

                    this.onTechniqueSet(ev.refid);

                }

            }.bind(this));

        }.bind(this);

        return button;

    }

    createTextureSection(): Atomic.UISection {

        var section = new Atomic.UISection();
        section.text = "Textures";
        section.value = 1;
        section.fontDescription = this.fd;

        var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        section.contentRoot.addChild(attrsVerticalLayout);

        // TODO: Filter on technique
        var textureUnits = [Atomic.TU_DIFFUSE, Atomic.TU_NORMAL, Atomic.TU_SPECULAR];// ,Atomic.TU_EMISSIVE, Atomic.TU_ENVIRONMENT,
            //Atomic.TU_CUSTOM1, Atomic.TU_CUSTOM2];

        for (var i in textureUnits) {

            var tunit: Atomic.TextureUnit = textureUnits[i];

            var tunitName = Atomic.Material.getTextureUnitName(tunit);

            var attrLayout = new Atomic.UILayout();
            attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

            var name = new Atomic.UITextField();
            name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
            name.skinBg = "InspectorTextAttrName";

            name.text = tunitName;
            name.fontDescription = this.fd;

            attrLayout.addChild(name);

            var textureWidget = new Atomic.UITextureWidget();
            textureWidget.texture = this.getTextureThumbnail(this.material.getTexture(tunit));

            var tlp = new Atomic.UILayoutParams();
            tlp.width = 64;
            tlp.height = 64;
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

        var mlp = new Atomic.UILayoutParams();
        mlp.width = 340;

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
        materialSection.fontDescription = this.fd;
        materialLayout.addChild(materialSection);

        var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_PREFERRED;

        // NAME
        var nameLayout = new Atomic.UILayout();
        nameLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var name = new Atomic.UITextField();
        name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        name.skinBg = "InspectorTextAttrName";

        name.text = "Name";
        name.fontDescription = this.fd;

        nameLayout.addChild(name);

        var field = new Atomic.UIEditField();
        field.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        field.skinBg = "TBAttrEditorField";;
        field.fontDescription = this.fd;
        var lp = new Atomic.UILayoutParams();
        lp.width = 180;
        field.layoutParams = lp;

        field.text = material.getTexture(Atomic.TU_DIFFUSE).name;

        nameLayout.addChild(field);

        attrsVerticalLayout.addChild(nameLayout);

        // TECHNIQUE LAYOUT

        var techniqueLayout = new Atomic.UILayout();
        techniqueLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_GRAVITY;
        techniqueLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_PREFERRED;

        name = new Atomic.UITextField();
        name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        name.skinBg = "InspectorTextAttrName";

        name.text = "Technique";
        name.fontDescription = this.fd;

        techniqueLayout.addChild(name);

        var techniquePopup = this.createTechniquePopup();

        techniqueLayout.addChild(techniquePopup);

        attrsVerticalLayout.addChild(techniqueLayout);

        materialSection.contentRoot.addChild(attrsVerticalLayout);

        materialLayout.addChild(this.createTextureSection());
        materialLayout.addChild(this.createShaderParametersSection());

        this.addChild(materialLayout);

    }

    techniqueButton:Atomic.UIButton;
    material: Atomic.Material;
    nameTextField: Atomic.UITextField;
    fd: Atomic.UIFontDescription = new Atomic.UIFontDescription();


}

export = MaterialInspector;
