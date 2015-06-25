

class MainToolbar extends Atomic.UIWidget {


    constructor(parent: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/maintoolbar.tb.txt");

        parent.addChild(this);

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK && ev.target) {

            if (ev.target.id == "3d_translate" || ev.target.id == "3d_rotate" || ev.target.id == "3d_scale") {

                var mode = 1;
                if (ev.target.id == "3d_rotate" )
                  mode = 2;
                else if (ev.target.id == "3d_scale")
                  mode = 3;

                  this.sendEvent("GizmoEditModeChanged", {mode:mode});

                return true;

            } else if (ev.target.id == "maintoolbar_play") {

              new ToolCore.PlayCmd().run();
              return true;

            }

        }

    }

}



export = MainToolbar;
