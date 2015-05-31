

class ScriptWidget extends Atomic.UIWidget {

    constructor() {

        super();

        // JS way of binding method
        // this.subscribeToEvent(this, "WidgetEvent", this.handleWidgetEvent.bind(this));

        // TypeScript-ey
        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    onEventClick(target: Atomic.UIWidget, refid: string): boolean {

        return false;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI.EVENT_TYPE_CLICK) {

            return this.onEventClick(ev.target, ev.refid);

        }

        return false;

    }

}

export = ScriptWidget;
