//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            return this.onEventClick(ev.target, ev.refid);

        }

    }

}

export = ScriptWidget;
