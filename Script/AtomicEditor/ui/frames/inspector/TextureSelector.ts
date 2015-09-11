//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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

        var textures = db.getAssetsByImporterType("TextureImporter");

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
