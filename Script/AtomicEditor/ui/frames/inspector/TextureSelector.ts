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

class TextureSelector extends Atomic.UIWindow {

    constructor(parent: Atomic.UIWidget) {

        super();

        this.text = "Select Texture";

        this.rect = [0, 0, 320, 512];

        var mainLayout = new Atomic.UILayout();
        mainLayout.gravity = Atomic.UI_GRAVITY_ALL;
        mainLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_AVAILABLE;
        mainLayout.axis = Atomic.UI_AXIS_Y;
        this.contentRoot.addChild(mainLayout);

        // really want a grid container
        var scrollContainer = new Atomic.UIScrollContainer();
        scrollContainer.gravity = Atomic.UI_GRAVITY_ALL;
        scrollContainer.scrollMode = Atomic.UI_SCROLL_MODE_Y_AUTO;
        scrollContainer.adaptContentSize = true;

        var scrollLayout = new Atomic.UILayout();
        scrollLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        scrollLayout.axis = Atomic.UI_AXIS_Y;

        scrollContainer.contentRoot.addChild(scrollLayout);

        var db = ToolCore.getAssetDatabase();

        var textures = db.getAssetsByImporterType("TextureImporter", "Texture2D");

        for (var i in textures) {

            var thumbnail = textures[i].cachePath + "_thumbnail.png";
            var cache = Atomic.getResourceCache();

            var textureWidget = new Atomic.UITextureWidget();
            textureWidget.texture = <Atomic.Texture2D> cache.getTempResource("Texture2D", thumbnail);

            var tlp = new Atomic.UILayoutParams();
            tlp.width = 64;
            tlp.height = 64;
            textureWidget.layoutParams = tlp;

            scrollLayout.addChild(textureWidget);

        }

        mainLayout.addChild(scrollContainer);

        parent.addChild(this);

        this.center();

        this.subscribeToEvent("WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (ev.target != this && !this.isAncestorOf(ev.target)) {

                //this.close();

            }

        }

        return false;

    }

}

export = TextureSelector;
