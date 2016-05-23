//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

import ScriptWidget = require("ui/ScriptWidget");
import UIEvents = require("ui/UIEvents");
import EditorUI = require("ui/EditorUI");
import EditorEvents = require("editor/EditorEvents");

import TextureSelector = require("./TextureSelector");

var techniqueSource = new Atomic.UIMenuItemSource();

var solidSource = new Atomic.UIMenuItemSource();
solidSource.addItem(new Atomic.UIMenuItem("Diffuse", "Diffuse"));
solidSource.addItem(new Atomic.UIMenuItem("Diffuse Emissive", "Diffuse Emissive"));
solidSource.addItem(new Atomic.UIMenuItem("Diffuse Normal", "Diffuse Normal"));
solidSource.addItem(new Atomic.UIMenuItem("Diffuse Specular", "Diffuse Specular"));
solidSource.addItem(new Atomic.UIMenuItem("Diffuse Normal Specular", "Diffuse Normal Specular"));
solidSource.addItem(new Atomic.UIMenuItem("Diffuse Unlit", "Diffuse Unlit"));
solidSource.addItem(new Atomic.UIMenuItem("No Texture", "No Texture"));

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

var projectSource = new Atomic.UIMenuItemSource();
var _ = new Atomic.UIMenuItem();

var techniqueLookup = {
    "Techniques/Diff.xml": "Diffuse",
    "Techniques/DiffEmissive.xml": "Diffuse Emissive",
    "Techniques/DiffNormal.xml": "Diffuse Normal",
    "Techniques/DiffSpec.xml": "Diffuse Specular",
    "Techniques/DiffNormalSpec.xml": "Diffuse Normal Specular",
    "Techniques/DiffUnlit.xml": "Diffuse Unlit",
    "Techniques/DiffAlpha.xml": "Alpha",
    "Techniques/DiffAlphaMask.xml": "Alpha Mask",
    "Techniques/DiffAdd.xml": "Additive",
    "Techniques/NoTexture.xml": "No Texture",
    "Techniques/DiffLightMap.xml": "Lightmap",
    "Techniques/DiffLightMapAlpha.xml": "Lightmap Alpha"
};

var techniqueReverseLookup = {};
var projectTechniques = {};
var projectTechniquesAddress = {};

for (var key in techniqueLookup) {

    techniqueReverseLookup[techniqueLookup[key]] = key;

}

class MaterialInspector extends ScriptWidget {

    currentTexture: Atomic.UITextureWidget = null;
    tunit: number;
    textureWidget: Atomic.UITextureWidget;

    constructor() {

        super();

        this.fd.id = "Vera";
        this.fd.size = 11;

        this.subscribeToEvent(EditorEvents.RemoveCurrentAssetAssigned, (ev: EditorEvents.RemoveCurrentAssetAssignedEvent) => this.createTextureRemoveButtonCallback(this.tunit, this.textureWidget));
        this.subscribeToEvent("ResourceAdded", (ev: ToolCore.ResourceAddedEvent) => this.refreshTechniquesPopup());
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
            field.skinBg = "TBAttrEditorField";
            field.fontDescription = this.fd;
            var lp = new Atomic.UILayoutParams();
            lp.width = 140;
            field.layoutParams = lp;

            field.id = params[i].name;
            field.text = params[i].valueString;

            field.subscribeToEvent(field, "WidgetEvent", function (ev: Atomic.UIWidgetEvent) {

                if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {

                    var field = <Atomic.UIEditField>ev.target;
                    this.material.setShaderParameter(field.id, field.text);

                }

            }.bind(this));

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

        var thumb = <Atomic.Texture2D>cache.getTempResource("Texture2D", thumbnail);

        if (thumb)
            return thumb;

        return texture;

    }

    onTechniqueSet(techniqueName: string) {

        this.techniqueButton.text = techniqueName;

        var cache = Atomic.getResourceCache();
        var technique = <Atomic.Technique>cache.getResource("Technique", techniqueReverseLookup[techniqueName]);
        var resourcePath = ToolCore.toolSystem.project.getResourcePath();

        if (technique == null) {
            var techniquePath = "";

            for (var i in projectTechniques) {
                if (techniqueName == projectTechniques[i]) {
                    techniquePath = projectTechniquesAddress[i];
                    break;
                }
            }
            techniquePath = techniquePath.replace(resourcePath, "");
            technique = <Atomic.Technique>cache.getResource("Technique", techniquePath);
        }
        this.material.setTechnique(0, technique);
    }

