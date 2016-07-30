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
import InspectorWidget = require("./InspectorWidget");
import InspectorUtils = require("./InspectorUtils");

import TextureSelector = require("./TextureSelector");

class TextureInspector extends InspectorWidget {

    constructor() {

        super();

        this.fd.id = "Vera";
        this.fd.size = 11;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        return false;

    }

    getTextureThumbnail(texture: Atomic.Texture2D): Atomic.Texture {

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

    createTextureSection(): Atomic.UISection {

        var section = new Atomic.UISection();
        section.text = "Texture Image";
        section.value = 1;
        section.fontDescription = this.fd;

        var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_CENTER;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        section.contentRoot.addChild(attrsVerticalLayout);

        var attrLayout = new Atomic.UILayout();
        attrLayout.layoutDistribution = Atomic.UI_LAYOUT_POSITION_CENTER;

        var textureWidget = new Atomic.UITextureWidget();

        var tlp = new Atomic.UILayoutParams();
        tlp.width = 200;
        tlp.height = 200;
        textureWidget.layoutParams = tlp;
        textureWidget.texture = this.getTextureThumbnail(this.texture);

        var textureButton = new Atomic.UIButton();
        textureButton.skinBg = "TBButton.flatoutline";
        textureButton["textureWidget"] = textureWidget;

        textureButton.contentRoot.addChild(textureWidget);

        attrLayout.addChild(textureButton);

        attrsVerticalLayout.addChild(attrLayout);

        return section;

    }

    inspect(texture: Atomic.Texture2D, asset: ToolCore.Asset) {

        this.texture = texture;
        this.asset = asset;
        this.importer = <ToolCore.TextureImporter>asset.importer;

        var mlp = new Atomic.UILayoutParams();
        mlp.width = 310;

        var textureLayout = new Atomic.UILayout();
        textureLayout.spacing = 4;

        textureLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        textureLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        textureLayout.layoutParams = mlp;
        textureLayout.axis = Atomic.UI_AXIS_Y;

        var textureSection = new Atomic.UISection();
        textureSection.text = "Texture";
        textureSection.value = 1;
        textureSection.fontDescription = this.fd;
        textureLayout.addChild(textureSection);

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

        field.text = Atomic.splitPath(asset.name).fileName;

        nameLayout.addChild(field);

        attrsVerticalLayout.addChild(nameLayout);

        var maxSizeLayout = new Atomic.UILayout();
        maxSizeLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        //COMPRESSION SIZE
        var maxSize = InspectorUtils.createAttrName("Max Size");
        this.populateCompressionSizeList();

        maxSizeLayout.addChild(maxSize);
        maxSizeLayout.addChild(this.compressionSize);

        attrsVerticalLayout.addChild(maxSizeLayout);
        attrsVerticalLayout.addChild(this.createApplyButton());

        textureSection.contentRoot.addChild(attrsVerticalLayout);

        textureLayout.addChild(this.createTextureSection());

        this.addChild(textureLayout);

    }

    onApply()
    {

        this.importer.setCompressedImageSize(Number(this.compressionSize.text));
        this.asset.import();
        this.asset.save();

    }

    populateCompressionSizeList()
    {
        this.compressionSize = new Atomic.UISelectDropdown();
        this.compressionSizeSource = new Atomic.UISelectItemSource();

        for (var i = 0; i < this.compressionSizes.length; i ++)
        {
            var size = new Atomic.UISelectItem();
            size.setString(this.compressionSizes[i].toString());           
            this.compressionSizeSource.addItem(size);
        }

        this.compressionSize.setSource(this.compressionSizeSource);

        if (this.importer.getCompressedImageSize() != 0)
        {
            this.compressionSize.setText(this.importer.getCompressedImageSize().toString());
        }
        else
        {
            this.compressionSize.setText("NONE");
        }
    }

    texture: Atomic.Texture2D;

    techniqueButton: Atomic.UIButton;
    material: Atomic.Material;
    asset: ToolCore.Asset;
    nameTextField: Atomic.UITextField;
    compressionSize: Atomic.UISelectDropdown;
    compressionSizeSource: Atomic.UISelectItemSource;
    fd: Atomic.UIFontDescription = new Atomic.UIFontDescription();
    importer: ToolCore.TextureImporter;
    compressionSizes: number[] = [32, 64, 128, 256, 512, 1024, 2048, 4096, 8192];

}

export = TextureInspector;
