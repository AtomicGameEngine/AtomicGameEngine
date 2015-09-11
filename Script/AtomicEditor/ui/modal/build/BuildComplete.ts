//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//


class BuildComplete extends Atomic.UIWindow {

    constructor(parent: Atomic.UIWidget, ev: ToolCore.BuildCompleteEvent) {

        super();

        parent.addChild(this);

        this.buildFolder = ev.buildFolder;

        this.text = ev.success ? "Build Complete" : "Build Failed";
        this.load("AtomicEditor/editor/ui/buildcomplete.tb.txt");

        this.resizeToFitContent();
        this.center();

        var reveal = <Atomic.UIButton>this.getWidget("reveal");

        if (!ev.success)
            reveal.setState(Atomic.UI_WIDGET_STATE_DISABLED, true);


        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (ev.target.id == "reveal") {

                console.log("REVEAL!");

                var utils = new Editor.FileUtils();
                utils.revealInFinder(this.buildFolder);

            }

            if (ev.target.id == "ok") {
                // passed up to build output window for now
                // which closes both
                return false;
            }

        }

        return false;
    }

    buildFolder: string;

}

export = BuildComplete;