    createTechniquePopup(): Atomic.UIWidget {

        this.refreshTechniquesPopup();

        var button = this.techniqueButton = new Atomic.UIButton();
        var technique = this.material.getTechnique(0);
        var techniqueName = "";

        if (technique != null) {
            techniqueName = technique.name.replace("Techniques/", "").replace(".xml", "");
        } else {
            techniqueName = "UNDEFINED";
        }

        button.text = techniqueName;

        button.fontDescription = this.fd;

        var lp = new Atomic.UILayoutParams();
        lp.width = 140;
        button.layoutParams = lp;

        button.onClick = function () {

            var menu = new Atomic.UIMenuWindow(button, "technique popup");

            menu.fontDescription = this.fd;
            menu.show(techniqueSource);

            button.subscribeToEvent(button, "WidgetEvent", function (ev: Atomic.UIWidgetEvent) {

                if (ev.type != Atomic.UI_EVENT_TYPE_CLICK)
                    return;

                if (ev.target && ev.target.id == "technique popup") {

                    this.onTechniqueSet(ev.refid);

                }

            }.bind(this));

        }.bind(this);

        return button;

    }

    acceptAssetDrag(importerTypeName: string, ev: Atomic.DragEndedEvent): ToolCore.AssetImporter {

        var dragObject = ev.dragObject;

        if (dragObject.object && dragObject.object.typeName == "Asset") {

            var asset = <ToolCore.Asset>dragObject.object;

            if (asset.importerTypeName == importerTypeName) {
                return asset.importer;
            }

        }

        return null;

    }

    openTextureSelectionBox(textureUnit: number, textureWidget: Atomic.UITextureWidget) {

        var inspector = this;

        EditorUI.getModelOps().showResourceSelection("Select Texture", "TextureImporter", "Texture2D", function (asset: ToolCore.Asset, args: any) {

            var texture = <Atomic.Texture2D>Atomic.cache.getResource("Texture2D", asset.path);

            if (texture) {
                inspector.material.setTexture(textureUnit, texture);
                textureWidget.texture = inspector.getTextureThumbnail(texture);

                this.sendEvent(EditorEvents.InspectorProjectReference, { "path": texture.getName() });
            }

        });

    }

    // Big Texture Button(referenced texture file path in project frame)
    createTextureButtonCallback(textureUnit: number, textureWidget: Atomic.UITextureWidget) {

        return () => {

            var texture = this.material.getTexture(textureUnit);

            if (textureWidget.getTexture() != null) {
                this.sendEvent(EditorEvents.InspectorProjectReference, { "path": texture.getName() });
            } else {
                this.openTextureSelectionBox(textureUnit, textureWidget);
            }

            return true;

        };

    }

    // Small Texture Button (Opens texture selection window)
    createTextureReferenceButtonCallback(textureUnit: number, textureWidget: Atomic.UITextureWidget) {

        return () => {
            this.tunit = textureUnit;
            this.textureWidget = textureWidget;
            this.openTextureSelectionBox(textureUnit, textureWidget);
            return true;
        };
    }

    //Remove Texture Button
    createTextureRemoveButtonCallback(textureUnit: number, textureWidget: Atomic.UITextureWidget) {

        var texture = this.material.getTexture(textureUnit);

        if (texture != null && textureWidget != null) {
            textureWidget.setTexture(null);
        }

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
        var textureUnits = [Atomic.TU_DIFFUSE, Atomic.TU_NORMAL, Atomic.TU_SPECULAR, Atomic.TU_EMISSIVE];

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

            var tlp = new Atomic.UILayoutParams();
            tlp.width = 64;
            tlp.height = 64;
            textureWidget.layoutParams = tlp;
            textureWidget.texture = this.getTextureThumbnail(this.material.getTexture(tunit));

            var textureButton = new Atomic.UIButton();
            textureButton.skinBg = "TBButton.flatoutline";
            textureButton["tunit"] = tunit;
            textureButton["textureWidget"] = textureWidget;

            //Create drop-down buttons to open Texture Selection Dialog Box
            var textureRefButton = new Atomic.UIButton();
            textureRefButton.skinBg = "arrow.down";
            textureRefButton["tunit"] = tunit;
            textureRefButton["textureWidget"] = textureWidget;

            textureButton.onClick = this.createTextureButtonCallback(tunit, textureWidget);
            textureRefButton.onClick = this.createTextureReferenceButtonCallback(tunit, textureWidget);

            textureButton.contentRoot.addChild(textureWidget);

            attrLayout.addChild(textureButton);
            attrLayout.addChild(textureRefButton);

            attrsVerticalLayout.addChild(attrLayout);

            // handle dropping of texture on widget
            textureButton.subscribeToEvent(textureButton, "DragEnded", (ev: Atomic.DragEndedEvent) => {

                var importer = this.acceptAssetDrag("TextureImporter", ev);

                if (importer) {

                    var textureImporter = <ToolCore.TextureImporter>importer;
                    var asset = textureImporter.asset;

                    var texture = <Atomic.Texture2D>Atomic.cache.getResource("Texture2D", asset.path);

                    if (texture) {

                        this.material.setTexture(ev.target["tunit"], texture);
                        (<Atomic.UITextureWidget>ev.target["textureWidget"]).texture = this.getTextureThumbnail(texture);

                        this.sendEvent("InspectorProjectReference", { "path": texture.getName(), "ButtonID": texture.getName() });
                    }
                }
            });

        }

