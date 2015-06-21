
class TextureSelector extends Atomic.UIWindow {

    constructor(parent:Atomic.UIWidget) {

        super();

        this.text = "Select Texture";

        this.rect = [0,0, 320, 512];

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

        scrollContainer.addChild(scrollLayout);

        mainLayout.addChild(scrollContainer);

        parent.addChild(this);

        this.center();

    }

}

export = TextureSelector;