        return section;

    }

    loadProjectTechniques(directory: string, menuItem: Atomic.UIMenuItemSource) {

        var resourcePath = ToolCore.toolSystem.project.getResourcePath();
        var TechniqueAssets = ToolCore.getAssetDatabase().getFolderAssets(directory);

        for (var i = 0; i < TechniqueAssets.length; i++) {

            var asset = TechniqueAssets[i];

            if (TechniqueAssets[i].isFolder()) {

                if (this.scanDirectoryForTechniques(asset.path)) {

                    var subfoldersource = new Atomic.UIMenuItemSource();
                    _ = new Atomic.UIMenuItem(TechniqueAssets[i].name);
                    _.subSource = subfoldersource;
                    menuItem.addItem(_);

                    this.loadProjectTechniques(asset.path, subfoldersource);
                }
            }
            else {
                projectTechniques[i] = TechniqueAssets[i].name;
                projectTechniquesAddress[i] = TechniqueAssets[i].path;
                menuItem.addItem(new Atomic.UIMenuItem(projectTechniques[i], projectTechniques[i]));
            }
        }
    }

    scanDirectoryForTechniques(directory: string): boolean {

        var techniqueAssets = ToolCore.getAssetDatabase().getFolderAssets(directory);

        for (var i = 0; i < techniqueAssets.length; i++) {

            var asset = techniqueAssets[i];

            if (techniqueAssets[i].isFolder()) {
                if (this.scanDirectoryForTechniques(asset.path)) {
                    return true;
                }
            }
            else if (techniqueAssets[i].getExtension() == ".xml") {
                return true;
            }
        }
        return false;
    }

    refreshTechniquesPopup() {

        techniqueSource.clear();

        _ = new Atomic.UIMenuItem("Solid");
        _.subSource = solidSource;
        techniqueSource.addItem(_);

        _ = new Atomic.UIMenuItem("Transparency");
        _.subSource = tranSource;
        techniqueSource.addItem(_);

        _ = new Atomic.UIMenuItem("Lightmap");
        _.subSource = lightmapSource;
        techniqueSource.addItem(_);

        var projectTechniquesPath = ToolCore.toolSystem.project.getResourcePath() + "Techniques";

        if (Atomic.fileSystem.dirExists(projectTechniquesPath)) {

            if (this.scanDirectoryForTechniques(projectTechniquesPath)) {

                projectSource.clear();

                _ = new Atomic.UIMenuItem("Project");
                _.subSource = projectSource;
                techniqueSource.addItem(_);

                this.loadProjectTechniques(projectTechniquesPath, projectSource);
            }
        }
    }

    inspect(asset: ToolCore.Asset, material: Atomic.Material) {
        // Add folders to resource directory

        this.asset = asset;
        this.material = material;

        var mlp = new Atomic.UILayoutParams();
        mlp.width = 310;

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
        field.skinBg = "TBAttrEditorField";
        field.fontDescription = this.fd;
        var lp = new Atomic.UILayoutParams();
        lp.width = 160;
        field.layoutParams = lp;

        field.text = Atomic.splitPath(material.name).fileName;

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

        var button = new Atomic.UIButton();
        button.fontDescription = this.fd;
        button.gravity = Atomic.UI_GRAVITY_RIGHT;
        button.text = "Save";

        button.onClick = function () {

            var importer = <ToolCore.MaterialImporter>this.asset.getImporter();
            importer.saveMaterial();

        }.bind(this);

        materialLayout.addChild(button);

        this.addChild(materialLayout);

    }

    techniqueButton: Atomic.UIButton;
    material: Atomic.Material;
    asset: ToolCore.Asset;
    nameTextField: Atomic.UITextField;
    fd: Atomic.UIFontDescription = new Atomic.UIFontDescription();


}

export = MaterialInspector;
